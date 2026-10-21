// Runtime disjunctive-graph types — port of CNode/CEdge/CPtrNodeArea
// (FJSSUnit.h). Nodes are shared mutable objects; schedules hold references
// into the master jobNodes structure, exactly like the C++ pointer arrays.
import { FN } from "./fn.ts";

export interface RNode {
  job: number;
  process: number;
  machine: number;
  name: string;
  pt: FN;
  r: FN; // earliest start (computed)
  C: FN; // completion (computed)
  price: number; // in-degree counter (topo-sort scratch)
  prevEdge: (REdge | null)[]; // critical predecessor, one per FN component
}

export interface REdge {
  list: number; // machine idx (dis) or job idx (con)
  item: number; // item index; -1 = start→first, -2 = last→end
  con: boolean; // true = conjunctive, false = disjunctive
  node1: RNode;
  node2: RNode;
}

/** runtime schedule (CPtrNodeArea): per machine, ordered node refs */
export interface PtrSchedule {
  machines: RNode[][];
  voof: number;
  makespan: FN;
}

/** SA/GA progress snapshot pushed to the UI (UpdateDialogData port) */
export interface SolveProgress {
  runtime: number; // whole seconds
  x0: number;
  xBest: number;
  temp: number;
  actCycle: number;
  maxCycle: number;
  gaAction: number; // 0..5 GA stage indicator
}
