// Domain model — port of CJob/CMachine/CNode/CSchedule (FJSSUnit.h) and
// CFJSS project fields (FJSS.h)
import { FN } from "./fn.ts";

// problem type (m_nType): 0=crisp, 1=fuzzy DD, 2=fuzzy PT, 3=both
export type ProblemType = 0 | 1 | 2 | 3;

export interface Job {
  name: string;
  dd: FN; // due date
  color: number; // jobIndex % 20
  alfa: number;
  beta: number;
  lambda1: number;
  lambda2: number;
  lambda3: number;
}

export interface Machine {
  name: string;
  color: number; // machineIndex % 20
}

/** operation (CNode static part) */
export interface Op {
  job: number;
  process: number;
  machine: number;
  name: string;
  pt: FN; // processing time
}

/** the editable part of an Op — what the Process dialog sends to `updateProcess` */
export type OpEdit = Pick<Op, "name" | "machine" | "pt">;

/** a schedule: per machine, ordered (job,process) references (CScheduleArea) */
export interface ScheduleRef {
  job: number;
  process: number;
}

export interface Schedule {
  machines: ScheduleRef[][];
  voof: number;
  makespan: FN;
}

/** the base problem data (serialized part of CFJSS) */
export interface Project {
  name: string;
  date: string;
  maker: string;
  type: ProblemType;
  nJob: number;
  nMachine: number;
  machines: Machine[];
  jobs: Job[];
  jobOps: Op[][]; // [job][process]
}

/** criteria / weights settings (not serialized in .jss) */
export interface CriteriaSettings {
  criterion: number; // 0..9
  critMaxOrSum: 0 | 1; // 0=MAX, 1=SUM
  w1m: number;
  w2s: number;
  w1: number;
  w2: number;
  w3: number;
  w4: number;
}

export interface SAParams {
  time: number; // seconds
  minTemp: number;
  temp: number;
  cycle: number;
  tempCoef: number;
  cycleCoef: number;
}

export interface GAParams {
  time: number; // seconds
  population: number;
  cycle: number;
  selCoef: number;
  minMSX: number;
  msmCycle: number;
  lsCycle: number;
}

/** convergence-vs-time sample (CAxis) */
export interface AxisPoint {
  x: number; // seconds
  y: number; // VOOF
}

export const defaultCriteria = (): CriteriaSettings => ({
  criterion: 0,
  critMaxOrSum: 0,
  w1m: 1.0,
  w2s: 0.5,
  w1: 1.0,
  w2: 0.5,
  w3: 0.25,
  w4: 0.1,
});

export const defaultSAParams = (): SAParams => ({
  time: 10,
  minTemp: 10,
  temp: 1000.0,
  cycle: 100,
  tempCoef: 0.88,
  cycleCoef: 2,
});

export const defaultGAParams = (): GAParams => ({
  time: 10,
  population: 10,
  cycle: 200,
  selCoef: 2,
  minMSX: 3,
  msmCycle: 10,
  lsCycle: 40,
});

/** conversion coefficients (DeleteAllBase defaults, FJSS.cpp:50-127) */
export interface ConvertSettings {
  defaultDDCoef: number;
  ddCoef: number;
  ptCoef: number;
  jobCoef: number;
  machineCoef: number;
}

export const defaultConvert = (): ConvertSettings => ({
  defaultDDCoef: 2.2,
  ddCoef: 0.4,
  ptCoef: 0.2,
  jobCoef: 0.04,
  machineCoef: 0.02,
});

/** SetTypeProp (FJSS.cpp:196-212): components used for DD / PT per type */
export function typeProps(
  type: ProblemType,
): { ddPart: number; ptPart: number } {
  switch (type) {
    case 0:
      return { ddPart: 1, ptPart: 1 };
    case 1:
      return { ddPart: 4, ptPart: 1 };
    case 2:
      return { ddPart: 1, ptPart: 4 };
    case 3:
      return { ddPart: 4, ptPart: 4 };
    default:
      // C++ SetTypeProp leaves counts stale on unknown type (default: break);
      // fall back to crisp counts so a garbage CSV `type` degrades, not throws.
      return { ddPart: 1, ptPart: 1 };
  }
}

export const isFuzzyDD = (type: ProblemType): boolean =>
  type === 1 || type === 3;
export const isFuzzyPT = (type: ProblemType): boolean =>
  type === 2 || type === 3;
