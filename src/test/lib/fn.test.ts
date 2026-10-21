// Fuzzy arithmetic — values verified against the C++ formulas (FJSSFN.cpp)
import { assertAlmostEquals, assertEquals } from "@std/assert";
import { FN } from "../../type/fn.ts";
import {
  extendedFnFx,
  fnArea,
  fnCenter,
  fnDevi,
  fnFx,
  fnMax,
  fnMean,
  fnPlus,
  fnRDevi,
} from "../../lib/fn.ts";
import { fnLeft, fnRight, fnSpillover } from "../../lib/fn-area.ts";

const A: FN = [2, 4, 6, 8];
const B: FN = [1, 3, 5, 7];
const CRISP: FN = [5, 5, 5, 5];

Deno.test("fnPlus is component-wise", () => {
  assertEquals(fnPlus(A, B), [3, 7, 11, 15]);
});

Deno.test("fnMax is component-wise", () => {
  assertEquals(fnMax(A, B), [2, 4, 6, 8]);
  assertEquals(fnMax([1, 5, 5, 9], A), [2, 5, 6, 9]);
});

Deno.test("fnFx membership (trapezoid)", () => {
  assertEquals(fnFx(A, 2), 0);
  assertEquals(fnFx(A, 3), 0.5);
  assertEquals(fnFx(A, 5), 1);
  assertEquals(fnFx(A, 7), 0.5);
  assertEquals(fnFx(A, 8), 0);
});

Deno.test("fnMean crisp short-circuit", () => {
  assertEquals(fnMean(CRISP), 5);
});

Deno.test("fnMean symmetric trapezoid = center", () => {
  // (1/3) * (64+48+36-16-8-4)/(8+6-4-2) = (1/3)*(120/8) = 5
  assertAlmostEquals(fnMean(A), 5, 1e-12);
});

Deno.test("fnCenter", () => {
  assertEquals(fnCenter(A), 5);
});

Deno.test("fnDevi symmetric trapezoid", () => {
  // (1/6)*(512+384+288+216-64-32-16-8)/8 - 25 = (1/6)*1280/8 - 25 = 26.666..-25
  assertAlmostEquals(fnDevi(A), Math.sqrt(1280 / 48 - 25), 1e-12);
});

Deno.test("fnArea trapezoid", () => {
  // (4-2)/2 + (6-4) + (8-6)/2 = 1+2+1 = 4
  assertEquals(fnArea(A), 4);
});

Deno.test("fnRDevi finite for symmetric trapezoid", () => {
  const v = fnRDevi(A);
  if (!Number.isFinite(v)) throw new Error("RDevi not finite");
});

Deno.test("fnSpillover identical FNs equals own area", () => {
  assertAlmostEquals(fnSpillover(A, A), fnArea(A), 1e-9);
});

Deno.test("fnSpillover disjoint FNs is zero", () => {
  assertAlmostEquals(fnSpillover([0, 1, 2, 3], [10, 11, 12, 13]), 0, 1e-9);
});

Deno.test("fnSpillover overlap is symmetric", () => {
  assertAlmostEquals(fnSpillover(A, B), fnSpillover(B, A), 1e-9);
});

Deno.test("fnLeft: Ci right of Di is 0", () => {
  assertEquals(fnLeft(A, B), 0); // Ci1>=Di1 && Ci2>=Di2
});

Deno.test("fnLeft: Ci fully left of Di (no crossing)", () => {
  // Ci=[1,3], Di=[2,4]: small=(3-1)/2=1, large=(4-2)/2+(2-1)=2 → 1
  assertAlmostEquals(fnLeft([1, 3, 9, 9], [2, 4, 9, 9]), 1, 1e-12);
});

Deno.test("fnRight: Ci left of Di is 0", () => {
  assertEquals(fnRight(B, A), 0); // Ci3<=Di3 && Ci4<=Di4
});

Deno.test("extendedFnFx penalty slopes", () => {
  assertAlmostEquals(extendedFnFx(A, 1, 0.5, 0.25), 0.5 * (1 - 2), 1e-12);
  assertAlmostEquals(extendedFnFx(A, 10, 0.5, 0.25), 0.25 * (8 - 10), 1e-12);
  assertEquals(extendedFnFx(A, 5, 0.5, 0.25), 1); // intended µ(n) inside
});
