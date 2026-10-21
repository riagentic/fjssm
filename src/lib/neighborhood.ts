// Neighborhood operators for SA/GA — exact port of VicinityS1,
// GetCriticalBlocks, GetMinMaxCBN, GetDisGraphDistance
// (FJSSCompute.cpp:884-1249, 1850-1860)
import { PtrSchedule, REdge } from "../type/engine.ts";
import { randI } from "./rand.ts";
import { assignSchedule, Engine, getVOOF } from "./engine.ts";

/**
 * S1 move: swap the two operations of a random disjunctive critical edge
 * (VicinityS1, :884-964). Uses the critical path from the engine's LAST
 * getVOOF call — the same cross-call state the C++ relies on.
 */
export function vicinityS1(
  e: Engine,
  source: PtrSchedule,
  destination: PtrSchedule,
): void {
  assignSchedule(destination, source);

  const disEdges: REdge[][] = [[], [], [], []];
  for (let i = 0; i < e.ptPart; i++) {
    for (const edge of e.criticalPath[i]!) {
      if (!edge.con) disEdges[i]!.push(edge);
    }
  }

  let zeroDisBlock = 0;
  for (let i = 0; i < e.ptPart; i++) {
    if (!disEdges[i]!.length) zeroDisBlock++;
    if (zeroDisBlock === e.ptPart) return;
  }

  let randPart: number;
  do {
    randPart = randI(e.ptPart - 1);
  } while (!disEdges[randPart]!.length);

  const nRand = randI(disEdges[randPart]!.length - 1);
  const machine = disEdges[randPart]![nRand]!.list;
  const node = disEdges[randPart]![nRand]!.item;

  const seq = destination.machines[machine]!;
  const first = seq[node]!;
  seq[node] = seq[node + 1]!;
  seq[node + 1] = first;
}

/** partition critical paths into maximal same-machine disjunctive blocks,
 *  dropping blocks with < 3 nodes (GetCriticalBlocks, :967-1030) */
export function getCriticalBlocks(e: Engine): void {
  for (let i = 0; i < e.ptPart; i++) {
    e.blockMachine[i] = [];
    e.blockBegin[i] = [];
    e.blockEnd[i] = [];
  }

  for (let j = 0; j < e.ptPart; j++) {
    let lastMachine = -10;
    let block = -1;
    for (const edge of e.criticalPath[j]!) {
      if (!edge.con) {
        if (lastMachine === edge.list) {
          e.blockEnd[j]![block] = edge.item + 1;
        } else {
          lastMachine = edge.list;
          e.blockMachine[j]!.push(edge.list);
          e.blockBegin[j]!.push(edge.item);
          e.blockEnd[j]!.push(edge.item + 1);
          block++;
        }
      } else {
        lastMachine = -10;
      }
    }
  }

  for (let j = 0; j < e.ptPart; j++) {
    for (let i = e.blockMachine[j]!.length - 1; i > -1; i--) {
      const nodeCount = e.blockEnd[j]![i]! - e.blockBegin[j]![i]! + 1;
      if (nodeCount < 3) {
        e.blockMachine[j]!.splice(i, 1);
        e.blockBegin[j]!.splice(i, 1);
        e.blockEnd[j]!.splice(i, 1);
      }
    }
  }
}

/** positional Hamming distance over machine sequences
 *  (GetDisGraphDistance, :1850-1860) */
export function getDisGraphDistance(p1: PtrSchedule, p2: PtrSchedule): number {
  let result = 0;
  for (let i = 0; i < p1.machines.length; i++) {
    const l1 = p1.machines[i]!;
    const l2 = p2.machines[i]!;
    for (let j = 0; j < l1.length; j++) {
      if (l1[j] !== l2[j]) result++;
    }
  }
  return result;
}

/**
 * Enumerate all critical-block moves of X, keep the one that maximizes
 * (minDistType=0) or minimizes (=1) the distance to `parent`
 * (GetMinMaxCBN, :1142-1249). Mutates `x` to the best neighbor found.
 */
export function getMinMaxCBN(
  e: Engine,
  parent: PtrSchedule,
  x: PtrSchedule,
  minDistType: 0 | 1,
): number {
  const nx = {
    machines: x.machines.map((l) => l.slice()),
    voof: x.voof,
    makespan: [...x.makespan] as typeof x.makespan,
  };
  let nxBest = {
    machines: x.machines.map((l) => l.slice()),
    voof: x.voof,
    makespan: [...x.makespan] as typeof x.makespan,
  };

  let bestDist = getDisGraphDistance(parent, nx);

  getVOOF(e, x); // establishes the critical path
  getCriticalBlocks(e);

  for (let i = 0; i < e.ptPart; i++) {
    for (let j = 0; j < e.blockMachine[i]!.length; j++) {
      const machine = e.blockMachine[i]![j]!;
      const beginNode = e.blockBegin[i]![j]!;
      const endNode = e.blockEnd[i]![j]!;
      const interNode = endNode - beginNode - 1;

      for (let k = 0; k < interNode; k++) {
        const actNode = beginNode + 1 + k;
        const seq = nx.machines[machine]!;
        const data = seq[actNode]!;

        // move to block begin
        seq.splice(actNode, 1);
        seq.splice(beginNode, 0, data);
        let dist = getDisGraphDistance(parent, nx);
        if (bestDist < dist && !minDistType) {
          bestDist = dist;
          nxBest = {
            machines: nx.machines.map((l) => l.slice()),
            voof: nx.voof,
            makespan: [...nx.makespan] as typeof nx.makespan,
          };
        }
        if (bestDist > dist && minDistType) {
          bestDist = dist;
          nxBest = {
            machines: nx.machines.map((l) => l.slice()),
            voof: nx.voof,
            makespan: [...nx.makespan] as typeof nx.makespan,
          };
        }

        // move to block end
        seq.splice(beginNode, 1);
        seq.splice(endNode, 0, data);
        dist = getDisGraphDistance(parent, nx);
        if (bestDist < dist && !minDistType) {
          bestDist = dist;
          nxBest = {
            machines: nx.machines.map((l) => l.slice()),
            voof: nx.voof,
            makespan: [...nx.makespan] as typeof nx.makespan,
          };
        }
        if (bestDist > dist && minDistType) {
          bestDist = dist;
          nxBest = {
            machines: nx.machines.map((l) => l.slice()),
            voof: nx.voof,
            makespan: [...nx.makespan] as typeof nx.makespan,
          };
        }

        // undo: move back to original position
        seq.splice(endNode, 1);
        seq.splice(actNode, 0, data);
      }
    }
  }

  assignSchedule(x, nxBest);
  return bestDist;
}
