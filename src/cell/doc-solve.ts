// Solve executors for the doc cell — SA/GA runs and both experiments.
// Each takes the live state draft `s`, snapshots the inputs, runs the ported
// engine and writes progress + results back into state.
import { DocState, idleSolving } from "../type/doc.ts";
import { AxisPoint, Project, Schedule } from "../type/model.ts";
import { PtrSchedule } from "../type/engine.ts";
import { createEngine } from "../lib/engine.ts";
import {
  makeBudgetPacer,
  makeThrottledYield,
  solveGA,
  SolveHooks,
  solveSA,
} from "../lib/solver.ts";
import {
  convergenceExperiment,
  criteriaExperiment,
} from "../lib/experiments.ts";
import { strMethod } from "../lib/format.ts";
import { ableCriteria } from "../lib/criteria.ts";
import { isFuzzyDD, isFuzzyPT } from "../type/model.ts";

/** Leave the dispatch path (5ms sync-prefix budget) before heavy solve setup. */
const offDispatchPath = (): Promise<void> =>
  new Promise((r) => setTimeout(r, 0));

const snapshot = <T>(v: T): T => JSON.parse(JSON.stringify(v)) as T;
const snapshotProject = (s: DocState): Project =>
  snapshot(s.project) as Project;

const ptrToSchedule = (ptr: PtrSchedule): Schedule => ({
  machines: ptr.machines.map((list) =>
    list.map((n) => ({ job: n.job, process: n.process }))
  ),
  voof: ptr.voof,
  makespan: [...ptr.makespan],
});

/** IsAbleResult(1|2) (FJSS.cpp:586-617) */
function ableResult(s: DocState, nType: 1 | 2): void {
  const r = s.results;
  if (nType === 1) {
    if (r.ableResult === 2 || r.ableResult === 3) r.ableResult = 3;
    else {
      r.ableResult = 1;
      r.selectedResult = 0;
    }
  } else {
    if (r.ableResult === 1 || r.ableResult === 3) r.ableResult = 3;
    else {
      r.ableResult = 2;
      r.selectedResult = 1;
    }
  }
}

/** CAxisList::Add throttling (FJSSUnit.cpp:763-775) */
function makeAxisSampler(points: AxisPoint[], minDelta: number) {
  return (x: number, y: number) => {
    const last = points[points.length - 1];
    if (last && x < last.x + minDelta) return;
    points.push({ x, y });
  };
}

function makeProgressHook(s: DocState): SolveHooks["onProgress"] {
  let lastUi = 0;
  return (p) => {
    if (Date.now() - lastUi < 100) return;
    lastUi = Date.now();
    s.solving.runtime = p.runtime;
    s.solving.x0 = p.x0;
    s.solving.xBest = p.xBest;
    s.solving.temp = p.temp;
    s.solving.actCycle = p.actCycle;
    s.solving.maxCycle = p.maxCycle;
    s.solving.gaAction = p.gaAction;
  };
}

/** Výpočet → Simulované žíhání (SolveSA driver, m_nAction = 0) */
export async function runSA(s: DocState): Promise<void> {
  if (s.solving.action !== -1) return;
  s.solving = { ...idleSolving(), action: 0, subAction: 0 };
  await offDispatchPath();
  try {
    const project = snapshotProject(s);
    const crit = snapshot(s.crit);
    const sa = snapshot(s.sa);
    const points: AxisPoint[] = [];
    const sample = makeAxisSampler(points, s.minDelta);

    const e = createEngine(project, crit);
    const best = await solveSA(e, sa, {
      yieldFn: makeThrottledYield(),
      pace: makeBudgetPacer(sa.time),
      onProgress: makeProgressHook(s),
      onSample: (t, x) => sample(t, x),
    });

    s.results.saSchedule = ptrToSchedule(best);
    ableResult(s, 1);
    s.results.axis.push({ name: strMethod(0), points });
  } finally {
    s.solving = idleSolving();
  }
}

/** Výpočet → Genetický algoritmus (SolveGA driver, m_nAction = 1) */
export async function runGA(s: DocState): Promise<void> {
  if (s.solving.action !== -1) return;
  s.solving = { ...idleSolving(), action: 1, subAction: 1 };
  await offDispatchPath();
  try {
    const project = snapshotProject(s);
    const crit = snapshot(s.crit);
    const ga = snapshot(s.ga);
    const points: AxisPoint[] = [];
    const sample = makeAxisSampler(points, s.minDelta);

    const e = createEngine(project, crit);
    const best = await solveGA(e, ga, {
      yieldFn: makeThrottledYield(),
      pace: makeBudgetPacer(ga.time),
      onProgress: makeProgressHook(s),
      onSample: (t, x) => sample(t, x),
    });

    s.results.gaSchedule = ptrToSchedule(best);
    ableResult(s, 2);
    s.results.axis.push({ name: strMethod(1), points });
  } finally {
    s.solving = idleSolving();
  }
}

/** Výpočet → Konvergence metod (ConvExpDlg.cpp:136-224, m_nAction = 2) */
export async function runConvergence(s: DocState): Promise<void> {
  if (s.solving.action !== -1) return;
  s.solving = { ...idleSolving(), action: 2 };
  await offDispatchPath();
  try {
    // the original overrides the SA/GA times permanently
    s.sa.time = s.coe.solveTime;
    s.ga.time = s.coe.solveTime;

    const project = snapshotProject(s);
    const crit = snapshot(s.crit);
    const sa = snapshot(s.sa);
    const ga = snapshot(s.ga);

    s.results.convergence = [];

    const rows = await convergenceExperiment(
      project,
      crit,
      sa,
      ga,
      s.coe.solveTime,
      s.coe.solveCount,
      s.coe.allCriterion,
      {
        yieldFn: makeThrottledYield(),
        onOverall: (pos, range) => {
          s.solving.allPos = pos;
          s.solving.allRange = range;
        },
        onPart: (pos, range) => {
          s.solving.partPos = pos;
          s.solving.partRange = range;
        },
        onStage: (subAction, critMaxOrSum, criterion) => {
          s.solving.subAction = subAction;
          s.solving.critMaxOrSum = critMaxOrSum;
          s.solving.criterion = criterion;
        },
      },
    );

    // experiment leaves the mutated criterion settings behind (C++ behavior)
    s.crit.critMaxOrSum = crit.critMaxOrSum;
    s.crit.criterion = crit.criterion;

    s.results.convergence = rows;
    s.results.ableConvResult = true;
  } finally {
    s.solving = idleSolving();
  }
}

/** Výpočet → Srovnání kritérií (CritExpDlg.cpp:44-94, m_nAction = 3) */
export async function runCriteria(s: DocState): Promise<void> {
  if (s.solving.action !== -1) return;
  s.solving = { ...idleSolving(), action: 3 };
  await offDispatchPath();
  try {
    const project = snapshotProject(s);
    const crit = snapshot(s.crit);
    const sa = snapshot(s.sa);
    const ga = snapshot(s.ga);
    const iterations = s.cre.iteration;
    const method = s.cre.method;

    s.results.iteration = [];
    s.results.re = [];
    s.results.averageRE = [];

    const result = await criteriaExperiment(
      project,
      crit,
      sa,
      ga,
      iterations,
      method,
      {
        yieldFn: makeThrottledYield(),
        onOverall: (pos, range) => {
          s.solving.allPos = pos;
          s.solving.allRange = range;
        },
        onPart: (pos, range) => {
          s.solving.partPos = pos;
          s.solving.partRange = range;
        },
      },
    );

    const able = ableCriteria(isFuzzyPT(project.type), isFuzzyDD(project.type));
    if (able.length) s.crit.criterion = able[able.length - 1]!;

    s.results.iteration = result.iteration;
    s.results.re = result.re;
    s.results.averageRE = result.averageRE;
    s.results.creIterationUsed = iterations;
    s.results.ableCritResult = true;
  } finally {
    s.solving = idleSolving();
  }
}
