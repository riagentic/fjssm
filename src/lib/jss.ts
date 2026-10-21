// Native .jss binary format — byte-exact port of the MFC CArchive
// serialization chain: CFJSS::Serialize (FJSS.cpp:758-803) →
// CMachineList/CJobList/CNodeArea::Serialize (FJSSUnit.cpp:813-972) →
// CFN::Serialize (FJSSFN.cpp:651-671).
// Little-endian; CString = length-prefixed CP1250 (byte, FF+WORD, …).
import { FN } from "../type/fn.ts";
import { ProblemType, Project } from "../type/model.ts";
import { decodeCp1250, encodeCp1250 } from "./cp1250.ts";

class Reader {
  private view: DataView;
  private pos = 0;
  constructor(private bytes: Uint8Array) {
    this.view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  }
  int32(): number {
    const v = this.view.getInt32(this.pos, true);
    this.pos += 4;
    return v;
  }
  double(): number {
    const v = this.view.getFloat64(this.pos, true);
    this.pos += 8;
    return v;
  }
  string(): string {
    // MFC CString length prefix: BYTE, or FF + WORD, or FF FF FF + DWORD
    let len = this.view.getUint8(this.pos);
    this.pos += 1;
    if (len === 0xff) {
      len = this.view.getUint16(this.pos, true);
      this.pos += 2;
      if (len === 0xffff) {
        len = this.view.getUint32(this.pos, true);
        this.pos += 4;
      }
    }
    const raw = this.bytes.subarray(this.pos, this.pos + len);
    this.pos += len;
    return decodeCp1250(raw);
  }
  fn(): FN {
    return [this.int32(), this.int32(), this.int32(), this.int32()];
  }
}

class Writer {
  private chunks: Uint8Array[] = [];
  int32(v: number): void {
    const b = new Uint8Array(4);
    new DataView(b.buffer).setInt32(0, v, true);
    this.chunks.push(b);
  }
  double(v: number): void {
    const b = new Uint8Array(8);
    new DataView(b.buffer).setFloat64(0, v, true);
    this.chunks.push(b);
  }
  string(s: string): void {
    const raw = encodeCp1250(s);
    if (raw.length < 0xff) {
      this.chunks.push(new Uint8Array([raw.length]));
    } else if (raw.length < 0xfffe) {
      const b = new Uint8Array(3);
      b[0] = 0xff;
      new DataView(b.buffer).setUint16(1, raw.length, true);
      this.chunks.push(b);
    } else {
      const b = new Uint8Array(7);
      b[0] = 0xff;
      b[1] = 0xff;
      b[2] = 0xff;
      new DataView(b.buffer).setUint32(3, raw.length, true);
      this.chunks.push(b);
    }
    this.chunks.push(raw);
  }
  fn(fn: FN): void {
    for (const v of fn) this.int32(v);
  }
  bytes(): Uint8Array {
    const total = this.chunks.reduce((n, c) => n + c.length, 0);
    const out = new Uint8Array(total);
    let pos = 0;
    for (const c of this.chunks) {
      out.set(c, pos);
      pos += c.length;
    }
    return out;
  }
}

/** structural sanity bound — reject a non-.jss / corrupt file fast with a clear
 *  signal rather than relying on an eventual out-of-bounds read. */
const MAX_DIM = 100_000;
function checkCount(v: number, what: string): number {
  if (v < 0 || v > MAX_DIM) throw new Error(`jss: invalid ${what} ${v}`);
  return v;
}

/** parse a native .jss file. Throws on structurally invalid input. */
export function parseJss(bytes: Uint8Array): Project {
  const r = new Reader(bytes);

  const name = r.string();
  const date = r.string();
  const maker = r.string();
  const type = r.int32() as ProblemType;
  const nJob = checkCount(r.int32(), "nJob");
  const nMachine = checkCount(r.int32(), "nMachine");
  if (type < 0 || type > 3) {
    throw new Error(`jss: unknown project type ${type}`);
  }

  // machine list (FJSSUnit.cpp:882-902)
  const machineCount = checkCount(r.int32(), "machineCount");
  const machines = [];
  for (let i = 0; i < machineCount; i++) {
    machines.push({ name: r.string(), color: r.int32() });
  }

  // job list (FJSSUnit.cpp:813-864): name, alfa, beta, l1-l3, color, DD
  const jobCount = checkCount(r.int32(), "jobCount");
  const jobs = [];
  for (let i = 0; i < jobCount; i++) {
    const jobName = r.string();
    const alfa = r.double();
    const beta = r.double();
    const lambda1 = r.double();
    const lambda2 = r.double();
    const lambda3 = r.double();
    const color = r.int32();
    const dd = r.fn();
    jobs.push({
      name: jobName,
      dd,
      color,
      alfa,
      beta,
      lambda1,
      lambda2,
      lambda3,
    });
  }

  // node area (FJSSUnit.cpp:904-972): per job list of ops
  const areaCount = checkCount(r.int32(), "areaCount");
  const jobOps = [];
  for (let i = 0; i < areaCount; i++) {
    const nodeCount = checkCount(r.int32(), "nodeCount");
    const ops = [];
    for (let j = 0; j < nodeCount; j++) {
      const opName = r.string();
      const job = r.int32();
      const process = r.int32();
      const machine = r.int32();
      const pt = r.fn();
      ops.push({ name: opName, job, process, machine, pt });
    }
    jobOps.push(ops);
  }

  return { name, date, maker, type, nJob, nMachine, machines, jobs, jobOps };
}

/** serialize a project to native .jss bytes */
export function writeJss(p: Project): Uint8Array {
  const w = new Writer();

  w.string(p.name);
  w.string(p.date);
  w.string(p.maker);
  w.int32(p.type);
  w.int32(p.nJob);
  w.int32(p.nMachine);

  w.int32(p.machines.length);
  for (const m of p.machines) {
    w.string(m.name);
    w.int32(m.color);
  }

  w.int32(p.jobs.length);
  for (const j of p.jobs) {
    w.string(j.name);
    w.double(j.alfa);
    w.double(j.beta);
    w.double(j.lambda1);
    w.double(j.lambda2);
    w.double(j.lambda3);
    w.int32(j.color);
    w.fn(j.dd);
  }

  w.int32(p.jobOps.length);
  for (const ops of p.jobOps) {
    w.int32(ops.length);
    for (const op of ops) {
      w.string(op.name);
      w.int32(op.job);
      w.int32(op.process);
      w.int32(op.machine);
      w.fn(op.pt);
    }
  }

  return w.bytes();
}
