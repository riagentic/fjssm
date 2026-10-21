// Convergence & criteria experiments — port of CConvExpDlg::OnBnClickedBtSolve
// (ConvExpDlg.cpp:136-224), CCritExpDlg::OnBnClickedSolve + RE/ARE math
// (CritExpDlg.cpp:44-199), and CFJSS::SolveAmongAbleCriterion
// (FJSSCompute.cpp:708-728).
import {
  CriteriaSettings,
  GAParams,
  Project,
  SAParams,
} from "../type/model.ts";
import { createEngine, Engine, getVOOF } from "./engine.ts";
import { ableCriteria } from "./criteria.ts";
import { isFuzzyDD, isFuzzyPT } from "../type/model.ts";
import { solveGA, SolveHooks, solveSA } from "./solver.ts";
import { PtrSchedule } from "../type/engine.ts";

export interface ExperimentHooks {
  onOverall?(pos: number, range: number): void;
  onPart?(pos: number, range: number): void;
  onStage?(subAction: number, critMaxOrSum: number, criterion: number): void;
  yieldFn?(): Promise<void>;
}

/**
 * Convergence experiment: for each criterion (or just the current one),
 * solveCount× { SA,GA × Max,Sum }. Returns rows [critNr, SA-Max, GA-Max,
 * SA-Sum, GA-Sum]. Mutates `crit` and overrides sa.time/ga.time with
 * solveTime — exactly like the original.
 */
export async function convergenceExperiment(
  project: Project,
  crit: CriteriaSettings,
  sa: SAParams,
  ga: GAParams,
  solveTime: number,
  solveCount: number,
  allCriterion: boolean,
  hooks: ExperimentHooks = {},
): Promise<number[][]> {
  sa.time = solveTime;
  ga.time = solveTime;

  const able = ableCriteria(isFuzzyPT(project.type), isFuzzyDD(project.type));
  const crits = allCriterion ? able : [crit.criterion];
  const rows: number[][] = [];
  const range = crits.length * solveCount * 2;

  const solverHooks: SolveHooks = {
    yieldFn: hooks.yieldFn,
    onProgress: (p) => hooks.onPart?.(p.runtime, solveTime),
  };

  for (let i = 0; i < crits.length; i++) {
    for (let k = 0; k < solveCount; k++) {
      const row: number[] = [crits[i]!];
      for (let j = 0; j < 2; j++) {
        crit.critMaxOrSum = j as 0 | 1;
        crit.criterion = crits[i]!;

        hooks.onStage?.(0, j, crits[i]!);
        const e1 = createEngine(project, crit);
        const bestSA = await solveSA(e1, sa, solverHooks);
        row.push(bestSA.voof);

        hooks.onStage?.(1, j, crits[i]!);
        const e2 = createEngine(project, crit);
        const bestGA = await solveGA(e2, ga, solverHooks);
        row.push(bestGA.voof);

        hooks.onOverall?.(i * solveCount * 2 + k * 2 + j + 1, range);
      }
      rows.push(row);
    }
  }
  return rows;
}

/** SolveAmongAbleCriterion (FJSSCompute.cpp:708-728): evaluate the current
 *  schedule under every able criterion + the determining one */
function solveAmongAbleCriterion(
  e: Engine,
  sched: PtrSchedule,
  able: number[],
  state: { actIter: number; iterations: number; rows: number[][] },
): void {
  if (state.actIter < state.iterations) {
    const backup = e.crit.criterion;
    const row: number[] = [];
    for (const c of able) {
      e.crit.criterion = c;
      row.push(getVOOF(e, sched));
    }
    e.crit.criterion = backup;
    row.push(backup);
    row.push(getVOOF(e, sched));
    state.rows.push(row);
  }
  state.actIter++;
}

export interface CriteriaExperimentResult {
  iteration: number[][]; // raw VOOF rows
  re: number[][]; // relative errors (%)
  averageRE: number[][]; // per-criterion averages + overall column
}

/** GetRE (CritExpDlg.cpp:191-199) */
function getRE(voof: number, best: number, worst: number): number {
  const result = (100.0 * Math.abs(voof - best)) / Math.abs(worst - best);
  if (Math.abs(worst - best) === 0) return 100;
  return result;
}

/**
 * Criteria-comparison experiment: for each able criterion, run SA or GA for
 * `iterations` iterations, sampling every-criterion VOOFs each iteration;
 * then compute relative errors and their averages.
 */
export async function criteriaExperiment(
  project: Project,
  crit: CriteriaSettings,
  sa: SAParams,
  ga: GAParams,
  iterations: number,
  method: 0 | 1,
  hooks: ExperimentHooks = {},
): Promise<CriteriaExperimentResult> {
  const able = ableCriteria(isFuzzyPT(project.type), isFuzzyDD(project.type));
  const rows: number[][] = [];

  for (let i = 0; i < able.length; i++) {
    const state = { actIter: 0, iterations, rows };
    crit.criterion = able[i]!;
    const e = createEngine(project, crit);

    const solverHooks: SolveHooks = {
      yieldFn: hooks.yieldFn,
      creIterations: iterations,
      creTick: () => state.actIter,
      onSample: (_t, _x, sched) => {
        solveAmongAbleCriterion(e, sched, able, state);
        hooks.onPart?.(Math.min(state.actIter, iterations), iterations);
      },
    };

    if (method === 0) await solveSA(e, sa, solverHooks);
    else await solveGA(e, ga, solverHooks);

    hooks.onOverall?.(i + 1, able.length);
  }

  // SolveRelativeError (CritExpDlg.cpp:96-145)
  const re = rows.map((r) => r.slice());
  const nIter = iterations;
  for (let i = 0; i < able.length; i++) {
    const best: number[] = [];
    const worst: number[] = [];
    let first = true;
    for (let j = 0; j < nIter; j++) {
      for (let k = 0; k < able.length; k++) {
        const v = re[i * nIter + j]![k]!;
        if (first) {
          best[k] = v;
          worst[k] = v;
        } else {
          best[k] = Math.min(best[k]!, v);
          worst[k] = Math.max(worst[k]!, v);
        }
      }
      first = false;
    }
    for (let j = 0; j < nIter; j++) {
      for (let k = 0; k < able.length; k++) {
        re[i * nIter + j]![k] = getRE(
          re[i * nIter + j]![k]!,
          best[k]!,
          worst[k]!,
        );
      }
    }
  }

  // SolveAverageRE (CritExpDlg.cpp:147-187)
  const averageRE: number[][] = [];
  for (let i = 0; i < able.length; i++) {
    const avg: number[] = [];
    let first = true;
    for (let j = 0; j < nIter; j++) {
      for (let k = 0; k < able.length; k++) {
        if (first) avg[k] = re[i * nIter + j]![k]!;
        else avg[k] = avg[k]! + re[i * nIter + j]![k]!;
      }
      first = false;
    }
    for (let k = 0; k < able.length; k++) avg[k] = avg[k]! / nIter;
    averageRE.push(avg);
  }
  for (const row of averageRE) {
    let suma = 0;
    for (const v of row) suma += v;
    row.push(suma / able.length);
  }

  return { iteration: rows, re, averageRE };
}
