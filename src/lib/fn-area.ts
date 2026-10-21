// Intersection-area and left/right integral distances between two trapezoids
// — exact port of FNSpillover / FNLeft / FNRight (FJSSFN.cpp:198-533)
import { FN, FN_PART } from "../type/fn.ts";
import { fnFx } from "./fn.ts";

type Part = {
  x: number;
  o: number; // owner: 0=fn1, 1=fn2
  fx: number; // µ on own FN
  fx2: number; // µ on the other FN
  fxMin: number;
  fxo0: number; // µ track of fn1 at x
  fxo1: number; // µ track of fn2 at x
  oMin: number; // owner of the min track (2 = tie)
};

/** area of the intersection min(µ1,µ2) (FNSpillover, FJSSFN.cpp:198-347) */
export function fnSpillover(fn1: FN, fn2: FN): number {
  const fns: [FN, FN] = [fn1, fn2];
  const arr: Part[] = [];
  for (let i = 0; i < FN_PART; i++) {
    arr[2 * i] = {
      x: fn1[i]!,
      o: 0,
      fx: fnFx(fn1, fn1[i]!),
      fx2: 0,
      fxMin: 0,
      fxo0: 0,
      fxo1: 0,
      oMin: 0,
    };
    arr[2 * i + 1] = {
      x: fn2[i]!,
      o: 1,
      fx: fnFx(fn2, fn2[i]!),
      fx2: 0,
      fxMin: 0,
      fxo0: 0,
      fxo1: 0,
      oMin: 0,
    };
  }

  // stable bubble sort by x (original: swap-and-restart, FJSSFN.cpp:220-243)
  let sorting = true;
  while (sorting) {
    sorting = false;
    for (let i = 0; i < 2 * FN_PART - 1; i++) {
      if (arr[i]!.x > arr[i + 1]!.x) {
        const t = arr[i]!;
        arr[i] = arr[i + 1]!;
        arr[i + 1] = t;
        sorting = true;
        break;
      }
    }
  }

  let result = 0;
  for (let i = 0; i < 2 * FN_PART - 1; i++) {
    for (let j = 0; j < 2; j++) {
      const p = arr[i + j]!;
      p.fx2 = fnFx(fns[1 - p.o as 0 | 1], p.x);
      p.fxMin = Math.min(p.fx, p.fx2);
      if (p.o === 0) {
        p.fxo0 = p.fx;
        p.fxo1 = p.fx2;
      } else {
        p.fxo0 = p.fx2;
        p.fxo1 = p.fx;
      }
      if (p.fx === p.fx2) p.oMin = 2;
      if (p.fx < p.fx2) p.oMin = p.o;
      if (p.fx > p.fx2) p.oMin = 1 - p.o;
    }

    const a = arr[i]!, b = arr[i + 1]!;
    const dist = b.x - a.x;
    const maxH = Math.max(a.fxMin, b.fxMin);
    const minH = Math.min(a.fxMin, b.fxMin);

    if (a.oMin === b.oMin || a.oMin === 2 || b.oMin === 2) {
      // (a) same owner of the min track — rectangle + triangle
      result += minH * dist + ((maxH - minH) * dist) / 2;
    } else {
      // (b) min switches owner — the two µ tracks cross inside the segment
      const k1 = (b.fxo0 - a.fxo0) / dist;
      const k2 = (b.fxo1 - a.fxo1) / dist;
      const q1 = a.fxo0;
      const q2 = a.fxo1;
      const iSect = (q2 - q1) / (k1 - k2) + a.x;
      const yISect = fnFx(fns[0], iSect);

      let square1 = 0;
      if (Math.min(a.fxMin, b.fxMin) !== 0) {
        square1 = Math.min(a.fxMin, b.fxMin) * dist;
      }

      let square2 = 0;
      if (a.fxMin !== b.fxMin) {
        square2 = a.fxMin > b.fxMin
          ? (iSect - a.x) * (a.fxMin - b.fxMin)
          : (b.x - iSect) * (b.fxMin - a.fxMin);
      }

      const leftTri = ((iSect - a.x) * Math.abs(yISect - a.fxMin)) / 2;
      const rightTri = ((b.x - iSect) * Math.abs(yISect - b.fxMin)) / 2;

      result += square1 + square2 + leftTri + rightTri;
    }
  }
  return result;
}

/** ∫max(0, L_Di(α) − L_Ci(α)) dα over the rising edges (FNLeft, :393-461) */
export function fnLeft(ci: FN, di: FN): number {
  const ci1 = ci[0], ci2 = ci[1], di1 = di[0], di2 = di[1];

  if (ci1 >= di1 && ci2 >= di2) return 0;

  if ((ci1 <= di1 && ci2 < di2) || (ci1 < di1 && ci2 <= di2)) {
    const small = (ci2 - ci1) / 2;
    const large = (di2 - di1) / 2 + (di1 - ci1);
    return large - small;
  }

  if (ci1 < di1 && ci2 > di2) {
    const kCi = 1 / (ci2 - ci1);
    const kDi = 1 / (di2 - di1);
    const qCi = fnFx(ci, di1);
    const xISect = (qCi - 0) / (kDi - kCi) + di1;
    const yISect = fnFx(ci, xISect);
    const small = ((xISect - ci1) * yISect) / 2;
    const large = ((xISect - di1) * yISect) / 2 + (di1 - ci1) * yISect;
    return large - small;
  }

  if (ci1 > di1 && ci2 < di2) {
    const kCi = 1 / (ci2 - ci1);
    const kDi = 1 / (di2 - di1);
    const qDi = fnFx(di, ci1);
    const xISect = (0 - qDi) / (kDi - kCi) + ci1;
    const yISect = fnFx(ci, xISect);
    const small = ((ci2 - xISect) * (1 - yISect)) / 2;
    const large = ((di2 - xISect) * (1 - yISect)) / 2;
    return large - small;
  }

  return 0; // unreachable for valid trapezoids (C++ falls through undefined)
}

/** ∫max(0, R_Ci(α) − R_Di(α)) dα over the falling edges (FNRight, :464-533) */
export function fnRight(ci: FN, di: FN): number {
  const ci3 = ci[2], ci4 = ci[3], di3 = di[2], di4 = di[3];

  if (ci3 <= di3 && ci4 <= di4) return 0;

  if ((ci3 >= di3 && ci4 > di4) || (ci3 > di3 && ci4 >= di4)) {
    const small = (di4 - di3) / 2;
    const large = (ci4 - ci3) / 2 + (ci3 - di3);
    return large - small;
  }

  if (ci3 < di3 && ci4 > di4) {
    const kCi = -1 / (ci4 - ci3);
    const kDi = -1 / (di4 - di3);
    const qCi = -1 + fnFx(ci, di3);
    const xISect = (qCi - 0) / (kDi - kCi) + di3;
    const yISect = fnFx(ci, xISect);
    const small = ((di4 - xISect) * yISect) / 2;
    const large = ((ci4 - xISect) * yISect) / 2;
    return large - small;
  }

  if (ci3 > di3 && ci4 < di4) {
    const kCi = -1 / (ci4 - ci3);
    const kDi = -1 / (di4 - di3);
    const qDi = -1 + fnFx(di, ci3);
    const xISect = (0 - qDi) / (kDi - kCi) + ci3;
    const yISect = fnFx(ci, xISect);
    const small = ((xISect - ci3) * (1 - yISect)) / 2;
    const large = ((xISect - di3) * (1 - yISect)) / 2;
    return large - small;
  }

  return 0; // unreachable for valid trapezoids (C++ falls through undefined)
}
