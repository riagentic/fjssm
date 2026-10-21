// Fuzzy trapezoid arithmetic — exact port of CFN + free functions (FJSSFN.cpp)
import { FN, FN_PART } from "../type/fn.ts";

/** result = op1 + op2, component-wise (CFN::Plus) */
export const fnPlus = (op1: FN, op2: FN): FN => [
  op1[0] + op2[0],
  op1[1] + op2[1],
  op1[2] + op2[2],
  op1[3] + op2[3],
];

/** component-wise max (CFN::Max) */
export const fnMax = (a: FN, b: FN): FN => [
  Math.max(a[0], b[0]),
  Math.max(a[1], b[1]),
  Math.max(a[2], b[2]),
  Math.max(a[3], b[3]),
];

/** membership function µ(n) (FNFx, FJSSFN.cpp:349-369) */
export function fnFx(fn: FN, n: number): number {
  let r = 0;
  if (n <= fn[0]) r = 0;
  if (n > fn[0] && n < fn[1]) r = (n - fn[0]) / (fn[1] - fn[0]);
  if (n >= fn[1] && n <= fn[2]) r = 1;
  if (n > fn[2] && n < fn[3]) r = (fn[3] - n) / (fn[3] - fn[2]);
  if (n >= fn[3]) r = 0;
  return r;
}

/** ranking mean m(A) (CFN::Mean, FJSSFN.cpp:68-83) */
export function fnMean(a: FN): number {
  const [a1, a2, a3, a4] = a;
  if (a1 === a2 && a1 === a3 && a1 === a4) return a1;
  return (1 / 3) *
    ((a4 ** 2 + a4 * a3 + a3 ** 2 - a2 ** 2 - a1 * a2 - a1 ** 2) /
      (a4 + a3 - a2 - a1));
}

/** geometric center (CFN::Center) */
export function fnCenter(a: FN): number {
  return ((a[0] + a[3]) / 2 + (a[1] + a[2]) / 2) / 2;
}

/** standard deviation s(A) (CFN::Devi, FJSSFN.cpp:99-117) */
export function fnDevi(a: FN): number {
  const [a1, a2, a3, a4] = a;
  const meanSq = fnMean(a) ** 2;
  // C++ CFN::Devi groups left-assoc: ((1/6)*poly)/denom - meanSq
  // (FJSSFN.cpp:107-112), NOT (1/6)*(poly/denom). Matched for bit-exactness.
  const poly = a4 ** 3 + a4 ** 2 * a3 + a4 * a3 ** 2 + a3 ** 3 - a2 ** 3 -
    a2 ** 2 * a1 - a2 * a1 ** 2 - a1 ** 3;
  return Math.sqrt(((1 / 6) * poly) / (a4 + a3 - a2 - a1) - meanSq);
}

/** area under µ from m(A) to a4 (CFN::MeanRightArea, FJSSFN.cpp:552-587) */
export function fnMeanRightArea(fn: FN): number {
  const [, b, c, d] = fn;
  const mean = fnMean(fn);
  let tri1 = 0, sq1 = 0, sq2 = 0;
  if (mean < b) {
    tri1 = ((1 - fnFx(fn, mean)) * (b - mean)) / 2;
    sq1 = fnFx(fn, mean) * (b - mean);
  }
  if (mean < c) sq2 = c - Math.max(b, mean);
  const tri3 = (d - Math.max(c, mean)) / 2;
  return tri1 + sq1 + sq2 + tri3;
}

/** right standard deviation sR(A) (CFN::RDevi, FJSSFN.cpp:125-163) */
export function fnRDevi(fn: FN): number {
  const [a, b, c, d] = fn;
  const mean = fnMean(fn);
  let area1 = 0, area2 = 0;
  if (mean < b) {
    area1 = (1 / 3) * ((b ** 3 - mean ** 3) / (b - a)) -
      (1 / 2) * (a ** 2 * (b ** 2 - mean ** 2)) / (b - a);
  }
  if (mean < c) {
    area2 = 0.5 * c ** 2 - 0.5 * Math.max(b, mean) ** 2;
  }
  const area3 = (-1 / 3) * ((d ** 3 - mean ** 3) / (d - c)) +
    0.5 * ((d * (d ** 2 - mean ** 2)) / (d - c));
  return (area1 + area2 + area3 - mean) / fnMeanRightArea(fn);
}

/** support area of the trapezoid (FNArea, FJSSFN.cpp:535-550) */
export function fnArea(fn: FN): number {
  return (fn[1] - fn[0]) / 2 + (fn[2] - fn[1]) + (fn[3] - fn[2]) / 2;
}

/** avoid degenerate trapezoid after conversion (CFN::Check) */
export function fnCheck(a: FN): FN {
  if (a[0] === a[1] && a[0] === a[2] && a[0] === a[3]) {
    return [a[0], a[1], a[2], a[2] + 1];
  }
  return a;
}

/**
 * Extended membership with linear penalty slopes outside support
 * (ExtendedFNFx, FJSSFN.cpp:589-605). The original's middle-range guard is
 * `n<=a1 && n>=a4` (never true) leaving the result uninitialized; we use the
 * intended `a1<=n<=a4 → µ(n)`.
 */
export function extendedFnFx(
  fn: FN,
  n: number,
  alfa: number,
  beta: number,
): number {
  if (n < fn[0]) return alfa * (n - fn[0]);
  if (n > fn[FN_PART - 1]!) return beta * (fn[FN_PART - 1]! - n);
  return fnFx(fn, n);
}
