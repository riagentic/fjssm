// FJSS Maker — main frame: menu, two toolbars, client area, status bar
// (CMainFrame + CFJSSMView shell)
import { doc } from "./cell/doc.ts";
import { view } from "./ui/view-store.ts";
import { MenuBar } from "./ui/MenuBar.tsx";
import { Toolbars } from "./ui/Toolbars.tsx";
import { StatusBar } from "./ui/StatusBar.tsx";
import { GridView } from "./ui/GridView.tsx";
import { GanttView } from "./ui/GanttView.tsx";
import { ChartView } from "./ui/ChartView.tsx";
import { runCommand } from "./ui/commands.ts";
import { AboutDlg } from "./ui/dlg/AboutDlg.tsx";
import { ProjectDlg } from "./ui/dlg/ProjectDlg.tsx";
import { MachineDlg } from "./ui/dlg/MachineDlg.tsx";
import { JobDlg } from "./ui/dlg/JobDlg.tsx";
import { ProcessDlg } from "./ui/dlg/ProcessDlg.tsx";
import { SADlg } from "./ui/dlg/SADlg.tsx";
import { GADlg } from "./ui/dlg/GADlg.tsx";
import { CriteriaDlg } from "./ui/dlg/CriteriaDlg.tsx";
import { ConvertDlg } from "./ui/dlg/ConvertDlg.tsx";
import { ConvExpDlg } from "./ui/dlg/ConvExpDlg.tsx";
import { CritExpDlg } from "./ui/dlg/CritExpDlg.tsx";

// accelerators (FJSSM.rc:968-992)
const ACCEL: Record<string, string> = {
  n: "fileNew",
  o: "fileOpen",
  i: "fileImport",
  s: "fileSave",
  e: "fileExport",
  c: "editCopy",
  a: "editSelectAll",
  h: "computeSA",
  g: "computeGA",
  r: "resultSolve",
};

// deno-lint-ignore no-explicit-any
(globalThis as any).__fjssm = { doc, view, runCommand };

if (typeof document !== "undefined") {
  document.addEventListener("keydown", (e) => {
    // modal dialogs swallow accelerators, like DoModal in the original
    if (view.dialog !== null) {
      if (e.key === "Escape" && doc.solving.action === -1) {
        e.preventDefault();
        view.closeDialog();
      }
      return;
    }
    if (e.ctrlKey && !e.altKey && ACCEL[e.key.toLowerCase()]) {
      e.preventDefault();
      runCommand(ACCEL[e.key.toLowerCase()]!);
    }
    if (e.key === "F1") {
      e.preventDefault();
      runCommand("helpHelp");
    }
  });
}

function ClientArea() {
  switch (view.view) {
    case "grid":
      return <GridView kind={view.gridKind} />;
    case "gantt":
      return <GanttView />;
    case "chart":
      return <ChartView kind={view.chartKind} />;
    default:
      // desktop logo, centered on blank background (FJSSMView.cpp:105-140)
      return (
        <div
          style={{
            width: "100%",
            height: "100%",
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            background: "var(--appworkspace)",
          }}
        >
          <img
            src="/assets/logo_desktop.png"
            alt="FJSS Maker"
            style={{ maxWidth: "95%" }}
          />
        </div>
      );
  }
}

function Dialogs() {
  switch (view.dialog) {
    case "about":
      return <AboutDlg />;
    case "project":
      return <ProjectDlg />;
    case "machines":
      return <MachineDlg />;
    case "jobs":
      return <JobDlg />;
    case "processes":
      return <ProcessDlg />;
    case "sa":
      return <SADlg />;
    case "sa-params":
      return <SADlg paramsOnly />;
    case "ga":
      return <GADlg />;
    case "ga-params":
      return <GADlg paramsOnly />;
    case "criteria":
      return <CriteriaDlg />;
    case "convert":
      return <ConvertDlg />;
    case "convexp":
      return <ConvExpDlg />;
    case "critexp":
      return <CritExpDlg />;
    default:
      return null;
  }
}

export default function App() {
  // SDI window title: "<project> - FJSS Maker"
  if (typeof document !== "undefined") {
    const name = doc.project.name;
    document.title = name ? `${name} - FJSS Maker` : "FJSS Maker";
  }

  return (
    <div className="app-frame">
      <MenuBar />
      <Toolbars />
      <div className="client-area">
        <ClientArea />
      </div>
      <StatusBar />
      <Dialogs />
    </div>
  );
}
