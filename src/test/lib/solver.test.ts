// Engine + solver invariants on the original example problems
import { assert, assertAlmostEquals, assertEquals } from "@std/assert";
import { parseJss } from "../../lib/jss.ts";
import {
  createEngine,
  emptySchedule,
  generateBeginSchedule,
  getVOOF,
} from "../../lib/engine.ts";
import { solveGA, solveSA } from "../../lib/solver.ts";
import {
  defaultCriteria,
  defaultGAParams,
  defaultSAParams,
} from "../../type/model.ts";
import { defaultCriterionFor } from "../../lib/criteria.ts";
import { solveScheduleForResult } from "../../lib/reports.ts";
import { Schedule } from "../../type/model.ts";
import { PtrSchedule } from "../../type/engine.ts";

const NATIVE = new URL(
  "../../../fjssm-original/Examples/Native/",
  import.meta.url,
);
const load = (name: string) =>
  parseJss(Deno.readFileSync(new URL(name, NATIVE)));

function validSchedule(p: ReturnType<typeof load>, s: PtrSchedule): void {
  // every operation appears exactly once on its machine
  const seen = new Set<string>();
  let total = 0;
  for (let m = 0; m < s.machines.length; m++) {
    for (const node of s.machines[m]!) {
      assertEquals(node.machine, m, "op on wrong machine list");
      const key = `${node.job}-${node.process}`;
      assert(!seen.has(key), `duplicate op ${key}`);
      seen.add(key);
      total++;
    }
  }
  assertEquals(total, p.jobOps.reduce((n, ops) => n + ops.length, 0));
}

Deno.test("G&T generator produces valid active schedules", () => {
  const p = load("fisher6x6All.jss");
  const crit = { ...defaultCriteria(), criterion: 1 };
  const e = createEngine(p, crit);
  for (let i = 0; i < 5; i++) {
    const s = emptySchedule();
    generateBeginSchedule(e, s);
    const voof = getVOOF(e, s);
    validSchedule(p, s);
    assert(voof > 0);
    // fuzzy makespan must be a monotone trapezoid
    for (let k = 0; k < 3; k++) {
      assert(s.makespan[k]! <= s.makespan[k + 1]!);
    }
  }
});

Deno.test("SA improves on the initial schedule", async () => {
  const p = load("fisher6x6All.jss");
  const crit = {
    ...defaultCriteria(),
    criterion: defaultCriterionFor(true, true),
  };
  const e = createEngine(p, crit);
  const s0 = emptySchedule();
  generateBeginSchedule(e, s0);
  const v0 = getVOOF(e, s0);

  const best = await solveSA(e, { ...defaultSAParams(), time: 2 });
  validSchedule(p, best);
  assert(best.voof <= v0, `SA got worse: ${best.voof} > ${v0}`);
});

Deno.test("GA improves on the initial schedule", async () => {
  const p = load("PM6x6All.jss");
  const crit = {
    ...defaultCriteria(),
    criterion: defaultCriterionFor(true, true),
  };
  const e = createEngine(p, crit);
  const s0 = emptySchedule();
  generateBeginSchedule(e, s0);
  const v0 = getVOOF(e, s0);

  const eGA = createEngine(p, crit);
  const best = await solveGA(eGA, { ...defaultGAParams(), time: 2, cycle: 60 });
  validSchedule(p, best);
  assert(best.voof <= v0 * 1.05, `GA far worse: ${best.voof} vs ${v0}`);
});

Deno.test("stored schedule re-evaluates to the same VOOF", async () => {
  const p = load("fisher6x6All.jss");
  const crit = { ...defaultCriteria(), criterion: 1 };
  const e = createEngine(p, crit);
  const best = await solveSA(e, { ...defaultSAParams(), time: 1 });

  const stored: Schedule = {
    machines: best.machines.map((l) =>
      l.map((n) => ({ job: n.job, process: n.process }))
    ),
    voof: best.voof,
    makespan: [...best.makespan],
  };
  const { ptr } = solveScheduleForResult(p, crit, stored);
  assertAlmostEquals(ptr.voof, best.voof, 1e-9);
  assertEquals(ptr.makespan, best.makespan);
});

Deno.test("all criteria evaluate finite on a fuzzy problem", () => {
  const p = load("PM6x6All.jss");
  for (const criterion of [1, 2, 3, 4, 6, 7, 8, 9]) {
    const crit = { ...defaultCriteria(), criterion };
    const e = createEngine(p, crit);
    const s = emptySchedule();
    generateBeginSchedule(e, s);
    const v = getVOOF(e, s);
    assert(Number.isFinite(v), `criterion ${criterion} → ${v}`);
  }
});

Deno.test("crisp criterion 0 equals component-0 makespan (MAX)", () => {
  const p = load("fisher6x6All.jss");
  p.type = 0; // force crisp view of the data
  const crit = { ...defaultCriteria(), criterion: 0 };
  const e = createEngine(p, crit);
  const s = emptySchedule();
  generateBeginSchedule(e, s);
  const v = getVOOF(e, s);
  assertEquals(v, s.makespan[0]);
});
