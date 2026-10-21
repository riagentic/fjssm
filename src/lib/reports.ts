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
import { cs, Tr } from "./tr.ts";

export function generateProjectReport(p: Project, t: Tr = cs): string[][] {
  return [
    [t("Název projektu:"), p.name],
    [t("Datum:"), p.date],
    [t("Vytvořil:"), p.maker],
    [t("Typ problému"), strType(p, t)],
  ];
}

export function generateJobReport(p: Project, t: Tr = cs): string[][] {
  const rows: string[][] = [[
    t("Index jobu"),
    t("Název jobu"),
    t("Termín dokončení"),
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

export function generateMachineReport(p: Project, t: Tr = cs): string[][] {
  const rows: string[][] = [[t("Index stroje"), t("Název stroje")]];
  for (let i = 0; i < p.nMachine; i++) {
    rows.push([String(i), p.machines[i]!.name]);
  }
  return rows;
}

export function generateProcessReport(p: Project, t: Tr = cs): string[][] {
  const head = ["Index", t("Název")];
  const max = maxJobProcess(p);
  for (let i = 0; i < max; i++) head.push(t("Operace {0}", i + 1));
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
  t: Tr = cs,
): string[][] {
  const { engine, ptr } = solveScheduleForResult(p, crit, schedule);
  const rows: string[][] = [];

  rows.push([t("Název projektu: "), p.name]);
  rows.push([t("Použitá heuristická metoda: "), strMethod(selectedResult, t)]);
  rows.push([]);
  rows.push([t("Datum vytvoření"), p.date]);
  rows.push([t("Typ projektu"), strType(p, t)]);
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

  rows.push([t("Hodnota učelové funkce"), strNRDouble(ptr.voof)]);
  rows.push([t("Makespan rozvrhu"), strNRFn(p, ptr.makespan)]);

  return rows;
}

/** GenerateConvergenceReport (FJSSIO.cpp:420-449) */
export function generateConvergenceReport(
  convergence: number[][],
  t: Tr = cs,
): string[][] {
  const rows: string[][] = [
    ["", "Max", "", t("Suma"), ""],
    ["", "SA", "GA", "SA", "GA"],
  ];
  for (const row of convergence) {
    const out = [" " + t("Kriterium č. {0}", strNRInt(row[0]!))];
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
  t: Tr = cs,
): string[][] {
  const head = [t("Iterace")];
  for (const c of able) head.push(t("Krit. {0}", strNRInt(c)));
  head.push(t("Podle krit."));
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
  t: Tr = cs,
): string[][] {
  const head = [t("Iterace")];
  for (const c of able) head.push(t("RE Krit. {0}", strNRInt(c)));
  head.push(t("Podle krit."));
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
  t: Tr = cs,
): string[][] {
  const head = [""];
  for (const c of able) head.push(t("Kriterium č. {0}", strNRInt(c)));
  head.push(t("Celkem"));
  const rows: string[][] = [head];

  for (let i = 0; i < averageRE.length; i++) {
    const row = [t("Optimalizace užitím kr. {0}", strNRInt(able[i]!))];
    for (const v of averageRE[i]!) row.push(strNRDouble(v));
    rows.push(row);
  }
  return rows;
}
