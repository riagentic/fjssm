// Project mutations (the logic the doc-cell reducers delegate to) and the
// enable predicates the command layer reads: add/remove/move renumbering,
// out-of-range safety, isAbleToSolve, and per-type criterion enabling.
import { assert, assertEquals } from "@std/assert";
import {
  addJob,
  addMachine,
  addProcess,
  emptyProject,
  isAbleToSolve,
  moveProcess,
  removeJob,
  removeMachine,
  removeProcess,
} from "../../lib/project.ts";
import {
  ableCriteria,
  criterionFlags,
  defaultCriterionFor,
} from "../../lib/criteria.ts";

/** a 2-job / 3-machine crisp project, each job with one op per machine */
function sample() {
  const p = emptyProject();
  p.type = 0;
  for (let m = 0; m < 3; m++) addMachine(p, `M${m}`);
  for (let j = 0; j < 2; j++) {
    addJob(p, `J${j}`, [0, 0, 0, 0]);
    for (let m = 0; m < 3; m++) addProcess(p, j, m, m, [m + 1, 0, 0, 0]);
  }
  return p;
}

Deno.test("addMachine / addJob keep counts and palette colors consistent", () => {
  const p = emptyProject();
  addMachine(p, "M0");
  addMachine(p, "M1");
  assertEquals(p.nMachine, 2);
  assertEquals(p.machines[1]!.color, 1 % 20);
  addJob(p, "J0", [0, 0, 0, 0]);
  assertEquals(p.nJob, 1);
  assertEquals(p.jobOps.length, 1);
});

Deno.test("removeMachine drops its ops and renumbers higher machines", () => {
  const p = sample();
  removeMachine(p, 1); // drop the middle machine
  assertEquals(p.nMachine, 2);
  for (const ops of p.jobOps) {
    assertEquals(ops.length, 2); // the op on machine 1 is gone
    // remaining ops reference machines 0 and (2→1); processes recontiguous
    assertEquals(ops.map((o) => o.machine).sort(), [0, 1]);
    ops.forEach((o, i) => assertEquals(o.process, i));
  }
});

Deno.test("removeJob renumbers the job index on trailing ops", () => {
  const p = sample();
  removeJob(p, 0);
  assertEquals(p.nJob, 1);
  assertEquals(p.jobOps.length, 1);
  for (const op of p.jobOps[0]!) assertEquals(op.job, 0); // was job 1 → 0
});

Deno.test("removeProcess renumbers following processes", () => {
  const p = sample();
  removeProcess(p, 0, 1);
  const ops = p.jobOps[0]!;
  assertEquals(ops.length, 2);
  ops.forEach((o, i) => assertEquals(o.process, i));
});

Deno.test("moveProcess swaps neighbours and keeps process indices", () => {
  const p = sample();
  const before = p.jobOps[0]!.map((o) => o.machine);
  moveProcess(p, 0, 0, 1); // move first op down
  const after = p.jobOps[0]!.map((o) => o.machine);
  assertEquals(after[0], before[1]);
  assertEquals(after[1], before[0]);
  p.jobOps[0]!.forEach((o, i) => assertEquals(o.process, i));
});

Deno.test("isAbleToSolve requires jobs, machines, and at least one op", () => {
  assert(!isAbleToSolve(emptyProject()));
  const p = emptyProject();
  addMachine(p, "M0");
  addJob(p, "J0", [0, 0, 0, 0]);
  assert(!isAbleToSolve(p), "no ops yet → not solvable");
  addProcess(p, 0, 0, 0, [1, 0, 0, 0]);
  assert(isAbleToSolve(p));
});

Deno.test("criterion enabling matches the four problem types", () => {
  // crisp (type 0): only crit 0
  assertEquals(ableCriteria(false, false), [0]);
  assertEquals(defaultCriterionFor(false, false), 0);
  // fuzzy DD only (type 1): 0 and 5
  assertEquals(ableCriteria(false, true), [0, 5]);
  // fuzzy PT only (type 2): 1..4
  assertEquals(ableCriteria(true, false), [1, 2, 3, 4]);
  assertEquals(defaultCriterionFor(true, false), 1);
  // both fuzzy (type 3): 1..4 and 6..9
  assertEquals(ableCriteria(true, true), [1, 2, 3, 4, 6, 7, 8, 9]);
  // flags array is length 10, crit 5 never co-enabled with PT-fuzzy
  assertEquals(criterionFlags(true, true).length, 10);
  assertEquals(criterionFlags(true, true)[5], false);
});
