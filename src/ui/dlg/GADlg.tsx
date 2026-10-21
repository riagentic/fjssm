// Genetický algoritmus — IDD_GADLG 358×270 (FJSSM.rc), exact coordinates.
// Original quirks preserved: "Počet hlavních cylů" typo, right group box
// captioned "Static", "Učelová funkce" labels.
import { useLocal } from "aio/air";
import { Btn, Dialog, Edit, GroupBox, Static } from "./Dialog.tsx";
import { Progress } from "./Progress.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";
import { strCriterion, strNRDouble } from "../../lib/format.ts";

const toNum = (s: string): number => {
  const v = parseFloat(s);
  return Number.isFinite(v) ? v : 0;
};
const clamp = (v: number, lo: number, hi: number): number =>
  Math.min(hi, Math.max(lo, Math.trunc(v)));

// edits at x=138, rows 24..132; labels at x=18 (from the .rc)
const FIELDS: [string, string, number][] = [
  ["Čas výpočtu", "time", 24],
  ["Velikost populace", "population", 42],
  ["Počet hlavních cylů", "cycle", 60],
  ["Koeficient výběru", "selCoef", 78],
  ["Min. DGD pro křížení", "minMSX", 96],
  ["Počet cyklů pro MSM", "msmCycle", 114],
  ["Počet cyklů pro lokální hledání", "lsCycle", 132],
];

const STAGES: [string, number][] = [
  ["Vytváření počáteční populace", 186],
  ["Selekce", 198],
  ["MSX Křížení", 210],
  ["MSM Mutace", 222],
  ["Lokální hledání", 234],
  ["Úprava populace", 246],
];
// gaAction values from UpdateDialogData: 0=init,1=selection,2=MSX,3=MSM,4=LS,5=update
const STAGE_FOR_ACTION = [0, 1, 2, 3, 4, 5];

export function GADlg(props: { paramsOnly?: boolean }) {
  const { local: form, patch } = useLocal({
    time: doc.ga.time,
    population: doc.ga.population,
    cycle: doc.ga.cycle,
    selCoef: doc.ga.selCoef,
    minMSX: doc.ga.minMSX,
    msmCycle: doc.ga.msmCycle,
    lsCycle: doc.ga.lsCycle,
  });

  const solving = doc.solving.action === 1;
  const s = doc.solving;

  // DDV ranges (GADlg.cpp:49-55)
  const commit = () => ({
    time: clamp(form.time, 1, 10000),
    population: clamp(form.population, 2, 1000),
    cycle: clamp(form.cycle, 1, 10000),
    selCoef: clamp(form.selCoef, 1, 100),
    minMSX: clamp(form.minMSX, 1, 1000),
    msmCycle: clamp(form.msmCycle, 1, 1000),
    lsCycle: clamp(form.lsCycle, 1, 10000),
  });

  return (
    <Dialog title="Genetický algoritmus" w={358} h={270}>
      <GroupBox label="Vstupní parametry" x={6} y={6} w={192} h={156} />
      {!props.paramsOnly && (
        <GroupBox label="-Aktuální akce" x={6} y={168} w={192} h={96} />
      )}
      <GroupBox label="Static" x={204} y={6} w={150} h={258} />

      {
        /* fixed-length constant list: position IS the identity, and a key
          here would mix with the dialog's unkeyed static controls */
      }
      {FIELDS.map(([label, , y]) => (
        <Static text={label} x={18} y={y + 6} w={114} h={8} />
      ))}
      {FIELDS.map(([label, key, y]) => (
        <Edit
          label={label}
          value={form[key as keyof typeof form]}
          x={138}
          y={y}
          w={40}
          disabled={solving}
          onInput={(v) => patch({ [key]: toNum(v) } as never)}
        />
      ))}

      {!props.paramsOnly && (
        <>
          {STAGES.map(([, y], i) => (
            <Static
              text={solving && STAGE_FOR_ACTION[s.gaAction] === i
                ? "----->"
                : ""}
              x={36}
              y={y}
              w={15}
              h={8}
            />
          ))}
          {STAGES.map(([label, y]) => (
            <Static text={label} x={66} y={y} w={97} h={8} />
          ))}
        </>
      )}

      <Static text="Aktuální kritérium:" x={216} y={19} w={57} h={8} />
      <Static
        pre
        text={strCriterion(doc.crit.criterion, 2, doc.crit.critMaxOrSum)}
        x={216}
        y={30}
        w={120}
        h={30}
      />
      <Static text="Nejlepší jedinec v populaci:" x={216} y={67} w={88} h={8} />
      <Static text="Učelová funkce " x={216} y={79} w={54} h={8} />
      <Static
        text={solving
          ? strNRDouble(s.xBest)
          : doc.results.ableResult & 2
          ? strNRDouble(doc.results.gaSchedule?.voof ?? 0)
          : ""}
        x={282}
        y={79}
        w={48}
        h={8}
      />
      <Static
        text="Poslední vyšlechtěný potomek:"
        x={216}
        y={97}
        w={99}
        h={8}
      />
      <Static text="Učelová funkce " x={216} y={109} w={54} h={8} />
      <Static
        text={solving ? strNRDouble(s.x0) : ""}
        x={282}
        y={109}
        w={36}
        h={8}
      />
      <Static text="Cyklus:" x={216} y={127} w={48} h={8} />
      <Static
        text={solving ? `${s.actCycle} z ${s.maxCycle}` : ""}
        x={258}
        y={127}
        w={69}
        h={8}
      />
      <Progress
        x={216}
        y={138}
        w={120}
        h={12}
        pos={s.actCycle}
        range={s.maxCycle}
      />
      <Static text="Čas:" x={216} y={157} w={48} h={8} />
      <Static
        text={solving ? `${s.runtime} z ${doc.ga.time} s` : ""}
        x={258}
        y={157}
        w={67}
        h={8}
      />
      <Progress
        x={216}
        y={168}
        w={120}
        h={12}
        pos={s.runtime}
        range={doc.ga.time}
      />

      <Btn
        text="&Kritérium"
        x={222}
        y={198}
        disabled={solving}
        onClick={() => view.openDialog("criteria")}
      />
      {!props.paramsOnly && (
        <Btn
          text="&Výpočet"
          x={282}
          y={198}
          def
          disabled={solving}
          onClick={async () => {
            await doc.setGAParams(commit());
            void doc.solveGA();
          }}
        />
      )}
      <Btn
        text="Cancel"
        x={222}
        y={234}
        disabled={solving}
        onClick={() => view.closeDialog()}
      />
      <Btn
        text="OK"
        x={282}
        y={234}
        def={props.paramsOnly}
        disabled={solving}
        onClick={async () => {
          await doc.setGAParams(commit());
          view.closeDialog();
        }}
      />
    </Dialog>
  );
}
