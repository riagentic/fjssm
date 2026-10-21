// MSVC CRT rand()/srand() emulation — port of PrepareSrand/RandI/RandF
// (FJSSFN.cpp:612-648). The original uses the MSVC LCG; we replicate it so
// runs are statistically identical to the C++ build.

export const RAND_MAX = 0x7fff;

let seed = 1;
let lastTime = 0;

export function srand(s: number): void {
  seed = s >>> 0;
}

/** MSVC LCG: seed = seed*214013 + 2531011; return (seed>>16) & 0x7fff */
export function rand(): number {
  seed = (Math.imul(seed, 214013) + 2531011) >>> 0;
  return (seed >>> 16) & RAND_MAX;
}

/** Reseeds from wall clock at most once per second (FJSSFN.cpp:612-623). */
export function prepareSrand(): void {
  const t = Math.floor(Date.now() / 1000);
  if (t !== lastTime) {
    srand(t);
    lastTime = t;
  }
}

/**
 * Random int in [0..nMax] inclusive, round-half-up (FJSSFN.cpp:626-640).
 * Uses 32-bit float arithmetic like the original `(float)rand()/RAND_MAX*nMax`.
 */
export function randI(nMax: number): number {
  let f = Math.fround(rand());
  f = Math.fround(Math.fround(f / RAND_MAX) * nMax);
  let n = Math.trunc(f);
  if (f - n >= 0.5) n++;
  return n;
}

/** Random float in [0,1] inclusive (FJSSFN.cpp:642-648). */
export function randF(): number {
  return Math.fround(rand() / RAND_MAX);
}
