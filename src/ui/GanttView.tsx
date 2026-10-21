// Gantt chart — exact SVG port of CGant (gant.cpp). White canvas, job rows of
// 7 units: upper half = due-date trapezoid (job color), lower half = operation
// "fuzzy flags" from r→C (machine color). Keys: x/X y/Y zoom, r reset.
import { onMount, useDimensions, useLocal, useRef } from "aio/air";
import { doc } from "../cell/doc.ts";
import { Project, Schedule } from "../type/model.ts";
import { isFuzzyDD, isFuzzyPT } from "../type/model.ts";
import { solveScheduleForResult } from "../lib/reports.ts";
import { strMethod, strType } from "../lib/format.ts";
import { PALETTE } from "../lib/palette.ts";
import { registerCopy } from "./copy.ts";
import { FN_PART } from "../type/fn.ts";

const XMARGIN = 10, YMARGIN = 10, LLEGEND = 50, TLEGEND = 60, YVOID = 10;

export function GanttView() {
  const p = doc.project as Project;
  const r = doc.results;
  const sched = (r.selectedResult === 0 ? r.saSchedule : r.gaSchedule) as
    | Schedule
    | null;
  const { local: scale, set: setScale } = useLocal<
    { x: number; y: number } | null
  >(null);
  const host = useDimensions();
  const svgRef = useRef<SVGSVGElement>(null!);

  onMount(() => {
    // defer past the useDimensions-driven re-render, which would drop focus
    setTimeout(() => (host.ref.current as HTMLElement | null)?.focus(), 30);
  });

  if (!sched) return <div style={{ background: "#fff", height: "100%" }} />;

  const fdd = isFuzzyDD(p.type);
  const fpt = isFuzzyPT(p.type);
  const { engine, ptr } = solveScheduleForResult(p, doc.crit, sched);

  // GetMaxX / GetMaxY (gant.cpp:328-354)
  let maxPTOrDD = fpt ? ptr.makespan[3] : ptr.makespan[0];
  for (const job of p.jobs) {
    maxPTOrDD = Math.max(maxPTOrDD, fdd ? job.dd[3] : job.dd[0]);
  }
  const maxX = 2 * XMARGIN + maxPTOrDD + LLEGEND;
  const maxY = 7 * p.nJob * YVOID + 2 * YMARGIN + TLEGEND;

  // SetDefaultScale (gant.cpp:97-112) — fit to host once mounted
  const hw = host.width.value;
  const hh = host.height.value;
  const fx = scale?.x ?? (hw > 0 ? (hw / maxX) * 0.9 : 0.7);
  const fy = scale?.y ?? (hh > 0 ? (hh / maxY) * 0.8 : 0.7);

  const PTX = (x: number): number => (XMARGIN + x) * fx + LLEGEND;
  const PTY = (job: number, part: number): number =>
    (YMARGIN + 7 * job * YVOID + (7 - part) * YVOID) * fy + TLEGEND;
  const DDY = (job: number, part: number): number =>
    (YMARGIN + 7 * job * YVOID + (3 - part) * YVOID) * fy + TLEGEND;

  const width = (maxX - LLEGEND) * fx + LLEGEND;
  const height = (maxY - TLEGEND) * fy + TLEGEND;
  const gridRight = PTX(maxX - XMARGIN - LLEGEND);

  const shapes: unknown[] = [];

  // DrawGrid (gant.cpp:482-518)
  for (let i = 0; i < p.nJob; i++) {
    for (const part of [0, 3]) {
      shapes.push(
        <line
          key={`grid-solid-${i}-${part}`}
          x1={PTX(0)}
          y1={PTY(i, part)}
          x2={gridRight}
          y2={PTY(i, part)}
          stroke="rgb(0,0,0)"
        />,
      );
    }
    for (const part of [1, 2]) {
      shapes.push(
        <line
          key={`grid-faint-${i}-${part}`}
          x1={PTX(0)}
          y1={PTY(i, part)}
          x2={gridRight}
          y2={PTY(i, part)}
          stroke="rgb(200,200,200)"
        />,
      );
    }
    for (const part of [0, 1]) {
      shapes.push(
        <line
          key={`grid-dd-${i}-${part}`}
          x1={PTX(0)}
          y1={DDY(i, part)}
          x2={gridRight}
          y2={DDY(i, part)}
          stroke="rgb(200,200,200)"
        />,
      );
    }
  }

  // DrawPT (gant.cpp:384-454)
  for (let i = 0; i < p.nJob; i++) {
    const job = p.jobs[i]!;
    const dd: [number, number][] = [];
    for (let k = 0; k < 2; k++) {
      if (fdd) {
        dd[k] = [PTX(job.dd[k]!), DDY(i, k)];
        dd[k + 2] = [PTX(job.dd[k + 2]!), DDY(i, 1 - k)];
      } else {
        dd[k] = [PTX(0), DDY(i, k)];
        dd[k + 2] = [PTX(job.dd[0]), DDY(i, 1 - k)];
      }
    }
    shapes.push(
      <polygon
        key={`dd-${i}`}
        points={dd.map(([x, y]) => `${x},${y}`).join(" ")}
        fill={PALETTE[job.color]}
        stroke="rgb(0,0,0)"
        strokeWidth={1}
      />,
    );

    for (const node of engine.jobNodes[i]!) {
      const pt: [number, number][] = [];
      for (let k = 0; k < FN_PART; k++) {
        if (fpt) {
          pt[k] = [PTX(node.r[k]!), PTY(i, k)];
          pt[k + FN_PART] = [
            PTX(node.C[FN_PART - k - 1]!),
            PTY(i, FN_PART - k - 1),
          ];
        } else {
          pt[k] = [PTX(node.r[0]), PTY(i, k)];
          pt[k + FN_PART] = [PTX(node.C[0]), PTY(i, FN_PART - k - 1)];
        }
      }
      shapes.push(
        <polygon
          key={`pt-${i}-${node.process}`}
          points={pt.map(([x, y]) => `${x},${y}`).join(" ")}
          fill={PALETTE[p.machines[node.machine]!.color]}
          stroke="rgb(0,0,0)"
          strokeWidth={1}
        />,
      );
    }
  }

  // Edit → Kopírovat puts a BITMAP on the clipboard, like CGant::CopyToClipboard
  registerCopy(() => {
    const svg = svgRef.current;
    if (!svg) return;
    const xml = new XMLSerializer().serializeToString(svg);
    const img = new Image();
    img.onload = () => {
      const canvas = document.createElement("canvas");
      canvas.width = svg.width.baseVal.value;
      canvas.height = svg.height.baseVal.value;
      const ctx = canvas.getContext("2d")!;
      ctx.fillStyle = "#fff";
      ctx.fillRect(0, 0, canvas.width, canvas.height);
      ctx.drawImage(img, 0, 0);
      canvas.toBlob((blob) => {
        if (blob && "ClipboardItem" in globalThis) {
          void navigator.clipboard?.write([
            new ClipboardItem({ "image/png": blob }),
          ]).catch(() => navigator.clipboard?.writeText(xml));
        } else {
          void navigator.clipboard?.writeText(xml);
        }
      });
    };
    img.src = "data:image/svg+xml;charset=utf-8," + encodeURIComponent(xml);
  });

  return (
    <div
      ref={host.ref}
      tabIndex={0}
      style={{
        background: "#fff",
        width: "100%",
        height: "100%",
        overflow: "auto",
        outline: "none",
      }}
      onKeyDown={(e) => {
        const k = (e as KeyboardEvent).key;
        let { x, y } = { x: fx, y: fy };
        if (k === "x") x -= 0.1;
        if (k === "X") x += 0.1;
        if (k === "y") y -= 0.1;
        if (k === "Y") y += 0.1;
        if (k === "r") {
          setScale(null);
          return;
        }
        if (x < 0.1) x = 0.1;
        if (y < 0.1) y = 0.1;
        if (x !== fx || y !== fy) setScale({ x, y });
      }}
    >
      <svg
        ref={svgRef as unknown as { current: HTMLElement | null }}
        width={Math.max(width, 100)}
        height={Math.max(height, 100)}
        style={{ display: "block", background: "#fff" }}
        font-family="Tahoma, MS Sans Serif, sans-serif"
        font-size="11"
      >
        {/* DrawJobLegend (gant.cpp:530-535) */}
        <text key="lg-name" x={XMARGIN * fx + LLEGEND} y={YMARGIN * fy + 11}>
          {p.name}
        </text>
        <text
          key="lg-type"
          x={XMARGIN * fx + LLEGEND}
          y={YMARGIN * fy + 20 + 11}
        >
          {strType(p)}
        </text>
        <text
          key="lg-method"
          x={XMARGIN * fx + LLEGEND}
          y={YMARGIN * fy + 40 + 11}
        >
          {strMethod(r.selectedResult)}
        </text>
        {/* DrawMainLegend (gant.cpp:520-528) */}
        {p.jobs.map((_, i) => (
          <text key={i} x={XMARGIN * fx} y={PTY(i, 4) + 11}>Job {i + 1}</text>
        ))}
        {shapes}
      </svg>
    </div>
  );
}
