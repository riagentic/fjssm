// The two docked flat toolbars + the FJSS logo
// (MainFrm.cpp:118-164, FJSSM.rc:591-642)
import { commands, isEnabled, runCommand } from "./commands.ts";
import { clearStatus, setStatus } from "./status.ts";
import { t } from "./i18n.ts";

type Btn = { cmd: string; strip: 1 | 2; index: number } | "sep";

const TOOLBAR1: Btn[] = [
  { cmd: "fileNew", strip: 1, index: 0 },
  { cmd: "fileOpen", strip: 1, index: 1 },
  { cmd: "fileSave", strip: 1, index: 2 },
  { cmd: "fileImport", strip: 1, index: 3 },
  { cmd: "fileExport", strip: 1, index: 4 },
  "sep",
  { cmd: "editProject", strip: 1, index: 5 },
  { cmd: "editMachines", strip: 1, index: 6 },
  { cmd: "editJobs", strip: 1, index: 7 },
  { cmd: "editProcesses", strip: 1, index: 8 },
  "sep",
  { cmd: "editCut", strip: 1, index: 9 },
  { cmd: "editCopy", strip: 1, index: 10 },
  { cmd: "editPaste", strip: 1, index: 11 },
  "sep",
  { cmd: "viewProject", strip: 1, index: 12 },
  { cmd: "viewMachines", strip: 1, index: 13 },
  { cmd: "viewJobs", strip: 1, index: 14 },
  { cmd: "viewProcesses", strip: 1, index: 15 },
  "sep",
  { cmd: "convertCrispDD", strip: 1, index: 16 },
  { cmd: "convertCrispPT", strip: 1, index: 17 },
  { cmd: "convertFuzzyDD", strip: 1, index: 18 },
  { cmd: "convertFuzzyPT", strip: 1, index: 19 },
  "sep",
  { cmd: "convertDefaultDD", strip: 1, index: 20 },
  { cmd: "convertOptions", strip: 1, index: 21 },
];

const TOOLBAR2: Btn[] = [
  { cmd: "computeSA", strip: 2, index: 0 },
  { cmd: "computeGA", strip: 2, index: 1 },
  "sep",
  { cmd: "computeConvExp", strip: 2, index: 2 },
  { cmd: "computeCritExp", strip: 2, index: 3 },
  "sep",
  { cmd: "computeCriterion", strip: 2, index: 4 },
  "sep",
  { cmd: "resultSolve", strip: 2, index: 5 },
  { cmd: "resultGant", strip: 2, index: 6 },
  "sep",
  { cmd: "resultRun", strip: 2, index: 7 },
  { cmd: "resultConvexp", strip: 2, index: 8 },
  "sep",
  { cmd: "resultCritAre", strip: 2, index: 9 },
  { cmd: "resultCritSoare", strip: 2, index: 10 },
  "sep",
  { cmd: "helpAbout", strip: 2, index: 11 },
];

function Gripper() {
  return (
    <span
      style={{
        width: "3px",
        height: "20px",
        margin: "0 2px",
        borderLeft: "1px solid var(--btnhighlight)",
        borderRight: "1px solid var(--btnshadow)",
      }}
    />
  );
}

function Strip(props: { buttons: Btn[] }) {
  return (
    <div style={{ display: "flex", alignItems: "center" }}>
      <Gripper key="grip" />
      {props.buttons.map((b, i) => {
        if (b === "sep") return <span key={i} className="tb-sep" />;
        const enabled = isEnabled(b.cmd);
        const url = b.strip === 1
          ? "/assets/toolbar.png"
          : "/assets/toolbar2.png";
        return (
          <button
            key={b.cmd}
            type="button"
            className="tb-btn"
            style={{ minWidth: "23px", minHeight: "22px", boxShadow: "none" }}
            disabled={!enabled}
            title={t(commands[b.cmd]?.tip.trim() ?? "")}
            onMouseEnter={() => setStatus(commands[b.cmd]?.help ?? "")}
            onMouseLeave={clearStatus}
            onClick={() => runCommand(b.cmd)}
          >
            <span
              className="tb-ico"
              style={{
                display: "inline-block",
                backgroundImage: `url(${url})`,
                backgroundPosition: `${-16 * b.index}px 0`,
                width: "16px",
                height: "16px",
                filter: enabled ? undefined : "grayscale(1) opacity(0.4)",
              }}
            />
          </button>
        );
      })}
    </div>
  );
}

export function Toolbars() {
  return (
    <div className="toolbar" style={{ flexWrap: "wrap" }}>
      <Strip buttons={TOOLBAR1} />
      <Strip buttons={TOOLBAR2} />
      <span style={{ flex: 1 }} />
      <img
        src="/assets/logo_toolbar.png"
        alt="FJSS Maker"
        style={{ display: "block", imageRendering: "pixelated" }}
      />
    </div>
  );
}
