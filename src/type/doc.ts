// Document cell state shape — mirrors the non-serialized CFJSS runtime state
import {
  AxisPoint,
  ConvertSettings,
  CriteriaSettings,
  GAParams,
  Project,
  SAParams,
  Schedule,
} from "./model.ts";

export interface SolvingState {
  action: number; // -1 idle, 0 SA, 1 GA, 2 convergence, 3 criteria
  subAction: number; // -1 none, 0 SA running, 1 GA running
  runtime: number;
  x0: number;
  xBest: number;
  temp: number;
  actCycle: number;
  maxCycle: number;
  gaAction: number; // 0..5 GA stage
  critMaxOrSum: number; // experiment stage indicator
  criterion: number;
  partPos: number;
  partRange: number;
  allPos: number;
  allRange: number;
}

export interface AxisSeries {
  name: string;
  points: AxisPoint[];
}

export interface Results {
  ableResult: number; // 0 none, 1 SA, 2 GA, 3 both
  selectedResult: number; // 0 SA, 1 GA
  saSchedule: Schedule | null;
  gaSchedule: Schedule | null;
  axis: AxisSeries[];
  ableConvResult: boolean;
  convergence: number[][];
  ableCritResult: boolean;
  iteration: number[][];
  re: number[][];
  averageRE: number[][];
  creIterationUsed: number;
}

export interface COESettings {
  solveTime: number;
  solveCount: number;
  allCriterion: boolean;
  saveToFile: boolean;
}

export interface CRESettings {
  iteration: number;
  method: 0 | 1;
  saveToFile: boolean;
}

export type DocState = {
  project: Project;
  crit: CriteriaSettings;
  sa: SAParams;
  ga: GAParams;
  conv: ConvertSettings;
  coe: COESettings;
  cre: CRESettings;
  minDelta: number;
  fileName: string; // last opened/saved file name ("" = untitled)
  modified: boolean;
  solving: SolvingState;
  results: Results;
};

export const idleSolving = (): SolvingState => ({
  action: -1,
  subAction: -1,
  runtime: 0,
  x0: 0,
  xBest: 0,
  temp: 0,
  actCycle: 0,
  maxCycle: 0,
  gaAction: 0,
  critMaxOrSum: 0,
  criterion: 0,
  partPos: 0,
  partRange: 1,
  allPos: 0,
  allRange: 1,
});

export const emptyResults = (): Results => ({
  ableResult: 0,
  selectedResult: 0,
  saSchedule: null,
  gaSchedule: null,
  axis: [],
  ableConvResult: false,
  convergence: [],
  ableCritResult: false,
  iteration: [],
  re: [],
  averageRE: [],
  creIterationUsed: 100,
});
