// SA + GA metaheuristics — exact port of SolveSA / SolveGA + helpers
// (FJSSCompute.cpp:1324-1779). Async so progress can stream to clients;
// the algorithm structure, RNG usage and termination conditions are 1:1.
import { GAParams, SAParams } from "../type/model.ts";
import { PtrSchedule, SolveProgress } from "../type/engine.ts";
import { prepareSrand, randF, randI } from "./rand.ts";
import {
  assignSchedule,
  copySchedule,
  emptySchedule,
  Engine,
  generateBeginSchedule,
  getVOOF,
} from "./engine.ts";
import {
  getDisGraphDistance,
  getMinMaxCBN,
  vicinityS1,
} from "./neighborhood.ts";

export interface SolveHooks {
  /** UpdateDialogData equivalent — live progress for the dialog UI */
  onProgress?(p: SolveProgress): void;
  /** SaveRunningData (axis sampling / criteria-experiment sampling) */
  onSample?(exactTime: number, xBest: number, schedule: PtrSchedule): void;
  /** cooperative yield so the server event loop stays responsive */
  yieldFn?(): Promise<void>;
  /**
   * Optional pacer for a single interactive solve — spreads the run across the
   * configured time budget so the progress bar animates like the original
   * (which ran for seconds on 2002 hardware + SA's dropped busy-wait). `frac`
   * is completion in 0..1. Omitted for tests/experiments, which run flat-out.
   */
  pace?(frac: number): Promise<void>;
  /** criteria-experiment mode (m_nAction == 3): loop by iterations */
  creIterations?: number;
  creTick?(): number; // returns current m_nCREActIter
}

/**
 * Build a pacer that sleeps so real elapsed time tracks `frac × budget`, and
 * still yields for rendering when the solve runs slower than the budget. The
 * per-call sleep is capped so the UI never freezes on a huge budget.
 */
export function makeBudgetPacer(
  budgetSec: number,
): (frac: number) => Promise<void> {
  const budgetMs = Math.max(0, budgetSec) * 1000;
  const start = Date.now();
  let lastYield = start;
  return async (frac: number) => {
    const target = Math.min(1, Math.max(0, frac)) * budgetMs;
    const now = Date.now();
    const wait = target - (now - start);
    if (wait > 0) {
      await new Promise((r) => setTimeout(r, Math.min(wait, 50)));
      lastYield = Date.now();
    } else if (now - lastYield >= 40) {
      await new Promise((r) => setTimeout(r, 0)); // yield for render
      lastYield = now;
    }
  };
}

const nowSec = (): number => Math.floor(Date.now() / 1000);
/** GetRuntime — whole seconds since start (FJSSCompute.cpp:1662-1667) */
const runtime = (begin: number): number => nowSec() - begin;
/** GetExactTime — seconds + current-millis/1000 (:1669-1681) */
const exactTime = (begin: number): number =>
  nowSec() - begin + (Date.now() % 1000) / 1000;

/** yields to the event loop at most every `ms` of wall time */
export function makeThrottledYield(ms = 40): () => Promise<void> {
  let last = Date.now();
  return async () => {
    if (Date.now() - last >= ms) {
      await new Promise((r) => setTimeout(r, 0));
      last = Date.now();
    }
  };
}

const noYield = async (): Promise<void> => {};

function creActive(hooks: SolveHooks): boolean {
  return hooks.creIterations !== undefined && hooks.creTick !== undefined;
}

/** Simulated Annealing (SolveSA, FJSSCompute.cpp:1324-1454) */
export async function solveSA(
  e: Engine,
  p: SAParams,
  hooks: SolveHooks = {},
): Promise<PtrSchedule> {
  const doYield = hooks.yieldFn ?? noYield;
  prepareSrand();

  const x0 = emptySchedule();
  const x1 = emptySchedule();
  const xStar = emptySchedule();

  let t = p.temp;
  let n = p.cycle;
  const begin = nowSec();

  generateBeginSchedule(e, x0);
  let fx0 = getVOOF(e, x0);
  let fxStar = fx0;
  assignSchedule(xStar, x0);

  const cre = creActive(hooks);
  // cooling progress (0 at start → 1 at the temperature floor) for the pacer
  const coolDenom = Math.log(p.temp / Math.max(1, p.minTemp)) || 1;
  const coolFrac = (temp: number) =>
    Math.log(p.temp / Math.max(1, temp)) / coolDenom;

  do {
    const fracA = coolFrac(t);
    const fracB = coolFrac(t * p.tempCoef);
    let i = 0;
    for (; i < n; i++) {
      vicinityS1(e, x0, x1);
      const fx1 = getVOOF(e, x1);
      const delta = fx1 - fx0;

      if (delta < 0) {
        assignSchedule(x0, x1);
        fx0 = fx1;
        if (fx0 - fxStar < 0) {
          assignSchedule(xStar, x0);
          fxStar = fx0;
        }
      } else {
        if (Math.exp(-delta / t) > randF()) {
          assignSchedule(x0, x1);
          fx0 = fx1;
        }
      }

      hooks.onSample?.(exactTime(begin), fxStar, xStar);
      hooks.onProgress?.({
        runtime: runtime(begin),
        x0: fx0,
        xBest: fxStar,
        temp: t,
        actCycle: i,
        maxCycle: n,
        gaAction: 0,
      });
      if (hooks.pace) await hooks.pace(fracA + (fracB - fracA) * (i / n));
      else await doYield();

      if (runtime(begin) > p.time) break;
    }

    t = t * p.tempCoef;
    n = n + p.cycleCoef;

    hooks.onProgress?.({
      runtime: runtime(begin),
      x0: fx0,
      xBest: fxStar,
      temp: t,
      actCycle: i,
      maxCycle: n,
      gaAction: 0,
    });
  } while (
    (runtime(begin) < p.time && t > p.minTemp && !cre) ||
    (cre && hooks.creTick!() < hooks.creIterations!)
  );

  return xStar;
}

/** GenerateBeginPopulation + OrderPopulation (:1464-1509) */
function generateBeginPopulation(e: Engine, size: number): PtrSchedule[] {
  const pop: PtrSchedule[] = [];
  for (let i = 0; i < size; i++) {
    const s = emptySchedule();
    generateBeginSchedule(e, s);
    s.voof = getVOOF(e, s);
    pop.push(s);
  }
  return pop;
}

/** stable ascending sort by VOOF (bubble swap-and-restart, :1478-1509) */
function orderPopulation(pop: PtrSchedule[]): void {
  let sorting = true;
  while (sorting) {
    sorting = false;
    for (let i = 0; i < pop.length - 1; i++) {
      if (pop[i]!.voof > pop[i + 1]!.voof) {
        const t = pop[i]!;
        pop[i] = pop[i + 1]!;
        pop[i + 1] = t;
        sorting = true;
        break;
      }
    }
  }
}

/** tournament-ish selection (SelectParents, :1511-1534) */
function selectParents(
  pop: PtrSchedule[],
  selCoef: number,
): [PtrSchedule, PtrSchedule] {
  const rands: number[] = [];
  for (let i = 0; i < selCoef; i++) rands.push(randI(pop.length - 2));
  let p1 = rands[0]!;
  for (let i = 1; i < selCoef; i++) p1 = Math.min(p1, rands[i]!);
  return [pop[p1]!, pop[p1 + 1]!];
}

/** MSM mutation (MultiStepMutation, :1537-1565) */
function multiStepMutation(
  e: Engine,
  p1: PtrSchedule,
  msmCycle: number,
): PtrSchedule {
  const ml: PtrSchedule[] = [];
  const x = copySchedule(p1);
  for (let i = 0; i < msmCycle; i++) {
    getMinMaxCBN(e, p1, x, 0);
    ml.push(copySchedule(x));
  }
  return ml[ml.length - 1]!;
}

/** MSX crossover (MultiStepCrossover, :1567-1619) */
function multiStepCrossover(
  e: Engine,
  p1: PtrSchedule,
  p2: PtrSchedule,
): PtrSchedule {
  const ol: PtrSchedule[] = [copySchedule(p1), copySchedule(p2)];
  let i = 0;
  let actDistance = getDisGraphDistance(p1, p2);
  let lastDistance = 0;

  do {
    const x = copySchedule(ol[i]!);
    getMinMaxCBN(e, ol[i + 1]!, x, 1);
    ol.push(x);

    lastDistance = actDistance;
    actDistance = getDisGraphDistance(ol[i + 1]!, ol[i + 2]!);
    i++;
  } while (actDistance < lastDistance);

  return ol[ol.length - 1]!;
}

/** hill walk keeping the best seen (LocalSearch, :1621-1660) */
function localSearch(e: Engine, child: PtrSchedule, lsCycle: number): number {
  let x = copySchedule(child);
  const y = emptySchedule();
  let best = copySchedule(x);

  const fFx = getVOOF(e, x);
  let fBest = fFx;

  for (let i = 0; i < lsCycle; i++) {
    vicinityS1(e, x, y);
    const fFy = getVOOF(e, y);
    if (fFy < fBest) {
      fBest = fFy;
      best = copySchedule(y);
    }
    x = copySchedule(y);
  }

  assignSchedule(child, best);
  return fBest;
}

/** Genetic Algorithm — GA/MSX (SolveGA, FJSSCompute.cpp:1684-1779) */
export async function solveGA(
  e: Engine,
  p: GAParams,
  hooks: SolveHooks = {},
): Promise<PtrSchedule> {
  const doYield = hooks.yieldFn ?? noYield;
  prepareSrand();

  let n = 0;
  const begin = nowSec();
  const cre = creActive(hooks);

  const progress = (gaAction: number, child: number, best: number) => {
    hooks.onProgress?.({
      runtime: runtime(begin),
      x0: child,
      xBest: best,
      temp: 0,
      actCycle: n,
      maxCycle: p.cycle,
      gaAction,
    });
  };

  progress(0, 0, 0);
  const pop = generateBeginPopulation(e, p.population);
  orderPopulation(pop);

  let fChild = 0;

  do {
    progress(1, fChild, pop[0]!.voof); // selection stage
    const [p1, p2] = selectParents(pop, p.selCoef);
    const parentDistance = getDisGraphDistance(p1, p2);

    let child: PtrSchedule;
    if (parentDistance < p.minMSX) {
      progress(3, fChild, pop[0]!.voof); // MSM mutation
      child = multiStepMutation(e, p1, p.msmCycle);
    } else {
      progress(2, fChild, pop[0]!.voof); // MSX crossover
      child = multiStepCrossover(e, p1, p2);
    }

    progress(4, fChild, pop[0]!.voof); // local search
    localSearch(e, child, p.lsCycle);

    progress(5, fChild, pop[0]!.voof); // population update
    const refuse = pop[pop.length - 1]!.voof;
    fChild = getVOOF(e, child);

    if (refuse > fChild) pop[pop.length - 1] = child;
    orderPopulation(pop);

    n++;
    hooks.onSample?.(exactTime(begin), pop[0]!.voof, pop[0]!);
    if (hooks.pace) await hooks.pace(n / p.cycle);
    else await doYield();
  } while (
    (n < p.cycle && runtime(begin) < p.time && !cre) ||
    (cre && hooks.creTick!() < hooks.creIterations!)
  );

  return pop[0]!;
}
