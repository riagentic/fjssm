// Per-tab view state — module-level signals (client-only UI state; the
// aio-recommended pattern for cross-component state that never syncs).
// Same surface as a cell so callers read `view.view` / call `view.showGrid()`.
import { signal } from "aio/air";
import { ChartKind, DialogKind, GridKind, ViewKind } from "../type/view.ts";

const viewKind = signal<ViewKind>("desktop");
const gridKind = signal<GridKind>("project");
const chartKind = signal<ChartKind>("history");
const dialogKind = signal<DialogKind>(null);
// modal stack — opening a dialog from a dialog (SA → Kritérium) returns to
// the parent on close, like nested DoModal in the original
let dialogStack: DialogKind[] = [];

export const view = {
  get view(): ViewKind {
    return viewKind.value;
  },
  get gridKind(): GridKind {
    return gridKind.value;
  },
  get chartKind(): ChartKind {
    return chartKind.value;
  },
  get dialog(): DialogKind {
    return dialogKind.value;
  },
  showDesktop(): void {
    viewKind.set("desktop");
  },
  showGrid(kind: GridKind): void {
    gridKind.set(kind);
    viewKind.set("grid");
  },
  showGantt(): void {
    viewKind.set("gantt");
  },
  showChart(kind: ChartKind): void {
    chartKind.set(kind);
    viewKind.set("chart");
  },
  openDialog(dialog: DialogKind): void {
    const current = dialogKind.peek();
    if (current) dialogStack.push(current);
    dialogKind.set(dialog);
  },
  closeDialog(): void {
    dialogKind.set(dialogStack.pop() ?? null);
  },
  closeAllDialogs(): void {
    dialogStack = [];
    dialogKind.set(null);
  },
};
