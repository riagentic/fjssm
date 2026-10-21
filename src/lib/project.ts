// Project mutations & conversions — port of CFJSS add/remove/convert
// functions (FJSS.cpp:218-580)
import { FN } from "../type/fn.ts";
import {
  ConvertSettings,
  isFuzzyDD,
  isFuzzyPT,
  Job,
  Op,
  ProblemType,
  Project,
  typeProps,
} from "../type/model.ts";
import { fnCenter, fnCheck } from "./fn.ts";
import { prepareSrand, randF } from "./rand.ts";

export function emptyProject(): Project {
  return {
    name: "",
    date: strDate(),
    maker: "",
    type: 0,
    nJob: 0,
    nMachine: 0,
    machines: [],
    jobs: [],
    jobOps: [],
  };
}

/** GetStrDate (FJSS.cpp:34-47) — "d.m.yyyy" */
export function strDate(d = new Date()): string {
  return `${d.getDate()}.${d.getMonth() + 1}.${d.getFullYear()}`;
}

/** AddJob (FJSS.cpp:218-251) */
export function addJob(
  p: Project,
  name: string,
  dd: FN,
  alfa = 0,
  beta = 0,
  lambda1 = 0,
  lambda2 = 0,
  lambda3 = 0,
): void {
  const { ddPart } = typeProps(p.type);
  const job: Job = {
    name,
    dd: [0, 0, 0, 0],
    color: p.nJob % 20,
    alfa,
    beta,
    lambda1,
    lambda2,
    lambda3,
  };
  for (let i = 0; i < ddPart; i++) job.dd[i] = dd[i]!;
  p.jobs.push(job);
  p.jobOps.push([]);
  p.nJob++;
}

/** AddMachine (FJSS.cpp:254-272) */
export function addMachine(p: Project, name: string): void {
  p.machines.push({ name, color: p.nMachine % 20 });
  p.nMachine++;
}

/** AddProcess (FJSS.cpp:301-325) */
export function addProcess(
  p: Project,
  jobIndex: number,
  processIndex: number,
  machineIndex: number,
  pt: FN,
): void {
  const { ptPart } = typeProps(p.type);
  const ops = p.jobOps[jobIndex]!;
  const op: Op = {
    name: `Operace ${jobIndex}-${ops.length}`,
    job: jobIndex,
    process: processIndex,
    machine: machineIndex,
    pt: [0, 0, 0, 0],
  };
  for (let i = 0; i < ptPart; i++) op.pt[i] = pt[i]!;
  ops.push(op);
}

/** MoveProcessUp/Down (FJSS.cpp:274-299) */
export function moveProcess(
  p: Project,
  job: number,
  process: number,
  dir: -1 | 1,
): void {
  const ops = p.jobOps[job]!;
  const node = ops[process]!;
  ops.splice(process, 1);
  ops.splice(process + dir, 0, node);
  ops[process + dir]!.process = process + dir;
  ops[process]!.process = process;
}

/** RemoveJob (FJSS.cpp:364-384) */
export function removeJob(p: Project, job: number): void {
  p.jobs.splice(job, 1);
  p.jobOps.splice(job, 1);
  for (let i = job; i < p.jobOps.length; i++) {
    for (const op of p.jobOps[i]!) op.job--;
  }
  p.nJob--;
}

/** RemoveMachine (FJSS.cpp:328-361) — drops its ops, renumbers */
export function removeMachine(p: Project, machine: number): void {
  p.machines.splice(machine, 1);
  for (let i = p.jobOps.length - 1; i > -1; i--) {
    const ops = p.jobOps[i]!;
    for (let j = ops.length - 1; j > -1; j--) {
      if (ops[j]!.machine === machine) ops.splice(j, 1);
      else if (ops[j]!.machine > machine) ops[j]!.machine--;
    }
  }
  for (const ops of p.jobOps) {
    for (let j = 0; j < ops.length; j++) ops[j]!.process = j;
  }
  p.nMachine--;
}

/** RemoveProcess (FJSS.cpp:386-397) */
export function removeProcess(p: Project, job: number, process: number): void {
  const ops = p.jobOps[job]!;
  ops.splice(process, 1);
  for (let i = process; i < ops.length; i++) ops[i]!.process--;
}

/** IsAbleToSolve (FJSS.cpp:618-626) */
export function isAbleToSolve(p: Project): boolean {
  if (p.nJob > 0 && p.nMachine > 0) {
    for (let i = 0; i < p.nJob; i++) {
      if (p.jobOps[i]!.length > 0) return true;
    }
  }
  return false;
}

export function maxJobProcess(p: Project): number {
  let max = 0;
  for (let i = 0; i < p.nJob; i++) max = Math.max(max, p.jobOps[i]!.length);
  return max;
}

/** SetTypeMask (FJSS.cpp:164-194) — flips one fuzziness flag, returns type */
export function setTypeMask(p: Project, nCase: 0 | 1 | 2 | 3): void {
  let fdd = isFuzzyDD(p.type);
  let fpt = isFuzzyPT(p.type);
  switch (nCase) {
    case 0:
      fdd = false;
      break;
    case 1:
      fpt = false;
      break;
    case 2:
      fdd = true;
      break;
    case 3:
      fpt = true;
      break;
  }
  p.type = ((fdd ? 1 : 0) + (fpt ? 2 : 0)) as ProblemType;
}

const div = (a: number, b: number): number => Math.trunc(a / b);

/** ConvertToCrispDD (FJSS.cpp:437-451) — integer division as in C++ */
export function convertToCrispDD(p: Project): void {
  for (const job of p.jobs) {
    const hi = div(job.dd[2] + job.dd[1], 2);
    const lo = div(job.dd[3] + job.dd[0], 2);
    job.dd[0] = div(hi + lo, 2);
  }
  setTypeMask(p, 0);
}

/** ConvertToCrispPT (FJSS.cpp:452-468) */
export function convertToCrispPT(p: Project): void {
  for (const ops of p.jobOps) {
    for (const op of ops) {
      const hi = div(op.pt[2] + op.pt[1], 2);
      const lo = div(op.pt[3] + op.pt[0], 2);
      op.pt[0] = div(hi + lo, 2);
    }
  }
  setTypeMask(p, 1);
}

/** ConvertToFuzzyDD (FJSS.cpp:470-508) */
export function convertToFuzzyDD(p: Project, conv: ConvertSettings): void {
  prepareSrand();
  for (const job of p.jobs) {
    const dd = job.dd[0];
    let resultL = conv.ddCoef * dd * randF();
    let resultR = conv.ddCoef * dd * randF();
    job.dd[0] = dd - Math.trunc(resultL);
    job.dd[3] = dd + Math.trunc(resultR);
    resultL = resultL * randF();
    resultR = resultR * randF();
    job.dd[1] = dd - Math.trunc(resultL);
    job.dd[2] = dd + Math.trunc(resultR);
    job.dd = fnCheck(job.dd);
  }
  setTypeMask(p, 2);
}

/** ConvertToFuzzyPT (FJSS.cpp:510-548) */
export function convertToFuzzyPT(p: Project, conv: ConvertSettings): void {
  prepareSrand();
  for (const ops of p.jobOps) {
    for (const op of ops) {
      const pt = op.pt[0];
      let resultL = conv.ptCoef * pt * randF();
      let resultR = conv.ptCoef * pt * randF();
      op.pt[0] = pt - Math.trunc(resultL);
      op.pt[3] = pt + Math.trunc(resultR);
      resultL = resultL * randF();
      resultR = resultR * randF();
      op.pt[1] = pt - Math.trunc(resultL);
      op.pt[2] = pt + Math.trunc(resultR);
      op.pt = fnCheck(op.pt);
    }
  }
  setTypeMask(p, 3);
}

/** SetDefaultDD (FJSS.cpp:552-580) — DD from pure job end times */
export function setDefaultDD(p: Project, conv: ConvertSettings): void {
  const { ptPart } = typeProps(p.type);
  const fpt = isFuzzyPT(p.type);

  for (let i = 0; i < p.nJob; i++) {
    const ops = p.jobOps[i]!;
    if (!ops.length) continue;
    // SolvePureEndTime over this job
    const c: FN = [0, 0, 0, 0];
    for (const op of ops) {
      for (let k = 0; k < ptPart; k++) c[k] = c[k]! + op.pt[k]!;
    }
    let ci = fpt ? fnCenter(c) : c[0];
    ci = ci + conv.jobCoef * p.nJob * ci + conv.machineCoef * p.nMachine * ci;
    p.jobs[i]!.dd[0] = Math.trunc(ci);
  }

  if (isFuzzyDD(p.type)) convertToFuzzyDD(p, conv);
}
