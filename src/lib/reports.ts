// Report grids — port of CFJSS::Generate*Report (FJSSIO.cpp:204-534).
// Each returns rows of strings exactly as the C++ fills m_areasData.
import { Project, Schedule } from "../type/model.ts";
import { CriteriaSettings } from "../type/model.ts";
import { createEngine, getVOOF } from "./engine.ts";
import { PtrSchedule } from "../type/engine.ts";
import {
  NR_DD,
  strMethod,
  strNode,
  strNRDouble,
  strNRFn,
  strNRInt,
  strType,
} from "./format.ts";
import { maxJobProcess } from "./project.ts";

export function generateProjectReport(p: Project): string[][] {
  return [
    ["Název projektu:", p.name],
    ["Datum:", p.date],
    ["Vytvořil:", p.maker],
    ["Typ problému", strType(p)],
  ];
}

export function generateJobReport(p: Project): string[][] {
  const rows: string[][] = [[
    "Index jobu",
    "Název jobu",
    "Termín dokončení",
    "Alfa",
    "Beta",
    "Lam1",
    "Lam2",
    "Lam3",
  ]];
  for (let i = 0; i < p.nJob; i++) {
    const j = p.jobs[i]!;
    rows.push([
      String(i),
      j.name,
      strNRFn(p, j.dd, NR_DD),
      strNRDouble(j.alfa),
      strNRDouble(j.beta),
      strNRDouble(j.lambda1),
      strNRDouble(j.lambda2),
      strNRDouble(j.lambda3),
    ]);
  }
  return rows;
}

export function generateMachineReport(p: Project): string[][] {
  const rows: string[][] = [["Index stroje", "Název stroje"]];
  for (let i = 0; i < p.nMachine; i++) {
    rows.push([String(i), p.machines[i]!.name]);
  }
  return rows;
}

export function generateProcessReport(p: Project): string[][] {
  const head = ["Index", "Název"];
  const max = maxJobProcess(p);
  for (let i = 0; i < max; i++) head.push(`Operace ${i + 1}`);
  const rows: string[][] = [head];

  for (let i = 0; i < p.nJob; i++) {
    const row = [String(i), p.jobs[i]!.name];
    for (const op of p.jobOps[i]!) {
      row.push(
        `${op.job},${op.process},${op.machine} (${strNRFn(p, op.pt)})`,
      );
    }
    rows.push(row);
  }
  return rows;
}

/** SolveScheduleForResult (FJSSIO.cpp:312-333) — rebuild node schedule from
 *  stored refs and evaluate; returns the live engine for r/C readout */
export function solveScheduleForResult(
  p: Project,
  crit: CriteriaSettings,
  schedule: Schedule,
) {
  const e = createEngine(p, crit);
  const ptr: PtrSchedule = {
    machines: schedule.machines.map((list) =>
      list.map((ref) => e.jobNodes[ref.job]![ref.process]!)
    ),
    voof: 0,
    makespan: [0, 0, 0, 0],
  };
  getVOOF(e, ptr);
  return { engine: e, ptr };
}

/** GenerateResultReport (FJSSIO.cpp:334-418) */
export function generateResultReport(
  p: Project,
  crit: CriteriaSettings,
  schedule: Schedule,
  selectedResult: number, // 0=SA, 1=GA
): string[][] {
  const { engine, ptr } = solveScheduleForResult(p, crit, schedule);
  const rows: string[][] = [];

  rows.push(["Název projektu: ", p.name]);
  rows.push(["Použitá heuristická metoda: ", strMethod(selectedResult)]);
  rows.push([]);
  rows.push(["Datum vytvoření", p.date]);
  rows.push(["Typ projektu", strType(p)]);
  rows.push([]);

  for (let i = 0; i < p.nJob; i++) {
    const row = [strNRInt(i), p.jobs[i]!.name];
    for (const node of engine.jobNodes[i]!) row.push(strNode(p, node, 1));
    rows.push(row);
  }
  rows.push([]);

  for (let i = 0; i < p.nMachine; i++) {
    const row = [strNRInt(i), p.machines[i]!.name];
    for (const node of ptr.machines[i]!) row.push(strNode(p, node, 1));
    rows.push(row);
  }
  rows.push([]);

  rows.push(["Hodnota učelové funkce", strNRDouble(ptr.voof)]);
  rows.push(["Makespan rozvrhu", strNRFn(p, ptr.makespan)]);

  return rows;
}

/** GenerateConvergenceReport (FJSSIO.cpp:420-449) */
export function generateConvergenceReport(convergence: number[][]): string[][] {
  const rows: string[][] = [
    ["", "Max", "", "Suma", ""],
    ["", "SA", "GA", "SA", "GA"],
  ];
  for (const row of convergence) {
    const out = [` Kriterium č. ${strNRInt(row[0]!)}`];
    for (let j = 1; j < row.length; j++) out.push(strNRDouble(row[j]!));
    rows.push(out);
  }
  return rows;
}

/** GenerateIterationReport (FJSSIO.cpp:451-479) */
export function generateIterationReport(
  iteration: number[][],
  able: number[],
  creIteration: number,
): string[][] {
  const head = ["Iterace"];
  for (const c of able) head.push(`Krit. ${strNRInt(c)}`);
  head.push("Podle krit.");
  const rows: string[][] = [head];

  for (let i = 0; i < iteration.length; i++) {
    const src = iteration[i]!;
    const row = [strNRInt((i % creIteration) + 1)];
    for (let j = 0; j < src.length - 2; j++) row.push(strNRDouble(src[j]!));
    row.push(strNRDouble(src[src.length - 2]!, 3, 0));
    rows.push(row);
  }
  return rows;
}

/** GenerateREReport (FJSSIO.cpp:481-509) */
export function generateREReport(
  re: number[][],
  iteration: number[][],
  able: number[],
  creIteration: number,
): string[][] {
  const head = ["Iterace"];
  for (const c of able) head.push(`RE Krit. ${strNRInt(c)}`);
  head.push("Podle krit.");
  const rows: string[][] = [head];

  for (let i = 0; i < re.length; i++) {
    const src = re[i]!;
    const row = [strNRInt((i % creIteration) + 1)];
    for (let j = 0; j < src.length - 2; j++) row.push(strNRDouble(src[j]!));
    const critNr = iteration[i]![iteration[i]!.length - 2]!;
    row.push(strNRDouble(critNr, 3, 0));
    rows.push(row);
  }
  return rows;
}

/** GenerateAREReport (FJSSIO.cpp:511-534) */
export function generateAREReport(
  averageRE: number[][],
  able: number[],
): string[][] {
  const head = [""];
  for (const c of able) head.push(`Kriterium č. ${strNRInt(c)}`);
  head.push("Celkem");
  const rows: string[][] = [head];

  for (let i = 0; i < averageRE.length; i++) {
    const row = [`Optimalizace užitím kr. ${strNRInt(able[i]!)}`];
    for (const v of averageRE[i]!) row.push(strNRDouble(v));
    rows.push(row);
  }
  return rows;
}
