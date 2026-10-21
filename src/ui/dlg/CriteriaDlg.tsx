// Výběr kritéria pro účelovou funkci — IDD_CRITERIADLG 382×265 (FJSSM.rc),
// exact radio/label/weight coordinates.
import { useLocal } from "aio/air";
import { Btn, Dialog, dlu, Edit, GroupBox, Static } from "./Dialog.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";
import { strCriterion } from "../../lib/format.ts";
import { criterionFlags } from "../../lib/criteria.ts";
import { isFuzzyDD, isFuzzyPT } from "../../type/model.ts";
import { t } from "../i18n.ts";

const toNum = (s: string): number => {
  const v = parseFloat(s);
  return Number.isFinite(v) ? v : 0;
};
const clampW = (v: number): number => Math.min(100, Math.max(0, v));

// radio y-position per criterion (x=24 left box, x=204 right box)
const CRIT_POS: [number, number][] = [
  [24, 30],
  [24, 66],
  [24, 84],
  [24, 102],
  [24, 144],
  [204, 36],
  [204, 108],
  [204, 132],
  [204, 156],
  [204, 180],
];
const CRIT_LABEL_X = [48, 48, 48, 48, 48, 222, 222, 222, 222, 222];

function Radio(
  props: {
    x: number;
    y: number;
    checked: boolean;
    disabled?: boolean;
    /** Accessible name — required, because most of these radios draw their
     *  visible text as a separately positioned element, not as `label`. */
    name: string;
    /** Visible text inside the <label>, when the radio carries its own. */
    label?: string;
    onPick: () => void;
  },
) {
  return (
    <label
      style={{
        ...dlu(props.x, props.y - 1),
        color: props.disabled ? "var(--graytext)" : "inherit",
        whiteSpace: "nowrap",
      }}
    >
      <input
        type="radio"
        aria-label={t(props.name)}
        checked={props.checked}
        disabled={props.disabled}
        onInput={props.onPick}
      />
      {props.label && t(props.label)}
    </label>
  );
}

export function CriteriaDlg() {
  const fpt = isFuzzyPT(doc.project.type);
  const fdd = isFuzzyDD(doc.project.type);
  const flags = criterionFlags(fpt, fdd);

  const { local: form, patch } = useLocal({
    criterion: doc.crit.criterion,
    maxOrSum: doc.crit.critMaxOrSum as 0 | 1,
    w1m: doc.crit.w1m,
    w2s: doc.crit.w2s,
    w1: doc.crit.w1,
    w2: doc.crit.w2,
    w3: doc.crit.w3,
    w4: doc.crit.w4,
  });

  return (
    <Dialog title="Výběr kritéria pro účelovou funkci" w={382} h={265}>
      <GroupBox
        label="Kritéria založená na makespanu"
        x={6}
        y={6}
        w={171}
        h={198}
      />
      <GroupBox label="Ostatní kritéria" x={186} y={6} w={192} h={198} />
      <GroupBox label="Typ účelové funkce" x={6} y={210} w={174} h={48} />

      <Static text="CPT /  *" x={24} y={18} w={27} h={8} />
      <Static text="FPT /  *" x={24} y={54} w={26} h={8} />
      <Static text="CPT / FDD kritéria" x={204} y={24} w={60} h={8} />
      <Static text="FPT /CDD kritéria" x={204} y={60} w={58} h={8} />
      <Static text="FPT / FDD kritéria" x={204} y={90} w={59} h={8} />

      {
        /* fixed-length constant list: position IS the identity, and a key
          here would mix with the dialog's unkeyed static controls */
      }
      {CRIT_POS.map(([x, y], i) => (
        <Radio
          x={x}
          y={y}
          name={strCriterion(i, 1)}
          checked={form.criterion === i}
          disabled={!flags[i]}
          onPick={() => patch({ criterion: i })}
        />
      ))}
      {CRIT_POS.map(([, y], i) => (
        <span
          style={{
            ...dlu(CRIT_LABEL_X[i]!, y, i >= 6 ? 150 : 120, 12),
            color: flags[i] ? "inherit" : "var(--graytext)",
            whiteSpace: "nowrap",
            overflow: "hidden",
          }}
        >
          {strCriterion(i, 1)}
        </span>
      ))}

      {/* w1m/w2s — criteria 2 & 3 (row 120) */}
      <Static text="w1" x={24} y={121} w={10} h={8} />
      <Edit
        label="w1 (varianta MAX)"
        value={form.w1m}
        x={48}
        y={120}
        w={30}
        h={12}
        disabled={!(form.criterion === 2 || form.criterion === 3)}
        onInput={(v) => patch({ w1m: clampW(toNum(v)) })}
      />
      <Static text="w2" x={84} y={121} w={10} h={8} />
      <Edit
        label="w2 (varianta SUMA)"
        value={form.w2s}
        x={102}
        y={120}
        w={30}
        h={12}
        disabled={!(form.criterion === 2 || form.criterion === 3)}
        onInput={(v) => patch({ w2s: clampW(toNum(v)) })}
      />

      {/* w1..w4 — criterion 4 (rows 162/180, two columns) */}
      <Static text="w1" x={24} y={163} w={10} h={8} />
      <Edit
        label="w1"
        value={form.w1}
        x={48}
        y={162}
        w={30}
        h={12}
        disabled={form.criterion !== 4}
        onInput={(v) => patch({ w1: clampW(toNum(v)) })}
      />
      <Static text="w2" x={24} y={181} w={10} h={8} />
      <Edit
        label="w2"
        value={form.w2}
        x={48}
        y={180}
        w={30}
        h={12}
        disabled={form.criterion !== 4}
        onInput={(v) => patch({ w2: clampW(toNum(v)) })}
      />
      <Static text="w3" x={84} y={163} w={10} h={8} />
      <Edit
        label="w3"
        value={form.w3}
        x={102}
        y={162}
        w={30}
        h={12}
        disabled={form.criterion !== 4}
        onInput={(v) => patch({ w3: clampW(toNum(v)) })}
      />
      <Static text="w4" x={84} y={181} w={10} h={8} />
      <Edit
        label="w4"
        value={form.w4}
        x={102}
        y={180}
        w={30}
        h={12}
        disabled={form.criterion !== 4}
        onInput={(v) => patch({ w4: clampW(toNum(v)) })}
      />

      <Radio
        x={18}
        y={228}
        name="Fmax"
        checked={form.maxOrSum === 0}
        onPick={() => patch({ maxOrSum: 0 as const })}
      />
      <Static text="Fmax" x={42} y={228} w={18} h={8} />
      <Radio
        x={18}
        y={239}
        name="Fsum"
        checked={form.maxOrSum === 1}
        onPick={() => patch({ maxOrSum: 1 as const })}
      />
      <Static text="Fsum" x={42} y={239} w={18} h={8} />

      <Btn
        text="OK"
        x={282}
        y={228}
        def
        onClick={() => {
          doc.setCriteria(form.criterion, form.maxOrSum, {
            w1m: form.w1m,
            w2s: form.w2s,
            w1: form.w1,
            w2: form.w2,
            w3: form.w3,
            w4: form.w4,
          });
          view.closeDialog();
        }}
      />
      <Btn text="Cancel" x={216} y={228} onClick={() => view.closeDialog()} />
    </Dialog>
  );
}
