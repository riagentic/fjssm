// CSV import/export — port of CFJSS::Import / Export / StrToFN
// (FJSSImport.cpp:14-342). Semicolon-separated, CRLF lines, CP1250 text.
import { FN, FN_PART } from "../type/fn.ts";
import { isFuzzyDD, isFuzzyPT, ProblemType, Project } from "../type/model.ts";
import { addJob, addMachine, addProcess, emptyProject } from "./project.ts";
import { NR_DD, strNRDouble, strNRFn } from "./format.ts";
import { decodeCp1250, encodeCp1250 } from "./cp1250.ts";

/** atol semantics: leading ws + optional sign + digits, 0 on failure */
function atol(s: string): number {
  const m = s.match(/^\s*[+-]?\d+/);
  return m ? parseInt(m[0], 10) : 0;
}

/** atof semantics */
function atof(s: string): number {
  const v = parseFloat(s);
  return Number.isFinite(v) ? v : 0;
}

/** StrToFN (FJSSImport.cpp:318-342): "a1,a2,a3,a4" */
export function strToFN(str: string): FN {
  const fn: FN = [0, 0, 0, 0];
  if (!str.length) return fn;
  let idx1 = 0;
  for (let i = 0; i < FN_PART - 1; i++) {
    // C++ str.Mid(idx1, idx2-idx1): a missing comma (idx2=-1) yields a negative
    // count → empty string → 0, and idx1 resets to 0 (idx2+1). Replicate exactly
    // so comma-short fields match ("5" → [0,0,0,5], not [5,5,5,5]).
    const idx2 = str.indexOf(",", idx1);
    fn[i] = atol(idx2 === -1 ? "" : str.substring(idx1, idx2));
    idx1 = idx2 + 1;
  }
  fn[FN_PART - 1] = atol(str.substring(idx1));
  return fn;
}

/** streaming cursor replicating FReadToSemicolon / FGoToNextLine */
class CsvCursor {
  private pos = 0;
  constructor(private text: string) {}

  /** read up to `;` or CR, consuming `;` but not CR (FJSSImport.cpp:137-160) */
  field(): string {
    let out = "";
    while (this.pos < this.text.length) {
      const ch = this.text[this.pos]!;
      this.pos++;
      if (ch === ";") break;
      if (ch === "\r") {
        this.pos--; // seek back — CR terminates the field but stays
        break;
      }
      out += ch;
    }
    return out;
  }

  /** advance past CR (+ following LF) (FJSSImport.cpp:163-184) */
  nextLine(): void {
    while (this.pos < this.text.length) {
      const ch = this.text[this.pos]!;
      this.pos++;
      if (ch === "\r") {
        if (this.pos < this.text.length && this.text[this.pos] === "\n") {
          this.pos++;
        }
        break;
      }
    }
  }
}

/** structural sanity bound — real problems are far smaller; a garbage/foreign
 *  file decoded as text yields either 0 or an absurd count, both rejected. */
const MAX_DIM = 100_000;

/** Import (FJSSImport.cpp:14-133). Assumes ops-per-job == machine count.
 *  Throws on structurally invalid input so callers surface an error dialog
 *  instead of silently loading an empty/corrupt project. */
export function importCsv(bytes: Uint8Array): Project {
  const f = new CsvCursor(decodeCp1250(bytes).replace(/\r?\n/g, "\r\n"));
  const p = emptyProject();

  f.field();
  p.name = f.field();
  f.nextLine();
  f.field();
  p.date = f.field();
  f.nextLine();
  f.field();
  p.maker = f.field();
  f.nextLine();

  f.nextLine(); // blank line

  f.field();
  p.type = atol(f.field()) as ProblemType;
  f.nextLine();
  f.field();
  const nJob = atol(f.field());
  f.nextLine();
  f.field();
  const nMachine = atol(f.field());
  f.nextLine();

  if (p.type < 0 || p.type > 3) {
    throw new Error(`csv: unknown project type ${p.type}`);
  }
  if (nJob <= 0 || nJob > MAX_DIM || nMachine <= 0 || nMachine > MAX_DIM) {
    throw new Error(
      `csv: invalid dimensions nJob=${nJob} nMachine=${nMachine}`,
    );
  }

  f.nextLine(); // blank line

  for (let i = 0; i < nMachine; i++) {
    addMachine(p, f.field());
    f.nextLine();
  }

  f.nextLine(); // blank line

  const fdd = isFuzzyDD(p.type);
  for (let i = 0; i < nJob; i++) {
    const name = f.field();
    let dd: FN = [0, 0, 0, 0];
    if (!fdd) dd[0] = atol(f.field());
    else dd = strToFN(f.field());
    const alfa = atof(f.field());
    const beta = atof(f.field());
    const lambda1 = atof(f.field());
    const lambda2 = atof(f.field());
    const lambda3 = atof(f.field());
    addJob(p, name, dd, alfa, beta, lambda1, lambda2, lambda3);
    f.nextLine();
  }

  f.nextLine(); // blank line

  const fpt = isFuzzyPT(p.type);
  for (let i = 0; i < nJob; i++) {
    for (let j = 0; j < nMachine; j++) {
      const machineIndex = atol(f.field());
      if (machineIndex < 0 || machineIndex >= nMachine) {
        throw new Error(
          `csv: op (job ${i}, ${j}) references machine ${machineIndex} of ${nMachine}`,
        );
      }
      let pt: FN = [0, 0, 0, 0];
      if (!fpt) pt[0] = atol(f.field());
      else pt = strToFN(f.field());
      addProcess(p, i, j, machineIndex, pt);
    }
    f.nextLine();
  }

  return p;
}

/** Export (FJSSImport.cpp:208-316) — CRLF + semicolons, CP1250 */
export function exportCsv(p: Project): Uint8Array {
  const lines: string[] = [];
  const legend = (label: string, data: string) =>
    lines.push(`${label};${data}`);

  legend("Název", p.name);
  legend("Datum:", p.date);
  legend("Vytvořil:", p.maker);
  lines.push("");
  legend("Typ:", String(p.type));
  legend("Jobů:", String(p.nJob));
  legend("Strojů:", String(p.nMachine));
  lines.push("");

  for (const m of p.machines) lines.push(m.name);
  lines.push("");

  for (const j of p.jobs) {
    lines.push(
      [
        j.name,
        strNRFn(p, j.dd, NR_DD),
        strNRDouble(j.alfa),
        strNRDouble(j.beta),
        strNRDouble(j.lambda1),
        strNRDouble(j.lambda2),
        strNRDouble(j.lambda3),
        "",
      ].join(";"),
    );
  }
  lines.push("");

  for (const ops of p.jobOps) {
    lines.push(
      ops.map((op) => `${op.machine};${strNRFn(p, op.pt)}`).join(";"),
    );
  }
  lines.push("");

  return encodeCp1250(lines.join("\r\n"));
}
