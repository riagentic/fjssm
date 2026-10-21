// Status bar — message pane + CAP/NUM/SCRL indicators (MainFrm.cpp:56-62)
import { signal } from "aio/air";
import { statusText } from "./status.ts";

const capLock = signal(false);
const numLock = signal(true);

if (typeof document !== "undefined") {
  document.addEventListener("keydown", (e) => {
    if (e.getModifierState) {
      capLock.set(e.getModifierState("CapsLock"));
      numLock.set(e.getModifierState("NumLock"));
    }
  });
}

function Indicator(props: { label: string; on: boolean }) {
  return (
    <div
      className="sb-pane sunken-thin"
      style={{
        width: "40px",
        textAlign: "center",
        color: props.on ? "inherit" : "var(--graytext)",
      }}
    >
      {props.label}
    </div>
  );
}

export function StatusBar() {
  return (
    <div className="statusbar">
      <div className="sb-pane main sunken-thin">{statusText.value}</div>
      <Indicator label="CAP" on={capLock.value} />
      <Indicator label="NUM" on={numLock.value} />
      <Indicator label="SCRL" on={false} />
    </div>
  );
}
