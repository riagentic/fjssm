// English translation coverage: every Czech UI string the source shows has an
// English entry, every entry is still used, and placeholders/mnemonics agree.
import { assertEquals } from "@std/assert";
import { EN } from "../../ui/i18n-en.ts";
import { cs } from "../../lib/tr.ts";
import { strCriterion, strType } from "../../lib/format.ts";
import { generateProjectReport } from "../../lib/reports.ts";
import { Project } from "../../type/model.ts";

const SRC = new URL("../../", import.meta.url);

/** strings shown as-is in both languages (codes, symbols, names) */
const NEUTRAL = new Set([
  "OK",
  "Cancel",
  "Alfa",
  "Beta",
  "Lam1",
  "Lam2",
  "Lam3",
  "w1",
  "w2",
  "w3",
  "w4",
  "Fmax",
  "Fsum",
  "CPT /  *",
  "FPT /  *",
  "&Crisp DD",
  "C&risp PT",
  "&Fuzzy DD",
  "F&uzzy PT",
  "&Default DD",
  "Recent File",
  "English",
  "Čeština",
  "Static", // the original's uncaptioned GA group box
  "Param. SA",
  "Param. GA",
  "Program FJSS Maker",
  "All right reserved",
  "Copyright (c) 2002",
  "Copyright (C) 2002",
  "riagentic",
  "Job",
]);

const FILES = [
  "ui/commands.ts",
  "ui/menu-def.ts",
  "ui/status.ts",
  "ui/ChartView.tsx",
  "ui/GanttView.tsx",
  "ui/GridView.tsx",
  "lib/format.ts",
  "lib/reports.ts",
];

// drop line and block comments, leaving string and template literals intact
function stripComments(s: string): string {
  let out = "";
  for (let i = 0; i < s.length; i++) {
    const c = s[i]!;
    if (c === '"' || c === "'" || c === "`") {
      let j = i + 1;
      while (j < s.length && s[j] !== c) j += s[j] === "\\" ? 2 : 1;
      out += s.slice(i, j + 1);
      i = j;
    } else if (c === "/" && s[i + 1] === "/") {
      while (i < s.length && s[i] !== "\n") i++;
      out += "\n";
    } else if (c === "/" && s[i + 1] === "*") {
      i = s.indexOf("*/", i + 2) + 1;
    } else out += c;
  }
  return out;
}

const CZECH = /[áčďéěíňóřšťúůýžÁČĎÉĚÍŇÓŘŠŤÚŮÝŽ]/;

/** every user-visible Czech source string the UI passes through t() */
async function sourceKeys(): Promise<Set<string>> {
  const files = [...FILES];
  for (const e of Deno.readDirSync(new URL("ui/dlg/", SRC))) {
    if (e.name.endsWith(".tsx")) files.push(`ui/dlg/${e.name}`);
  }
  const keys = new Set<string>();
  const tips = new Set<string>();
  const add = (raw: string) => {
    const s = JSON.parse(`"${raw}"`) as string;
    if (s.trim() && !NEUTRAL.has(s)) keys.add(s);
  };
  const STR = String.raw`"((?:[^"\\\n]|\\.)*)"`;
  const patterns = [
    new RegExp(String.raw`\bt\(\s*${STR}`, "g"),
    new RegExp(
      String.raw`\b(?:label|help|text|title|aria-label)(?:=|:\s*)${STR}`,
      "g",
    ),
  ];
  const TABLE = new RegExp(String.raw`\[\s*${STR},\s*(?:"|\d)`, "g"); // SA/GA fields
  for (const f of files) {
    const src = stripComments(await Deno.readTextFile(new URL(f, SRC)));
    for (
      const re of f.startsWith("ui/dlg/") ? [...patterns, TABLE] : patterns
    ) {
      for (const m of src.matchAll(re)) add(m[1]!);
    }
    // any other literal with Czech letters is UI text too
    for (const m of src.matchAll(new RegExp(STR, "g"))) {
      if (CZECH.test(m[1]!)) add(m[1]!);
    }
  }
  // tips are shown trimmed
  const cmds = stripComments(
    await Deno.readTextFile(new URL("ui/commands.ts", SRC)),
  );
  for (const m of cmds.matchAll(new RegExp(String.raw`\btip:\s*${STR}`, "g"))) {
    const raw = JSON.parse(`"${m[1]!}"`) as string;
    if (raw !== raw.trim()) tips.add(raw);
    add(m[1]!.trim());
  }
  for (const tip of tips) keys.delete(tip);
  return keys;
}

Deno.test("every Czech UI string has an English translation", async () => {
  const missing = [...await sourceKeys()].filter((k) => !(k in EN));
  assertEquals(missing, []);
});

Deno.test("no JSX text node shows Czech without t()", async () => {
  const bare: string[] = [];
  const walk = async (dir: URL, rel: string) => {
    for (const e of Deno.readDirSync(dir)) {
      if (e.isDirectory) {
        await walk(new URL(`${e.name}/`, dir), `${rel}${e.name}/`);
      } else if (e.name.endsWith(".tsx")) {
        const src = stripComments(
          await Deno.readTextFile(new URL(e.name, dir)),
        );
        for (const m of src.matchAll(/>([^<>{}]*)</g)) {
          if (CZECH.test(m[1]!)) bare.push(`${rel}${e.name}: ${m[1]!.trim()}`);
        }
      }
    }
  };
  await walk(new URL("ui/", SRC), "ui/");
  assertEquals(bare, []);
});

Deno.test("every English entry is still used by the source", async () => {
  const keys = await sourceKeys();
  const unused = Object.keys(EN).filter((k) => !keys.has(k));
  assertEquals(unused, []);
});

Deno.test("translations keep placeholders, mnemonics and line breaks", () => {
  const sig = (s: string) => ({
    args: (s.match(/\{\d+\}/g) ?? []).sort().join(),
    amp: (s.match(/&/g) ?? []).length,
    nl: (s.match(/\n/g) ?? []).length,
  });
  const bad = Object.entries(EN).filter(([k, v]) =>
    JSON.stringify(sig(k)) !== JSON.stringify(sig(v))
  );
  assertEquals(bad, []);
});

Deno.test("no English value is itself a Czech key (no double translation)", () => {
  const clash = Object.values(EN).filter((v) => v in EN);
  assertEquals(clash, []);
});

Deno.test("lib output without a translator stays the original Czech", () => {
  const p = {
    name: "P",
    date: "1.1.2002",
    maker: "M",
    type: 3,
    nJob: 2,
    nMachine: 3,
  } as unknown as Project;
  assertEquals(
    strType(p, cs),
    "(2 x 3) - Fuzzy termíny dokončení i fuzzy časy operací",
  );
  assertEquals(
    strCriterion(5, 2, 1),
    "(FPT/CDD)\nF(Ci) = Ext_DD(Ci)\nVarianta Suma",
  );
  assertEquals(generateProjectReport(p)[0], ["Název projektu:", "P"]);
});

Deno.test("lib output with the English translator", () => {
  const en = (k: string, ...a: (string | number)[]) => cs(EN[k] ?? k, ...a);
  const p = { type: 0, nJob: 1, nMachine: 1 } as unknown as Project;
  assertEquals(strType(p, en), "(1 x 1) - Crisp problem");
  assertEquals(
    strCriterion(0, 2, 0, en),
    "(CPT/*)\nCrisp Makespan\nMax variant",
  );
});

export { sourceKeys };
