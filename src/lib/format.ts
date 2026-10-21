// Number/string rendering — port of CFJSS::GetStrNR / GetStrType /
// GetStrMethod / GetStrCriterion / GetStrNode (FJSSIO.cpp:11-201)
import { FN, FN_PART } from "../type/fn.ts";
import { Project, typeProps } from "../type/model.ts";
import { RNode } from "../type/engine.ts";

export const NR_PT = 0;
export const NR_DD = 1;

/** printf "%3d" */
const d3 = (n: number): string => String(Math.trunc(n)).padStart(3, " ");

/** GetStrNR(CFN&, nrType) — "%3d"-joined components (FJSSIO.cpp:11-39) */
export function strNRFn(p: Project, fn: FN, nrType = NR_PT): string {
  const { ptPart, ddPart } = typeProps(p.type);
  const parts = nrType === NR_PT ? ptPart : nrType === NR_DD ? ddPart : FN_PART;
  const out: string[] = [];
  for (let i = 0; i < parts; i++) out.push(d3(fn[i]!));
  return out.join(",");
}

/** GetStrNR(int) */
export const strNRInt = (n: number): string => String(Math.trunc(n));

/** GetStrNR(double, nInt=3, nDec=2) — printf "%3.2f" */
export function strNRDouble(f: number, nInt = 3, nDec = 2): string {
  return f.toFixed(nDec).padStart(nInt, " ");
}

/** GetStrNode (FJSSIO.cpp:77-100): "j,p,m (PT)" or fat "j,p,m PT (r - C)" */
export function strNode(p: Project, node: RNode, nType = 0): string {
  const strPT = strNRFn(p, node.pt, NR_PT);
  if (nType === 0) {
    return `${node.job},${node.process},${node.machine} (${strPT})`;
  }
  const strR = strNRFn(p, node.r, NR_PT);
  const strC = strNRFn(p, node.C, NR_PT);
  return `${node.job},${node.process},${node.machine} ${strPT} (${strR} - ${strC})`;
}

/** GetStrType (FJSSIO.cpp:122-136) */
export function strType(p: Project): string {
  const jm = `(${p.nJob} x ${p.nMachine})`;
  switch (p.type) {
    case 0:
      return `${jm} - Deterministický případ`;
    case 1:
      return `${jm} - Fuzzy termíny dokončení`;
    case 2:
      return `${jm} - Fuzzy časy operací`;
    case 3:
      return `${jm} - Fuzzy termíny dokončení i fuzzy časy operací`;
    default:
      return `${jm} - Neznámý případ, Chyba !`;
  }
}

/** GetStrMethod (FJSSIO.cpp:138-150) */
export function strMethod(nType: number): string {
  switch (nType) {
    case 0:
      return "Simulované žíhání";
    case 1:
      return "Genetický algoritmus";
    default:
      return "";
  }
}

const CRIT_FORMULAS = [
  "Crisp Makespan",
  "F(Ci) = m(Ci)",
  "F(Ci) = w1m(Ci) + w2s(Ci)",
  "F(Ci) = w1m(Ci) + w2sR(Ci)",
  "F(Ci) = Suma(wi * a(i))",
  "F(Ci) = Ext_DD(Ci)",
  "F(Ci) = area(Ci n Di) / area(Ci)",
  "F(Ci) = area(Ci n Di) / area(Ci u Di)",
  "F(Ci) = area(CiE)+Area(CiT)-Area(CiD)+G(P)",
  "F(Ci) = nwi1 agr(Ci,Di)+wi2(1-dist(Ci,Di))",
];

/** GetStrCriterion (FJSSIO.cpp:153-201)
 *  nType 0: "(CPT/*)" tag only; 1: formula; 2: tag\nformula\nvariant; 3: formula */
export function strCriterion(
  criterion: number,
  nType: number,
  critMaxOrSum = 0,
): string {
  let str = "";
  if (nType === 0 || nType === 2) {
    if (criterion === 0) str = "(CPT/*)";
    else if (criterion < 5) str = "(FPT/*)";
    else if (criterion < 6) str = "(FPT/CDD)";
    else str = "(FPT/FDD)";
  }
  if (nType === 2) str += "\n";
  if (nType === 1 || nType === 2 || nType === 3) {
    str += CRIT_FORMULAS[criterion] ?? "Neznámé kriterium";
  }
  if (nType === 2) {
    str += critMaxOrSum === 0 ? "\nVarianta Max" : "\nVarianta Suma";
  }
  return str;
}
