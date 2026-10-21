// Menu structure — exact port of IDR_MAINFRAME MENU (FJSSM.rc:876-960).
// `&x` marks the mnemonic; icon = [sprite, index] into the toolbar strips.
export type MenuIcon = [1 | 2, number];

export interface MenuItem {
  label?: string; // with & mnemonic
  accel?: string;
  command?: string;
  icon?: MenuIcon;
  separator?: boolean;
  radio?: "sa" | "ga"; // Výsledky SA/GA radio marks
  grayed?: boolean;
}

export interface Menu {
  label: string;
  items: MenuItem[];
}

export const MENUS: Menu[] = [
  {
    label: "&Projekt",
    items: [
      { label: "&Nový", accel: "Ctrl+N", command: "fileNew", icon: [1, 0] },
      {
        label: "&Otevřít...",
        accel: "Ctrl+O",
        command: "fileOpen",
        icon: [1, 1],
      },
      {
        label: "&Importovat...",
        accel: "Ctrl+I",
        command: "fileImport",
        icon: [1, 3],
      },
      { label: "&Uložit", accel: "Ctrl+S", command: "fileSave", icon: [1, 2] },
      { label: "Uložit &jako...", command: "fileSaveAs" },
      {
        label: "&Exportovat",
        accel: "Ctrl+E",
        command: "fileExport",
        icon: [1, 4],
      },
      { separator: true },
      { label: "Recent File", grayed: true },
      { separator: true },
      { label: "U&končení", command: "appExit" },
    ],
  },
  {
    label: "&Úpravy",
    items: [
      { label: "&Projekt", command: "editProject", icon: [1, 5] },
      { separator: true },
      { label: "&Stroje", command: "editMachines", icon: [1, 6] },
      { label: "&Joby", command: "editJobs", icon: [1, 7] },
      { separator: true },
      { label: "&Operace", command: "editProcesses", icon: [1, 8] },
      { separator: true },
      { label: "Odstranit", accel: "Ctrl+X", command: "editCut", icon: [1, 9] },
      {
        label: "Kopírovat",
        accel: "Ctrl+C",
        command: "editCopy",
        icon: [1, 10],
      },
      { label: "Vložit", accel: "Ctrl+V", command: "editPaste", icon: [1, 11] },
      { label: "Vybrat vše", accel: "Ctrl+A", command: "editSelectAll" },
    ],
  },
  {
    label: "&Konverze",
    items: [
      { label: "&Crisp DD", command: "convertCrispDD", icon: [1, 16] },
      { label: "C&risp PT", command: "convertCrispPT", icon: [1, 17] },
      { separator: true },
      { label: "&Fuzzy DD", command: "convertFuzzyDD", icon: [1, 18] },
      { label: "F&uzzy PT", command: "convertFuzzyPT", icon: [1, 19] },
      { separator: true },
      { label: "&Default DD", command: "convertDefaultDD", icon: [1, 20] },
      { separator: true },
      { label: "&Nastavení ", command: "convertOptions", icon: [1, 21] },
    ],
  },
  {
    label: "P&rohlížení",
    items: [
      { label: "&Projekt", command: "viewProject", icon: [1, 12] },
      { separator: true },
      { label: "&Joby", command: "viewJobs", icon: [1, 14] },
      { label: "&Stroje", command: "viewMachines", icon: [1, 13] },
      { separator: true },
      { label: "&Operace", command: "viewProcesses", icon: [1, 15] },
    ],
  },
  {
    label: "&Výpočet",
    items: [
      {
        label: "&Simulované žíhání",
        accel: "Ctrl+H",
        command: "computeSA",
        icon: [2, 0],
      },
      {
        label: "&Genetický algoritmus",
        accel: "Ctrl+G",
        command: "computeGA",
        icon: [2, 1],
      },
      { separator: true },
      { label: "&Konvergence metod", command: "computeConvExp", icon: [2, 2] },
      { label: "&Srovnání kritérií", command: "computeCritExp", icon: [2, 3] },
      { separator: true },
      { label: "&Výběr kritéria", command: "computeCriterion", icon: [2, 4] },
    ],
  },
  {
    label: "V&ýsledky",
    items: [
      {
        label: "&Rozvrh",
        accel: "Ctrl+R",
        command: "resultSolve",
        icon: [2, 5],
      },
      { label: "&Ganttův graf", command: "resultGant", icon: [2, 6] },
      { label: "&Průběh výpočtu", command: "resultRun", icon: [2, 7] },
      { separator: true },
      { label: "&Konvergence", command: "resultConvexp", icon: [2, 8] },
      // Kritéria submenu flattened items are provided via `submenu` below
      { label: "Kri&téria", command: "__submenu_criteria" },
      { separator: true },
      { label: "&Simulované žíhání", command: "resultSA", radio: "sa" },
      { label: "&Genetický algoritmus", command: "resultGA", radio: "ga" },
    ],
  },
  {
    label: "&Nápověda",
    items: [
      { label: "&Nápověda", accel: "F1", command: "helpHelp" },
      { separator: true },
      { label: "&O Programu", command: "helpAbout", icon: [2, 11] },
    ],
  },
];

export const CRITERIA_SUBMENU: MenuItem[] = [
  { label: "&Iterace výpočtu", command: "resultCritIteration" },
  { label: "&Relativní chyby", command: "resultCritRe" },
  { label: "&Průměr relativních chyb", command: "resultCritAre", icon: [2, 9] },
  {
    label: "&Celkové relativní chyby",
    command: "resultCritSoare",
    icon: [2, 10],
  },
];
