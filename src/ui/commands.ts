// Command registry — every menu/toolbar command with its handler, help
// strings (from the FJSSM.rc string table) and enable rule.
import { doc } from "../cell/doc.ts";
import { view } from "./view-store.ts";
import { parseJss, writeJss } from "../lib/jss.ts";
import { exportCsv, importCsv } from "../lib/csv.ts";
import { strType } from "../lib/format.ts";
import { isAbleToSolve } from "../lib/project.ts";
import { Project } from "../type/model.ts";
import { downloadFile, pickFile } from "./fileio.ts";
import { runCopy } from "./copy.ts";

const title = (): string => doc.project.name || "FJSS Maker";

function confirmDiscard(): boolean {
  if (!doc.modified) return true;
  return confirm(
    `Projekt ${title()} byl změněn.\nOpravdu chcete zahodit neuložené změny?`,
  );
}

/** AskForConvert (FJSSMView.cpp:384-406) */
function askForConvert(nType: 0 | 1 | 2 | 3): boolean {
  const variants = [
    "DETERMINISTICKÝMI TERMÍNY DOKONČENÍ ?",
    "DETERMINISTICKÝMI DOBAMI TRVÁNÍ OPERACÍ ?",
    "FUZZY TERMÍNY DOKONČENÍ ?",
    "FUZZY DOBAMI TRVÁNÍ OPERACÍ ?",
  ];
  return confirm(
    `Opravdu chcete provést konverzi projektu na typ \n\ns  ${variants[nType]}`,
  );
}

/** ShowConvertReport (FJSSMView.cpp:408-424) */
function showConvertReport(): void {
  alert(
    `Hotovo !!! \n\nAktuální typ projektu:   ${
      strType(doc.project as Project)
    }\t`,
  );
}

async function convert(
  nType: 0 | 1 | 2 | 3,
  method: () => Promise<unknown>,
): Promise<void> {
  if (!askForConvert(nType)) return;
  await method();
  showConvertReport();
}

export interface Command {
  run(): void | Promise<void>;
  help: string; // status-bar long help
  tip: string; // tooltip short text
  enabled?(): boolean;
}

const able = (): boolean => isAbleToSolve(doc.project as Project);
const ableResult = (): boolean => doc.results.ableResult > 0;
const idle = (): boolean => doc.solving.action === -1;

export const commands: Record<string, Command> = {
  fileNew: {
    help: "Vytvoří nový projekt",
    tip: "Nový projekt",
    async run() {
      if (!confirmDiscard()) return;
      await doc.newProject();
      view.showDesktop();
    },
  },
  fileOpen: {
    help: "Otevře existující projekt",
    tip: "Otevřít",
    async run() {
      if (!confirmDiscard()) return;
      const file = await pickFile(".jss");
      if (!file) return;
      try {
        const project = parseJss(file.bytes);
        await doc.load(project, file.name);
        view.showDesktop();
      } catch {
        alert("Chyba při otevírání souboru, patrně nekompaktibilní formát !!!");
      }
    },
  },
  fileImport: {
    help: "Importuje projekt ze struktury .csv",
    tip: "Importovat",
    async run() {
      if (!confirmDiscard()) return;
      const file = await pickFile(".csv");
      if (!file) return;
      try {
        const project = importCsv(file.bytes);
        await doc.load(project, "");
        view.showDesktop();
      } catch {
        alert(
          "Chyba při importu dat, patrně nekompaktibilní formát souboru !!!",
        );
      }
    },
  },
  fileSave: {
    help: "Uloží aktuální projekt",
    tip: " Uložit",
    async run() {
      const name = doc.fileName || `${doc.project.name || "Projekt"}.jss`;
      downloadFile(writeJss(doc.project as Project), name);
      await doc.markSaved(name);
    },
  },
  fileSaveAs: {
    help: "Uloží aktuální projekt s novým jménem souboru",
    tip: " Uložit jako...",
    async run() {
      const suggested = doc.fileName || `${doc.project.name || "Projekt"}.jss`;
      const name = prompt("Uložit jako:", suggested);
      if (!name) return;
      downloadFile(
        writeJss(doc.project as Project),
        name.endsWith(".jss") ? name : `${name}.jss`,
      );
      await doc.markSaved(name.endsWith(".jss") ? name : `${name}.jss`);
    },
  },
  fileExport: {
    help: "Exportuje projekt do struktury .csv",
    tip: "Exportovat",
    run() {
      downloadFile(
        exportCsv(doc.project as Project),
        `${doc.project.name || "Projekt"}.csv`,
      );
    },
  },
  appExit: {
    help: "Ukončí aplikaci s dotazem na uložení neuloženého projektu",
    tip: "Ukončit",
    run() {
      if (!confirmDiscard()) return;
      globalThis.close();
    },
  },

  editProject: {
    help: "Editace zakladních údajů o projektu",
    tip: "Editace projektu",
    run: () => view.openDialog("project"),
  },
  editMachines: {
    help: "Editace týkající se strojů",
    tip: "Editace strojů",
    run: () => view.openDialog("machines"),
  },
  editJobs: {
    help: "Editace týkající se jobů",
    tip: "Editace jobů",
    run: () => view.openDialog("jobs"),
  },
  editProcesses: {
    help: "Editace týkající se operací",
    tip: "Editace operací",
    enabled: () => doc.project.nJob > 0 && doc.project.nMachine > 0,
    run: () => view.openDialog("processes"),
  },
  editCut: {
    help: "Odstranění a umístění do schránky",
    tip: " Odstranit",
    enabled: () => false,
    run() {},
  },
  editCopy: {
    help: "Uložení do schránky",
    tip: " Uložení do schránky",
    enabled: () => view.view !== "desktop",
    run: () => runCopy(),
  },
  editPaste: {
    help: "Vloží obsah schránky",
    tip: "Vložit",
    enabled: () => false,
    run() {},
  },
  editSelectAll: {
    help: "Vybere všechno",
    tip: "Vybrat vše",
    enabled: () => view.view === "grid",
    run() {
      globalThis.dispatchEvent(new CustomEvent("fjssm-select-all"));
    },
  },

  viewProject: {
    help: "Rychlé prohlížení parametrů projektu",
    tip: "Prohlížení projektu",
    run: () => view.showGrid("project"),
  },
  viewJobs: {
    help: "Rychlé prohlížení jobů",
    tip: "Prohlížení jobů",
    run: () => view.showGrid("jobs"),
  },
  viewMachines: {
    help: "Rychlé prohlížení strojů",
    tip: "Prohlížení strojů",
    run: () => view.showGrid("machines"),
  },
  viewProcesses: {
    help: "Rychlé prohlížení operací",
    tip: "Prohlížení operací",
    run: () => view.showGrid("processes"),
  },

  convertCrispDD: {
    help: "Konverze na problém s deterministickými termíny dokončení",
    tip: "Deterministické termíny dokončení",
    enabled: () => !(doc.project.type === 0 || doc.project.type === 2),
    run: () => convert(0, () => doc.convertCrispDD()),
  },
  convertCrispPT: {
    help: "Konverze na problém s deterministickými časy operací",
    tip: "Deterministické časy operací",
    enabled: () => !(doc.project.type === 0 || doc.project.type === 1),
    run: () => convert(1, () => doc.convertCrispPT()),
  },
  convertFuzzyDD: {
    help: "Konverze na problém s fuzzy termíny dokončení",
    tip: "Fuzzy termíny dokončení",
    enabled: () => !(doc.project.type === 1 || doc.project.type === 3),
    run: () => convert(2, () => doc.convertFuzzyDD()),
  },
  convertFuzzyPT: {
    help: "Konverze na problém s fuzzy časy operací",
    tip: "Fuzzy časy operací",
    enabled: () => !(doc.project.type === 2 || doc.project.type === 3),
    run: () => convert(3, () => doc.convertFuzzyPT()),
  },
  convertDefaultDD: {
    help: "Automatické nastavení termínů dokončení",
    tip: "Automatické termíny dokončení",
    enabled: able,
    async run() {
      if (
        !confirm(
          "Opravdu chcete provést AUTOMATICKÉ GENEROVÁNÍ VŠECH TERMÍNŮ DOKONČENÍ ?",
        )
      ) return;
      await doc.convertDefaultDD();
      alert("TERMÍNY DOKONOČENÍ byly úspěšně vygenerovány.");
    },
  },
  convertOptions: {
    help: "Nastavení koeficientů konverze",
    tip: "Nastavení konverze",
    run: () => view.openDialog("convert"),
  },

  computeSA: {
    help: "Hledání optimálního rozvrhu metodou simulovaného žíhání",
    tip: "Simulované žíhání",
    enabled: able,
    run: () => view.openDialog("sa"),
  },
  computeGA: {
    help: "Hledání optimálního rozvrhu metodou genetického algoritmu",
    tip: "Genetický algoritmus",
    enabled: able,
    run: () => view.openDialog("ga"),
  },
  computeConvExp: {
    help: "Multivýpočet pro zjišťování konvergence jednotlivých metod",
    tip: "Výpočet rychlosti konvergence",
    enabled: able,
    run: () => view.openDialog("convexp"),
  },
  computeCritExp: {
    help: "Multivýpočet pro porovnávání kvality jednotlivých kritérií",
    tip: "Výpočet kvality kritérií",
    enabled: able,
    run: () => view.openDialog("critexp"),
  },
  computeCriterion: {
    help: "Výběr kriteria účelové funkce",
    tip: "Výběr kritéria",
    enabled: able,
    run: () => view.openDialog("criteria"),
  },

  resultSolve: {
    help: "Report rozvrhu vybrané metody",
    tip: "Report rozvrhu",
    enabled: ableResult,
    run: () => view.showGrid("schedule"),
  },
  resultGant: {
    help: "Gantův graf rozvrhu vybrané metody",
    tip: "Gantův graf",
    enabled: ableResult,
    run: () => view.showGantt(),
  },
  resultRun: {
    help: "Graf průběhu výpočtu",
    tip: "Průběh výpočtu",
    enabled: ableResult,
    run: () => view.showChart("history"),
  },
  resultConvexp: {
    help: "Výsledky multivýpočtu konvergence",
    tip: "Výsledky konvergence",
    enabled: () => doc.results.ableConvResult,
    run: () => view.showGrid("convergence"),
  },
  resultCritIteration: {
    help: "Zobrazí iterace získáne multivýpočtem pro srovnání kritérií",
    tip: "Iterace výpočtu",
    enabled: () => doc.results.ableCritResult,
    run: () => view.showGrid("iteration"),
  },
  resultCritRe: {
    help: "Zobrazí relativní chyby iterací",
    tip: "Relativní chyby",
    enabled: () => doc.results.ableCritResult,
    run: () => view.showGrid("re"),
  },
  resultCritAre: {
    help: "Zobrazí průměrné hodnoty relativních chyb",
    tip: "Průměry relativních cyb",
    enabled: () => doc.results.ableCritResult,
    run: () => view.showGrid("are"),
  },
  resultCritSoare: {
    help: "Zobrazí graf středních hodnot průměrů relativních chyb do grafu",
    tip: "Střední hodnoty průměrů relativních chyb",
    enabled: () => doc.results.ableCritResult,
    run: () => view.showChart("soare"),
  },
  resultSA: {
    help: "Budou zobrazeny výsledky pro simulované žíhání",
    tip: "Výsledky pro simulované žíhání",
    enabled: () => (doc.results.ableResult & 1) > 0,
    run: () => void doc.selectResult(0),
  },
  resultGA: {
    help: "Budou zobrazeny výsledky pro genetický algoritmus",
    tip: "Výsledky pro genetický algoritmus",
    enabled: () => (doc.results.ableResult & 2) > 0,
    run: () => void doc.selectResult(1),
  },

  helpHelp: {
    help: "Tématická nápověda",
    tip: "Tématická nápověda",
    run() {
      // the original HTML Help content (hlp/*.htm), converted to UTF-8
      globalThis.open("/assets/help/main_index.htm", "_blank");
    },
  },
  helpAbout: {
    help: "Zobrazí informace o programu",
    tip: "O programu",
    run: () => view.openDialog("about"),
  },
};

/** true unless the command declares an enable rule that fails, or a solve
 *  is running (the original blocks the whole UI in modal dialogs) */
export function isEnabled(id: string): boolean {
  const cmd = commands[id];
  if (!cmd) return false;
  if (!idle() && !id.startsWith("result") && id !== "helpAbout") return false;
  return cmd.enabled ? cmd.enabled() : true;
}

export function runCommand(id: string): void {
  if (!isEnabled(id)) return;
  void commands[id]!.run();
}
