// File I/O — byte-exact .jss round-trip on the original C++ example files,
// CSV import of the original import samples.
import { assert, assertEquals } from "@std/assert";
import { parseJss, writeJss } from "../../lib/jss.ts";
import { exportCsv, importCsv, strToFN } from "../../lib/csv.ts";

const NATIVE = new URL(
  "../../../fjssm-original/Examples/Native/",
  import.meta.url,
);
const IMPORT = new URL(
  "../../../fjssm-original/Examples/Import/",
  import.meta.url,
);

Deno.test(".jss round-trip is byte-exact for all native examples", () => {
  let count = 0;
  for (const entry of Deno.readDirSync(NATIVE)) {
    if (!entry.name.endsWith(".jss")) continue;
    const bytes = Deno.readFileSync(new URL(entry.name, NATIVE));
    const project = parseJss(bytes);
    const out = writeJss(project);
    assertEquals(out.length, bytes.length, `${entry.name} length`);
    for (let i = 0; i < out.length; i++) {
      if (out[i] !== bytes[i]) {
        throw new Error(`${entry.name}: byte mismatch at ${i}`);
      }
    }
    count++;
  }
  assert(count >= 10, `expected >= 10 native examples, saw ${count}`);
});

Deno.test("fisher6x6All.jss parses to expected structure", () => {
  const p = parseJss(
    Deno.readFileSync(new URL("fisher6x6All.jss", NATIVE)),
  );
  assertEquals(p.name, "fisher6x6");
  assertEquals(p.type, 3);
  assertEquals(p.nJob, 6);
  assertEquals(p.nMachine, 6);
  assertEquals(p.jobOps.length, 6);
  for (const ops of p.jobOps) assertEquals(ops.length, 6);
  // trapezoids must be monotone
  for (const ops of p.jobOps) {
    for (const op of ops) {
      for (let i = 0; i < 3; i++) assert(op.pt[i]! <= op.pt[i + 1]!);
    }
  }
});

Deno.test("strToFN parses comma tuples", () => {
  assertEquals(strToFN("1,4,5,8"), [1, 4, 5, 8]);
  assertEquals(strToFN(""), [0, 0, 0, 0]);
});

Deno.test("CSV import of original samples", async () => {
  // find any .csv under Examples/Import (etalon/pm subfolders)
  const found: string[] = [];
  const walk = (dir: URL) => {
    for (const e of Deno.readDirSync(dir)) {
      const sub = new URL(
        e.name + (e.isDirectory ? "/" : ""),
        dir,
      );
      if (e.isDirectory) walk(sub);
      else if (e.name.endsWith(".csv")) found.push(sub.pathname);
    }
  };
  walk(IMPORT);
  assert(found.length > 0, "no CSV samples found");

  for (const path of found) {
    const p = importCsv(Deno.readFileSync(path));
    assert(p.nJob > 0, `${path}: no jobs`);
    assert(p.nMachine > 0, `${path}: no machines`);
    assertEquals(p.jobOps.length, p.nJob);
    for (const ops of p.jobOps) assertEquals(ops.length, p.nMachine);
  }
  await Promise.resolve();
});

Deno.test("CSV export → import survives the data", () => {
  const p = parseJss(
    Deno.readFileSync(new URL("PM6x6All.jss", NATIVE)),
  );
  const csv = exportCsv(p);
  const p2 = importCsv(csv);
  assertEquals(p2.nJob, p.nJob);
  assertEquals(p2.nMachine, p.nMachine);
  assertEquals(p2.type, p.type);
  for (let i = 0; i < p.nJob; i++) {
    assertEquals(p2.jobs[i]!.name, p.jobs[i]!.name);
    assertEquals(p2.jobs[i]!.dd, p.jobs[i]!.dd);
    for (let j = 0; j < p.jobOps[i]!.length; j++) {
      assertEquals(p2.jobOps[i]![j]!.machine, p.jobOps[i]![j]!.machine);
      assertEquals(p2.jobOps[i]![j]!.pt, p.jobOps[i]![j]!.pt);
    }
  }
});
