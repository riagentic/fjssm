// Report grid — CGridCtrl replacement (DrawGrid, FJSSMView.cpp:679-702).
// Yellow legend tint RGB(255,255,180); selection + clipboard copy.
import { onCleanup, onMount, useLocal, useRef } from "aio/air";
import { doc } from "../cell/doc.ts";
import { GridKind } from "../type/view.ts";
import { Project, Schedule } from "../type/model.ts";
import {
  generateAREReport,
  generateConvergenceReport,
  generateIterationReport,
  generateJobReport,
  generateMachineReport,
  generateProcessReport,
  generateProjectReport,
  generateREReport,
  generateResultReport,
} from "../lib/reports.ts";
import { ableCriteria } from "../lib/criteria.ts";
import { isFuzzyDD, isFuzzyPT } from "../type/model.ts";
import { registerCopy } from "./copy.ts";

const TINT = "rgb(255,255,180)";

function computeRows(kind: GridKind): { rows: string[][]; fixRows: number } {
  const p = doc.project as Project;
  const r = doc.results;
  const able = ableCriteria(isFuzzyPT(p.type), isFuzzyDD(p.type));
  switch (kind) {
    case "project":
      return { rows: generateProjectReport(p), fixRows: 1 };
    case "jobs":
      return { rows: generateJobReport(p), fixRows: 1 };
    case "machines":
      return { rows: generateMachineReport(p), fixRows: 1 };
    case "processes":
      return { rows: generateProcessReport(p), fixRows: 1 };
    case "schedule": {
      const sched = (r.selectedResult === 0 ? r.saSchedule : r.gaSchedule) as
        | Schedule
        | null;
      if (!sched) return { rows: [], fixRows: 0 };
      return {
        rows: generateResultReport(p, doc.crit, sched, r.selectedResult),
        fixRows: 0,
      };
    }
    case "convergence":
      return {
        rows: generateConvergenceReport(r.convergence as number[][]),
        fixRows: 0,
      };
    case "iteration":
      return {
        rows: generateIterationReport(
          r.iteration as number[][],
          able,
          r.creIterationUsed,
        ),
        fixRows: 0,
      };
    case "re":
      return {
        rows: generateREReport(
          r.re as number[][],
          r.iteration as number[][],
          able,
          r.creIterationUsed,
        ),
        fixRows: 0,
      };
    case "are":
      return {
        rows: generateAREReport(r.averageRE as number[][], able),
        fixRows: 0,
      };
  }
}

/** which cells get the yellow legend tint, per report (FJSSMView.cpp) */
function isTinted(
  kind: GridKind,
  row: number,
  col: number,
  nRows: number,
  nCols: number,
): boolean {
  switch (kind) {
    case "schedule":
      return row < 6 || (row >= nRows - 2 && col < 2);
    case "convergence":
      return row < 2 || col === 0;
    case "iteration":
    case "re":
    case "are":
      return row < 1 || col === nCols - 1;
    default:
      return false;
  }
}

export function GridView(props: { kind: GridKind }) {
  const { rows, fixRows } = computeRows(props.kind);
  const nRows = rows.length;
  const nCols = rows.reduce((m, r) => Math.max(m, r.length), 0);
  const { local: sel, set: setSel } = useLocal<
    { r1: number; c1: number; r2: number; c2: number } | null
  >(null);

  const inSel = (r: number, c: number): boolean => {
    if (!sel) return false;
    return r >= Math.min(sel.r1, sel.r2) && r <= Math.max(sel.r1, sel.r2) &&
      c >= Math.min(sel.c1, sel.c2) && c <= Math.max(sel.c1, sel.c2);
  };

  const copySelection = () => {
    const s = sel ?? { r1: 0, c1: 0, r2: nRows - 1, c2: nCols - 1 };
    const out: string[] = [];
    for (let r = Math.min(s.r1, s.r2); r <= Math.max(s.r1, s.r2); r++) {
      const line: string[] = [];
      for (let c = Math.min(s.c1, s.c2); c <= Math.max(s.c1, s.c2); c++) {
        line.push(rows[r]?.[c] ?? "");
      }
      out.push(line.join("\t"));
    }
    void navigator.clipboard?.writeText(out.join("\r\n"));
  };

  // fresh-closure handlers behind stable refs — mount-once listeners can't
  // go stale when the report data changes
  const live = useRef({ copySelection, selectAll: () => {} });
  live.current.copySelection = copySelection;
  live.current.selectAll = () =>
    setSel({ r1: 0, c1: 0, r2: nRows - 1, c2: nCols - 1 });

  registerCopy(() => live.current.copySelection());
  onMount(() => {
    const onSelectAll = () => live.current.selectAll();
    globalThis.addEventListener("fjssm-select-all", onSelectAll);
    onCleanup(() => {
      globalThis.removeEventListener("fjssm-select-all", onSelectAll);
    });
  });

  return (
    <div style={{ background: "#fff", minHeight: "100%", minWidth: "100%" }}>
      <table className="grid">
        <tbody>
          {rows.map((row, r) => (
            <tr key={r}>
              {Array.from({ length: nCols }, (_, c) => {
                const text = row[c] ?? "";
                const fixed = r < fixRows;
                const tint = isTinted(props.kind, r, c, nRows, nCols);
                if (fixed) return <th key={c}>{text}</th>;
                return (
                  <td
                    key={c}
                    className={inSel(r, c) ? "sel" : undefined}
                    style={tint && !inSel(r, c)
                      ? { background: TINT }
                      : undefined}
                    onMouseDown={() => setSel({ r1: r, c1: c, r2: r, c2: c })}
                    onMouseOver={(e) => {
                      if ((e as MouseEvent).buttons === 1 && sel) {
                        setSel({ ...sel, r2: r, c2: c });
                      }
                    }}
                  >
                    {text || " "}
                  </td>
                );
              })}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
