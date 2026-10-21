// Disjunctive-graph engine — port of CFJSS compute state + graph algorithms
// (FJSSCompute.cpp:16-506). One Engine instance per solver run; it carries the
// same mutable cross-call state as the C++ member variables (critical path,
// last critical node), which the neighborhood operators rely on.
import { FN, FN_PART } from "../type/fn.ts";
import { CriteriaSettings, Project, typeProps } from "../type/model.ts";
import { PtrSchedule, REdge, RNode } from "../type/engine.ts";
import { prepareSrand, randI } from "./rand.ts";
import { solveObjectiveFunction } from "./criteria.ts";

const zeroFn = (): FN => [0, 0, 0, 0];

export interface Engine {
  project: Project;
  crit: CriteriaSettings;
  ptPart: number;
  ddPart: number;
  jobNodes: RNode[][];
  ndStart: RNode;
  ndEnd: RNode;
  criticalPath: REdge[][]; // [FN_PART][]
  blockMachine: number[][];
  blockBegin: number[][];
  blockEnd: number[][];
  lastCritNode: RNode | null;
}

function makeNode(job: number, process: number, machine: number): RNode {
  return {
    job,
    process,
    machine,
    name: "",
    pt: zeroFn(),
    r: zeroFn(),
    C: zeroFn(),
    price: 0,
    prevEdge: [null, null, null, null],
  };
}

export function createEngine(project: Project, crit: CriteriaSettings): Engine {
  const { ptPart, ddPart } = typeProps(project.type);
  const jobNodes: RNode[][] = project.jobOps.map((ops) =>
    ops.map((op) => {
      const n = makeNode(op.job, op.process, op.machine);
      n.name = op.name;
      n.pt = [...op.pt];
      return n;
    })
  );
  return {
    project,
    crit,
    ptPart,
    ddPart,
    jobNodes,
    ndStart: makeNode(-1, -1, -1),
    ndEnd: makeNode(-2, -2, -2),
    criticalPath: [[], [], [], []],
    blockMachine: [[], [], [], []],
    blockBegin: [[], [], [], []],
    blockEnd: [[], [], [], []],
    lastCritNode: null,
  };
}

export const emptySchedule = (): PtrSchedule => ({
  machines: [],
  voof: 0,
  makespan: zeroFn(),
});

/** shallow-copy machine lists (CPtrNodeArea::operator=) — node refs shared */
export const copySchedule = (s: PtrSchedule): PtrSchedule => ({
  machines: s.machines.map((l) => l.slice()),
  voof: s.voof,
  makespan: [...s.makespan],
});

export const assignSchedule = (dst: PtrSchedule, src: PtrSchedule): void => {
  dst.machines = src.machines.map((l) => l.slice());
  dst.voof = src.voof;
  dst.makespan = [...src.makespan];
};

/** SolvePureEndTime (FJSSCompute.cpp:53-74) */
export function solvePureEndTime(e: Engine): void {
  for (let i = 0; i < e.project.nJob; i++) {
    const ops = e.jobNodes[i]!;
    for (let j = 0; j < ops.length; j++) {
      for (let k = 0; k < e.ptPart; k++) {
        ops[j]!.r[k] = j ? ops[j - 1]!.C[k]! : 0;
        ops[j]!.C[k] = ops[j]!.r[k]! + ops[j]!.pt[k]!;
      }
    }
  }
}

/** Giffler–Thompson active schedule generator (ActiveScheduleAlgoritm, :77-190) */
export function activeScheduleAlgorithm(e: Engine, sched: PtrSchedule): void {
  prepareSrand();
  sched.machines = [];
  for (let i = 0; i < e.project.nMachine; i++) sched.machines.push([]);

  const cut: RNode[] = [];
  const nextProcess: number[] = new Array(e.project.nJob).fill(0);

  for (let i = 0; i < e.project.nJob; i++) {
    if (e.jobNodes[i]!.length) cut.push(e.jobNodes[i]![0]!);
  }
  for (const n of cut) {
    for (let j = 0; j < e.ptPart; j++) n.C[j] = n.r[j]! + n.pt[j]!;
  }

  while (cut.length) {
    const fCmin: FN = [-1, -1, -1, -1];
    let machineIndex = 0;
    for (const n of cut) {
      if (n.C[0]! < fCmin[0] || fCmin[0] === -1) {
        for (let k = 0; k < e.ptPart; k++) fCmin[k] = n.C[k]!;
        machineIndex = n.machine;
      }
    }

    // conflict set — replicates the original's missing-braces `break`
    // (FJSSCompute.cpp:133-143): only component 0 is ever tested
    const conflictSet: number[] = [];
    for (let i = 0; i < cut.length; i++) {
      if (cut[i]!.machine === machineIndex) {
        for (let j = 0; j < e.ptPart; j++) {
          if (cut[i]!.r[j]! <= fCmin[j]!) conflictSet.push(i);
          break;
        }
      }
    }

    const nRand = randI(conflictSet.length - 1);
    const conflictIndex = conflictSet[nRand]!;
    const chosen = cut[conflictIndex]!;
    const fC: FN = zeroFn();
    for (let k = 0; k < e.ptPart; k++) fC[k] = chosen.C[k]!;
    const conflictJob = chosen.job;

    nextProcess[conflictJob]!++;
    conflictSet.splice(nRand, 1);

    for (const ci of conflictSet) {
      const n = cut[ci]!;
      for (let j = 0; j < e.ptPart; j++) {
        n.r[j] = Math.max(n.r[j]!, fC[j]!);
        n.C[j] = n.r[j]! + n.pt[j]!;
      }
    }

    sched.machines[machineIndex]!.push(chosen);
    cut.splice(conflictIndex, 1);

    if (e.jobNodes[conflictJob]!.length > nextProcess[conflictJob]!) {
      const next = e.jobNodes[conflictJob]![nextProcess[conflictJob]!]!;
      cut.push(next);
      for (let i = 0; i < e.ptPart; i++) next.C[i] = next.r[i]! + next.pt[i]!;
    }
  }
}

/** GenerateBeginSchedule (:16-23) */
export function generateBeginSchedule(e: Engine, sched: PtrSchedule): void {
  solvePureEndTime(e);
  activeScheduleAlgorithm(e, sched);
}

function generateNodeList(e: Engine): RNode[] {
  const list: RNode[] = [e.ndStart];
  for (let i = 0; i < e.project.nJob; i++) {
    for (const n of e.jobNodes[i]!) list.push(n);
  }
  list.push(e.ndEnd);
  return list;
}

/** conjunctive (technological) edges (GenerateConEdges, :229-277) */
function generateConEdges(e: Engine): REdge[] {
  const edges: REdge[][] = [];
  for (let i = 0; i < e.project.nJob; i++) edges.push([]);

  for (let i = 0; i < e.project.nJob; i++) {
    const ops = e.jobNodes[i]!;
    if (!ops.length) {
      edges[i]!.push(
        { list: 0, item: 0, con: true, node1: e.ndStart, node2: e.ndEnd },
      );
    } else {
      edges[i]!.push(
        { list: i, item: -1, con: true, node1: e.ndStart, node2: ops[0]! },
      );
    }
  }
  for (let i = 0; i < e.project.nJob; i++) {
    const ops = e.jobNodes[i]!;
    for (let j = 0; j < ops.length - 1; j++) {
      edges[i]!.push(
        { list: i, item: j, con: true, node1: ops[j]!, node2: ops[j + 1]! },
      );
    }
  }
  for (let i = 0; i < e.project.nJob; i++) {
    const ops = e.jobNodes[i]!;
    if (ops.length) {
      edges[i]!.push({
        list: i,
        item: -2,
        con: true,
        node1: ops[ops.length - 1]!,
        node2: e.ndEnd,
      });
    }
  }
  return edges.flat();
}

/** disjunctive (machine-sequence) edges (GenerateDisEdges, :279-299) */
function generateDisEdges(sched: PtrSchedule): REdge[] {
  const edges: REdge[] = [];
  for (let i = 0; i < sched.machines.length; i++) {
    const seq = sched.machines[i]!;
    for (let j = 0; j < seq.length - 1; j++) {
      edges.push(
        { list: i, item: j, con: false, node1: seq[j]!, node2: seq[j + 1]! },
      );
    }
  }
  return edges;
}

/**
 * Kahn topological sort (OrderAndCheckCycle, :302-410).
 * Returns sorted [nodes, edges], or null on cycle.
 */
function orderAndCheckCycle(
  nodes: RNode[],
  edges: REdge[],
): { nodes: RNode[]; edges: REdge[] } | null {
  for (const n of nodes) n.price = 0;
  for (const ed of edges) ed.node2.price++;

  const mSet: RNode[] = nodes.filter((n) => !n.price);
  const outNodes: RNode[] = [];
  const outEdges: REdge[] = [];

  while (mSet.length) {
    const x = mSet[0]!;
    for (const ed of edges) {
      if (ed.node1 === x) {
        outEdges.push(ed);
        ed.node2.price--;
        if (ed.node2.price === 0) mSet.push(ed.node2);
      }
    }
    outNodes.push(x);
    mSet.shift();
  }

  if (nodes.length - outNodes.length > 0) return null;
  return { nodes: outNodes, edges: outEdges };
}

/** CPM longest-path forward pass (SolveCPM, :414-469) */
function solveCPM(e: Engine, edges: REdge[], sched: PtrSchedule): void {
  const all = generateNodeList(e);
  for (const n of [e.ndStart, e.ndEnd]) {
    n.C = zeroFn();
    n.r = zeroFn();
  }
  for (const n of all) {
    for (let j = 0; j < e.ptPart; j++) {
      n.prevEdge[j] = null;
      n.r[j] = 0;
      n.C[j] = 0;
    }
  }

  for (const ed of edges) {
    const n1 = ed.node1, n2 = ed.node2;
    for (let j = 0; j < e.ptPart; j++) {
      n1.C[j] = n1.r[j]! + n1.pt[j]!;
      // `<=` so start-edges get overwritten (original comment)
      if (n2.r[j]! <= n1.C[j]!) n2.prevEdge[j] = ed;
      n2.r[j] = Math.max(n1.C[j]!, n2.r[j]!);
      n2.C[j] = n2.r[j]! + n2.pt[j]!;
    }
  }

  for (let i = 0; i < e.ptPart; i++) sched.makespan[i] = e.ndEnd.C[i]!;
}

/** walk prevEdge chains back from the last critical node (:471-506) */
function generateCriticalPath(e: Engine): void {
  for (let i = 0; i < FN_PART; i++) e.criticalPath[i] = [];
  if (!e.lastCritNode) return;
  for (let i = 0; i < e.ptPart; i++) {
    let node: RNode = e.lastCritNode;
    const path: REdge[] = [];
    while (node.prevEdge[i]) {
      path.push(node.prevEdge[i]!);
      node = node.prevEdge[i]!.node1;
    }
    path.reverse();
    e.criticalPath[i] = path;
  }
}

/** full evaluation pipeline (GetVOOF, FJSSCompute.cpp:25-51) */
export function getVOOF(e: Engine, sched: PtrSchedule): number {
  for (;;) {
    const nodes = generateNodeList(e);
    const con = generateConEdges(e);
    const dis = generateDisEdges(sched);
    const sorted = orderAndCheckCycle(nodes, [...con, ...dis]);
    if (!sorted) {
      generateBeginSchedule(e, sched);
      continue;
    }
    solveCPM(e, sorted.edges, sched);
    const result = solveObjectiveFunction(e, sched);
    generateCriticalPath(e);
    return result;
  }
}
