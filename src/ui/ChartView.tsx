// MSChart replacement — 2D XY convergence plot (OnResultHistory,
// FJSSMView.cpp:741-822) and 2D bar SOARE chart (OnResultCritSoare, :914-952).
// Same titles, labels, X-scaling quirk and legend as the OCX version.
import { doc } from "../cell/doc.ts";
import { ChartKind } from "../type/view.ts";
import { strCriterion, strNRDouble } from "../lib/format.ts";
import { ableCriteria } from "../lib/criteria.ts";
import { isFuzzyDD, isFuzzyPT, Project } from "../type/model.ts";

// MSChart 2.0 default series colors
const SERIES_COLORS = [
  "rgb(0,0,128)",
  "rgb(128,0,0)",
  "rgb(0,128,0)",
  "rgb(128,128,0)",
  "rgb(128,0,128)",
  "rgb(0,128,128)",
  "rgb(255,0,0)",
  "rgb(0,0,255)",
];

const W = 640, H = 420, ML = 70, MR = 150, MT = 50, MB = 50;

function Frame(
  props: { title: string; children?: unknown; legend: string[] },
) {
  return (
    <svg
      width={W}
      height={H}
      style={{ background: "#fff", display: "block" }}
      font-family="Arial, sans-serif"
      font-size="11"
    >
      <text key="title" x={W / 2} y={20} text-anchor="middle" font-size="14">
        {props.title}
      </text>
      <rect
        key="plot-box"
        x={ML}
        y={MT}
        width={W - ML - MR}
        height={H - MT - MB}
        fill="none"
        stroke="#000"
      />
      {props.children}
      {/* legend box */}
      <rect
        key="legend-box"
        x={W - MR + 10}
        y={MT}
        width={MR - 20}
        height={props.legend.length * 16 + 8}
        fill="#fff"
        stroke="#000"
      />
      {props.legend.map((name, i) => (
        <g key={i}>
          <rect
            x={W - MR + 14}
            y={MT + 6 + i * 16}
            width={10}
            height={8}
            fill={SERIES_COLORS[i % SERIES_COLORS.length]}
          />
          <text x={W - MR + 28} y={MT + 14 + i * 16} font-size="10">
            {name}
          </text>
        </g>
      ))}
    </svg>
  );
}

function HistoryChart() {
  const axis = doc.results.axis;
  if (!axis.length) {
    return <Frame title="Prubeh hledani optima ucelove funkce" legend={[]} />;
  }

  // X-scale quirk (FJSSMView.cpp:765-777): fXScale = (|maxY|+|minY|) / maxX
  let fMaxY = 0, fMinY = 0, fMaxX = 0;
  for (const s of axis) {
    const last = s.points.length - 1;
    if (last < 0) continue;
    fMaxY = Math.max(fMaxY, s.points[0]!.y);
    fMinY = Math.min(fMinY, s.points[last]!.y);
    fMaxX = Math.max(fMaxX, s.points[last]!.x);
  }
  const fXScale = fMaxX ? (Math.abs(fMaxY) + Math.abs(fMinY)) / fMaxX : 1;

  // data ranges over scaled X and raw Y
  let xMax = -Infinity, yMin = Infinity, yMax = -Infinity;
  for (const s of axis) {
    for (const pt of s.points) {
      xMax = Math.max(xMax, pt.x * fXScale);
      yMin = Math.min(yMin, pt.y);
      yMax = Math.max(yMax, pt.y);
    }
  }
  if (!Number.isFinite(xMax)) xMax = 1;
  if (yMin === yMax) {
    yMin -= 1;
    yMax += 1;
  }

  const px = (x: number): number => ML + (x / (xMax || 1)) * (W - ML - MR);
  const py = (y: number): number =>
    H - MB - ((y - yMin) / (yMax - yMin)) * (H - MT - MB);

  return (
    <Frame
      title="Prubeh hledani optima ucelove funkce"
      legend={axis.map((s) => s.name)}
    >
      {/* Y gridlines + labels */}
      {Array.from({ length: 5 }, (_, i) => {
        const y = yMin + ((yMax - yMin) * i) / 4;
        return (
          <g key={i}>
            <line
              x1={ML}
              y1={py(y)}
              x2={W - MR}
              y2={py(y)}
              stroke="#d0d0d0"
            />
            <text x={ML - 4} y={py(y) + 3} text-anchor="end" font-size="10">
              {strNRDouble(y)}
            </text>
          </g>
        );
      })}
      {axis.map((s, i) => (
        <polyline
          key={i}
          points={s.points.map((pt) => `${px(pt.x * fXScale)},${py(pt.y)}`)
            .join(" ")}
          fill="none"
          stroke={SERIES_COLORS[i % SERIES_COLORS.length]}
          strokeWidth={1.5}
        />
      ))}
      <text
        key="x-label"
        x={ML + (W - ML - MR) / 2}
        y={H - 10}
        text-anchor="middle"
      >
        cas vypoctu [s / {strNRDouble(fXScale)}]
      </text>
      <text
        key="y-label"
        x={16}
        y={MT + (H - MT - MB) / 2}
        text-anchor="middle"
        transform={`rotate(-90 16 ${MT + (H - MT - MB) / 2})`}
      >
        Hodnota ucelove funkce
      </text>
    </Frame>
  );
}

function SoareChart() {
  const p = doc.project as Project;
  const avg = doc.results.averageRE;
  const able = ableCriteria(isFuzzyPT(p.type), isFuzzyDD(p.type));
  const values = avg.map((row) => row[row.length - 1] ?? 0);
  const vMax = Math.max(1e-9, ...values);
  const n = values.length || 1;
  const plotW = W - ML - MR;
  const band = plotW / n;

  return (
    <Frame
      title="Srovnani kvality pouzitych kriterii"
      legend={able.map((c) => strCriterion(c, 3))}
    >
      {values.map((v, i) => {
        const h = (v / vMax) * (H - MT - MB - 10);
        return (
          <rect
            key={i}
            x={ML + i * band + band * 0.2}
            y={H - MB - h}
            width={band * 0.6}
            height={h}
            fill={SERIES_COLORS[i % SERIES_COLORS.length]}
            stroke="#000"
          />
        );
      })}
      <text key="x-label" x={ML + plotW / 2} y={H - 10} text-anchor="middle">
        Jednotliva kriteria
      </text>
    </Frame>
  );
}

export function ChartView(props: { kind: ChartKind }) {
  return (
    <div
      style={{
        background: "#fff",
        width: "100%",
        height: "100%",
        display: "flex",
        alignItems: "center",
        justifyContent: "center",
        overflow: "auto",
      }}
    >
      {props.kind === "history" ? <HistoryChart /> : <SoareChart />}
    </div>
  );
}
