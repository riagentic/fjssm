// Classic segmented Win32 progress bar (msctls_progress32)
import { dlu } from "./Dialog.tsx";

export function Progress(
  props: {
    x: number;
    y: number;
    w: number;
    h: number;
    pos: number;
    range: number;
  },
) {
  const frac = props.range > 0 ? Math.min(1, props.pos / props.range) : 0;
  const widthPx = Math.round(props.w * 1.5);
  const segW = 8;
  const segs = Math.floor((widthPx - 4) / (segW + 2));
  const lit = Math.round(segs * frac);
  return (
    <div
      className="sunken-thin"
      style={{
        ...dlu(props.x, props.y, props.w, props.h),
        display: "flex",
        gap: "2px",
        padding: "2px",
        background: "var(--btnface)",
      }}
    >
      {Array.from({ length: segs }, (_, i) => (
        <span
          key={i}
          style={{
            width: `${segW}px`,
            height: "100%",
            background: i < lit ? "rgb(0,0,128)" : "transparent",
          }}
        />
      ))}
    </div>
  );
}
