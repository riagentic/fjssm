// Objective functions 0-9 and the MAX/SUM aggregation — exact port of
// ObjectiveFunction0..9 + SolveObjectiveFunction (FJSSCompute.cpp:513-845)
import { FN, FN_PART } from "../type/fn.ts";
import { PtrSchedule } from "../type/engine.ts";
import { extendedFnFx, fnArea, fnDevi, fnMean, fnRDevi } from "./fn.ts";
import { fnLeft, fnRight, fnSpillover } from "./fn-area.ts";
import type { Engine } from "./engine.ts";

/** crit 0 — crisp makespan */
const of0 = (nCi: number): number => nCi;

/** crit 1 — fuzzy makespan mean m(Ci) */
const of1 = (ci: FN): number => fnMean(ci);

/** crit 2 — w1m(Ci) + w2s(Ci) */
const of2 = (ci: FN, w1m: number, w2s: number): number =>
  w1m * fnMean(ci) + w2s * fnDevi(ci);

/** crit 3 — w1m(Ci) + w2sR(Ci) */
const of3 = (ci: FN, w1m: number, w2s: number): number =>
  w1m * fnMean(ci) + w2s * fnRDevi(ci);

/** crit 4 — w1a4 + w2a3 + w3a2 + w4a1 */
const of4 = (
  ci: FN,
  w1: number,
  w2: number,
  w3: number,
  w4: number,
): number => w1 * ci[3] + w2 * ci[2] + w3 * ci[1] + w4 * ci[0];

/** crit 5 — −ExtendedFNFx(Di, Ci) (satisfaction, maximized) */
const of5 = (nCi: number, di: FN, alfa: number, beta: number): number =>
  -1 * extendedFnFx(di, nCi, alfa, beta);

/** crit 6 — Sakawa&Mori: −area(Ci∩Di)/area(Ci) */
const of6 = (ci: FN, di: FN): number => -1 * (fnSpillover(ci, di) / fnArea(ci));

/** crit 7 — Jaccard: −area(Ci∩Di)/area(Ci∪Di) */
const of7 = (ci: FN, di: FN): number => {
  const spill = fnSpillover(ci, di);
  return -1 * (spill / (fnArea(ci) + fnArea(di) - spill));
};

/** crit 8: area(Ci) − area(Ci∩Di) + alfa·Σ penalties
 *  (betaPenality stays 0 exactly as in the original, :677-689) */
const of8 = (ci: FN, di: FN, alfa: number, _beta: number): number => {
  let alfaPenality = 0;
  for (let i = 0; i < FN_PART; i++) {
    alfaPenality += Math.max(0, di[0] - ci[i]!);
    alfaPenality += Math.max(0, ci[i]! - di[3]);
  }
  return fnArea(ci) - fnSpillover(ci, di) + alfa * alfaPenality + 0;
};

/** crit 9 — −(alfa·spillover + beta·(1 − dist)) */
const of9 = (
  ci: FN,
  di: FN,
  alfa: number,
  beta: number,
  l1: number,
  l2: number,
  l3: number,
): number => {
  const dist = l1 * Math.abs(fnMean(di) - fnMean(ci)) +
    l2 * fnLeft(ci, di) + l3 * fnRight(ci, di);
  return -1 * (alfa * fnSpillover(ci, di) + beta * (1 - dist));
};

/** SolveObjectiveFunction (FJSSCompute.cpp:730-845) */
export function solveObjectiveFunction(e: Engine, sched: PtrSchedule): number {
  const { crit } = e;
  let result = 0;
  let maxValue = 0;
  let first = true;

  for (let i = 0; i < e.jobNodes.length; i++) {
    const ops = e.jobNodes[i]!;
    const lastIndex = ops.length - 1;
    if (lastIndex <= -1) continue;

    const last = ops[lastIndex]!;
    const ci: FN = [...last.C];
    const job = e.project.jobs[i]!;
    const di = job.dd;

    let value = 0;
    switch (crit.criterion) {
      case 0:
        value = of0(ci[0]);
        break;
      case 1:
        value = of1(ci);
        break;
      case 2:
        value = of2(ci, crit.w1m, crit.w2s);
        break;
      case 3:
        value = of3(ci, crit.w1m, crit.w2s);
        break;
      case 4:
        value = of4(ci, crit.w1, crit.w2, crit.w3, crit.w4);
        break;
      case 5:
        value = of5(ci[0], di, job.alfa, job.beta);
        break;
      case 6:
        value = of6(ci, di);
        break;
      case 7:
        value = of7(ci, di);
        break;
      case 8:
        value = of8(ci, di, job.alfa, job.beta);
        break;
      case 9:
        value = of9(
          ci,
          di,
          job.alfa,
          job.beta,
          job.lambda1,
          job.lambda2,
          job.lambda3,
        );
        break;
    }

    if (crit.critMaxOrSum === 0) {
      // MAX
      if (first) {
        result = value;
        e.lastCritNode = last;
        first = false;
      } else if (value > result) {
        result = value;
        e.lastCritNode = last;
      }
    } else {
      // SUM — lastCritNode still tracks the arg-max job (original quirk)
      if (first) {
        result = value;
        maxValue = value;
        e.lastCritNode = last;
        first = false;
      } else {
        if (value > maxValue) {
          maxValue = value;
          e.lastCritNode = last;
        }
        result += value;
      }
    }
  }

  sched.voof = result;
  return result;
}

/** SetCriterionFlag (FJSS.cpp:659-736) — which criteria the type allows */
export function criterionFlags(
  fpt: boolean,
  fdd: boolean,
): boolean[] {
  const flags = new Array(10).fill(false) as boolean[];
  if (fpt) {
    flags[1] =
      flags[2] =
      flags[3] =
      flags[4] =
        true;
    if (fdd) {
      flags[6] =
        flags[7] =
        flags[8] =
        flags[9] =
          true;
    }
  } else {
    flags[0] = true;
    if (fdd) flags[5] = true;
  }
  return flags;
}

/** the criteria list enabled for the type, in order (m_lstnAbleCriterion) */
export function ableCriteria(fpt: boolean, fdd: boolean): number[] {
  const out: number[] = [];
  criterionFlags(fpt, fdd).forEach((b, i) => {
    if (b) out.push(i);
  });
  return out;
}

/** first enabled criterion becomes the current one (FJSS.cpp:716-723) */
export function defaultCriterionFor(fpt: boolean, fdd: boolean): number {
  return ableCriteria(fpt, fdd)[0] ?? 0;
}
