// O programu — IDD_ABOUTBOX 214×174 (FJSSM.rc) with scrolling CCreditStatic
import { Btn, Dialog, dlu, Static } from "./Dialog.tsx";
import { view } from "../view-store.ts";

const CREDITS: ({ text: string; blue?: boolean } | { img: string })[] = [
  { text: "Program pro fuzzy rozvrhování" },
  { text: "zakázkové výroby" },
  { img: "/assets/logo_about_credits.png" },
  { text: "Na vývoji se podíleli:", blue: true },
  { text: "" },
  { text: "Programátor" },
  { text: "riagentic" },
  { text: "" },
  { img: "/assets/vutlogo_about_credits.png" },
  { text: "Testování" },
  { text: "riagentic" },
  { text: "" },
  { img: "/assets/bird_about_credits.png" },
  { text: "All right reserved" },
  { text: "Copyright (c) 2002" },
];

export function AboutDlg() {
  return (
    <Dialog title="O programu" w={214} h={174}>
      <img
        src="/assets/logo_about2.png"
        alt=""
        style={{ ...dlu(12, 4), imageRendering: "pixelated" }}
      />
      <Static text="Program FJSS Maker" x={54} y={12} w={66} h={8} />
      <Static text="Verze 1.0" x={54} y={24} w={32} h={8} />
      <div
        className="sunken-thin"
        style={{
          ...dlu(6, 48, 201, 78),
          overflow: "hidden",
          background: "var(--btnface)",
          textAlign: "center",
        }}
      >
        <div style={{ animation: "credits-scroll 30s linear infinite" }}>
          <style key="credits-keyframes">
            {`@keyframes credits-scroll {
              from { transform: translateY(130px); }
              to { transform: translateY(-100%); }
            }`}
          </style>
          {CREDITS.map((c, i) =>
            "img" in c
              ? (
                <div key={i}>
                  <img src={c.img} alt="" style={{ maxWidth: "90%" }} />
                </div>
              )
              : (
                <div
                  key={i}
                  style={{
                    color: c.blue ? "rgb(0,0,255)" : "#000",
                    fontWeight: c.blue ? "bold" : "normal",
                    lineHeight: "16px",
                  }}
                >
                  {c.text || " "}
                </div>
              )
          )}
        </div>
      </div>
      <Static
        text="Copyright (C) 2002"
        x={42}
        y={134}
        w={126}
        h={13}
      />
      <Btn text="OK" x={78} y={150} def onClick={() => view.closeDialog()} />
    </Dialog>
  );
}
