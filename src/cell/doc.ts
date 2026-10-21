// The document cell — the CFJSS/CFJSSMDoc state: project data, criteria,
// solver params, results. One shared source of truth, synced to all clients.
import { cell } from "aio";
import { FN } from "../type/fn.ts";
import {
  defaultConvert,
  defaultCriteria,
  defaultGAParams,
  defaultSAParams,
  isFuzzyDD,
  isFuzzyPT,
  OpEdit,
  Project,
} from "../type/model.ts";
import { DocState, emptyResults, idleSolving } from "../type/doc.ts";
import {
  addJob,
  addMachine,
  addProcess,
  convertToCrispDD,
  convertToCrispPT,
  convertToFuzzyDD,
  convertToFuzzyPT,
  emptyProject,
  moveProcess,
  removeJob,
  removeMachine,
  removeProcess,
  setDefaultDD,
} from "../lib/project.ts";
import { defaultCriterionFor } from "../lib/criteria.ts";
import { runConvergence, runCriteria, runGA, runSA } from "./doc-solve.ts";

const initialState = (): DocState => ({
  project: emptyProject(),
  crit: defaultCriteria(),
  sa: defaultSAParams(),
  ga: defaultGAParams(),
  conv: defaultConvert(),
  coe: { solveTime: 10, solveCount: 5, allCriterion: true, saveToFile: true },
  cre: { iteration: 100, method: 0, saveToFile: true },
  minDelta: 0.1,
  fileName: "",
  modified: false,
  solving: idleSolving(),
  results: emptyResults(),
});

/** SetModified(TRUE) (FJSS.cpp:805-820) — edits invalidate results */
function setModified(s: DocState): void {
  s.results.axis = [];
  s.results.ableResult = 0;
  s.results.selectedResult = 0;
  s.results.ableConvResult = false;
  s.results.ableCritResult = false;
  s.modified = true;
}

/** SetCriterionFlag side effect: current criterion snaps to first able */
function resetCriterion(s: DocState): void {
  s.crit.criterion = defaultCriterionFor(
    isFuzzyPT(s.project.type),
    isFuzzyDD(s.project.type),
  );
}

export const doc = cell("doc", {
  state: initialState(),
  // The original is an SDI app: every launch starts with a blank New project,
  // and only an explicit Save writes a .jss file. Nothing survives a restart
  // (edits, results, solver params, view state) — so persist nothing.
  persist: "none",
  methods: {
    /** Projekt → Nový (DeleteAll) */
    newProject(s) {
      Object.assign(s, initialState());
    },

    /** Projekt → Otevřít / Importovat — parsed client-side, loaded here */
    load(s, project: Project, fileName: string) {
      Object.assign(s, initialState());
      s.project = project;
      s.fileName = fileName;
      resetCriterion(s);
    },

    markSaved(s, fileName: string) {
      s.fileName = fileName;
      s.modified = false;
    },

    /** Úpravy → Projekt */
    setProjectInfo(s, name: string, maker: string) {
      s.project.name = name;
      s.project.maker = maker;
      setModified(s);
    },

    // ── Úpravy → Stroje ──
    addMachine(s, name: string) {
      addMachine(s.project, name);
      setModified(s);
    },
    updateMachine(s, index: number, name: string, color: number) {
      const m = s.project.machines[index];
      if (!m) return;
      m.name = name;
      m.color = color;
      setModified(s);
    },
    /** @returns how many machines are left — the caller re-selects with it,
     *  so it never has to read back a value the patch may not have carried. */
    removeMachine(s, index: number): number {
      removeMachine(s.project, index);
      setModified(s);
      return s.project.machines.length;
    },

    // ── Úpravy → Joby ──
    addJob(s, name: string) {
      addJob(s.project, name, [0, 0, 0, 0]);
      setModified(s);
    },
    updateJob(
      s,
      index: number,
      fields: {
        name: string;
        dd: FN;
        alfa: number;
        beta: number;
        lambda1: number;
        lambda2: number;
        lambda3: number;
        color: number;
      },
    ) {
      const j = s.project.jobs[index];
      if (!j) return;
      Object.assign(j, fields);
      setModified(s);
    },
    /** @returns how many jobs are left (see `removeMachine`). */
    removeJob(s, index: number): number {
      removeJob(s.project, index);
      setModified(s);
      return s.project.jobs.length;
    },

    // ── Úpravy → Operace ──
    addProcess(s, job: number) {
      const ops = s.project.jobOps[job];
      if (!ops) return;
      addProcess(s.project, job, ops.length, 0, [0, 0, 0, 0]);
      setModified(s);
    },
    updateProcess(
      s,
      job: number,
      process: number,
      fields: OpEdit,
    ) {
      const op = s.project.jobOps[job]?.[process];
      if (!op) return;
      Object.assign(op, fields);
      setModified(s);
    },
    removeProcess(s, job: number, process: number) {
      removeProcess(s.project, job, process);
      setModified(s);
    },
    moveProcess(s, job: number, process: number, dir: -1 | 1) {
      moveProcess(s.project, job, process, dir);
      setModified(s);
    },

    // ── Konverze ──
    convertCrispDD(s) {
      convertToCrispDD(s.project);
      resetCriterion(s);
      setModified(s);
    },
    convertCrispPT(s) {
      convertToCrispPT(s.project);
      resetCriterion(s);
      setModified(s);
    },
    convertFuzzyDD(s) {
      convertToFuzzyDD(s.project, s.conv);
      resetCriterion(s);
      setModified(s);
    },
    convertFuzzyPT(s) {
      convertToFuzzyPT(s.project, s.conv);
      resetCriterion(s);
      setModified(s);
    },
    convertDefaultDD(s) {
      setDefaultDD(s.project, s.conv);
      setModified(s);
    },
    setConvertSettings(
      s,
      v: {
        ddCoef: number;
        ptCoef: number;
        jobCoef: number;
        machineCoef: number;
      },
    ) {
      Object.assign(s.conv, v);
    },

    // ── Výpočet nastavení ──
    setCriteria(s, criterion: number, critMaxOrSum: 0 | 1, weights: {
      w1m: number;
      w2s: number;
      w1: number;
      w2: number;
      w3: number;
      w4: number;
    }) {
      s.crit.criterion = criterion;
      s.crit.critMaxOrSum = critMaxOrSum;
      Object.assign(s.crit, weights);
    },
    setSAParams(s, p: DocState["sa"]) {
      Object.assign(s.sa, p);
    },
    setGAParams(s, p: DocState["ga"]) {
      Object.assign(s.ga, p);
    },
    setCOE(s, p: Partial<DocState["coe"]>) {
      Object.assign(s.coe, p);
    },
    setCRE(s, p: Partial<DocState["cre"]>) {
      Object.assign(s.cre, p);
    },

    /** Výsledky → SA/GA radio */
    selectResult(s, which: 0 | 1) {
      s.results.selectedResult = which;
    },

    // ── Výpočty (async, stream progress) ──
    async solveSA(s) {
      await runSA(s);
    },
    async solveGA(s) {
      await runGA(s);
    },
    async solveConvergence(s) {
      await runConvergence(s);
    },
    async solveCriteria(s) {
      await runCriteria(s);
    },
  },
});
