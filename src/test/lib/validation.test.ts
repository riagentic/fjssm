// Malformed-input rejection: importCsv / parseJss must throw on structurally
// invalid input (so the UI surfaces an error dialog instead of silently
// loading an empty/corrupt project), and strToFN must match C++ CString::Mid
// on comma-short fields.
import { assert, assertEquals, assertThrows } from "@std/assert";
import { parseJss, writeJss } from "../../lib/jss.ts";
import { exportCsv, importCsv, strToFN } from "../../lib/csv.ts";
import { encodeCp1250 } from "../../lib/cp1250.ts";
import {
  addJob,
  addMachine,
  addProcess,
  emptyProject,
} from "../../lib/project.ts";

const NATIVE = new URL(
  "../../../fjssm-original/Examples/Native/",
  import.meta.url,
);
const csvBytes = (s: string) => encodeCp1250(s);

// A minimal valid crisp CSV (type 0): 1 job, 1 machine, 1 op on machine 0.
const VALID_CSV = [
  "Název;proj",
  "Datum:;2002",
  "Vytvořil:;x",
  "",
  "Typ:;0",
  "Jobů:;1",
  "Strojů:;1",
  "",
  "M0",
  "",
  "J0;5;0;0;0;0;0;",
  "",
  "0;3",
  "",
].join("\r\n");

Deno.test("importCsv accepts a minimal valid CSV", () => {
  const p = importCsv(csvBytes(VALID_CSV));
  assertEquals(p.nJob, 1);
  assertEquals(p.nMachine, 1);
  assertEquals(p.jobOps[0]!.length, 1);
  assertEquals(p.jobOps[0]![0]!.machine, 0);
});

Deno.test("importCsv rejects non-CSV garbage (no dimensions)", () => {
  // random text → atol yields 0 for the dimension fields → would silently
  // produce an empty project without validation.
  assertThrows(
    () => importCsv(csvBytes("this is not a project file at all\r\n")),
    Error,
    "invalid dimensions",
  );
});

Deno.test("importCsv rejects an out-of-range machine reference", () => {
  // op references machine 9 but only 1 machine exists.
  const bad = VALID_CSV.replace("0;3", "9;3");
  assertThrows(() => importCsv(csvBytes(bad)), Error, "machine 9");
});

Deno.test("importCsv rejects an unknown project type", () => {
  const bad = VALID_CSV.replace("Typ:;0", "Typ:;7");
  assertThrows(() => importCsv(csvBytes(bad)), Error, "unknown project type");
});

Deno.test("parseJss rejects a truncated file", () => {
  const bytes = Deno.readFileSync(new URL("fisher6x6All.jss", NATIVE));
  assertThrows(() => parseJss(bytes.subarray(0, 12)));
});

Deno.test("parseJss rejects random bytes as invalid", () => {
  // 64 bytes of 0xFF: string length prefixes / counts blow past MAX_DIM or
  // run off the end — either way it must throw, never return a bogus project.
  const junk = new Uint8Array(64).fill(0xff);
  assertThrows(() => parseJss(junk));
});

Deno.test("parseJss rejects an out-of-range project type", () => {
  // Build a valid empty project then corrupt the type dword (offset 3 length
  // prefixes of "" strings = 3 bytes, then type int32 at offset 3).
  const empty = emptyProject();
  empty.date = ""; // force all 3 header strings empty (1 len byte each)
  const bytes = writeJss(empty);
  const corrupt = bytes.slice();
  const typeOffset = 3; // name/date/maker = 3 empty CStrings, then int32 type
  new DataView(corrupt.buffer).setInt32(typeOffset, 99, true);
  assertThrows(() => parseJss(corrupt), Error, "unknown project type");
});

Deno.test("strToFN matches C++ Mid semantics on comma-short fields", () => {
  assertEquals(strToFN("1,4,5,8"), [1, 4, 5, 8]); // normal
  assertEquals(strToFN("5"), [0, 0, 0, 5]); // single value → last slot
  assertEquals(strToFN("10,20"), [10, 0, 10, 20]); // partial
  assertEquals(strToFN(""), [0, 0, 0, 0]); // empty
  assertEquals(strToFN("1,2,3,4,5"), [1, 2, 3, 4]); // extra ignored
});

Deno.test("CSV round-trips a fuzzy (type 3) project built in memory", () => {
  const p = emptyProject();
  p.name = "rt";
  p.type = 3; // fuzzy DD + fuzzy PT
  addMachine(p, "M0");
  addMachine(p, "M1");
  addJob(p, "J0", [1, 2, 3, 4]);
  addProcess(p, 0, 0, 0, [2, 3, 4, 5]);
  addProcess(p, 0, 1, 1, [1, 1, 2, 2]);

  const p2 = importCsv(exportCsv(p));
  assertEquals(p2.type, 3);
  assertEquals(p2.nJob, 1);
  assertEquals(p2.nMachine, 2);
  assertEquals(p2.jobs[0]!.dd, [1, 2, 3, 4]);
  assertEquals(p2.jobOps[0]![0]!.pt, [2, 3, 4, 5]);
  assertEquals(p2.jobOps[0]![1]!.machine, 1);
  assert(p2.jobOps[0]![1]!.pt.every((v, i) => v === [1, 1, 2, 2][i]));
});
