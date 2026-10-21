// Client-side view state kinds (SetView / dialog routing, FJSSMView.cpp)
export type ViewKind = "desktop" | "grid" | "gantt" | "chart";

export type GridKind =
  | "project"
  | "jobs"
  | "machines"
  | "processes"
  | "schedule"
  | "convergence"
  | "iteration"
  | "re"
  | "are";

export type ChartKind = "history" | "soare";

export type DialogKind =
  | null
  | "about"
  | "project"
  | "machines"
  | "jobs"
  | "processes"
  | "sa"
  | "ga"
  | "sa-params"
  | "ga-params"
  | "criteria"
  | "convert"
  | "convexp"
  | "critexp";
