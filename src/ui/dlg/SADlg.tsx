// Simulované žíhání — IDD_SADLG 328×241 (FJSSM.rc), exact coordinates
import { useLocal } from "aio/air";
import { Btn, Dialog, Edit, GroupBox, Static } from "./Dialog.tsx";
import { Progress } from "./Progress.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";
import { strCriterion, strNRDouble } from "../../lib/format.ts";
import { t } from "../i18n.ts";

const toNum = (s: string): number => {
  const v = parseFloat(s);
  return Number.isFinite(v) ? v : 0;
};
const clamp = (v: number, lo: number, hi: number): number =>
  Math.min(hi, Math.max(lo, v));

// label y-coords per the .rc; edits at x=108, rows 18..108
const FIELDS: [string, string, number][] = [
  ["Maximální čas výpočtu", "time", 18],
  ["Minimální teplota", "minTemp", 36],
  ["Počáteční teplota", "temp", 54],
  ["Počáteční počet cyklů", "cycle", 72],
  ["Koeficient redukce teploty", "tempCoef", 90],
  ["Koeficient délky cyklu", "cycleCoef", 108],
];

export function SADlg(props: { paramsOnly?: boolean }) {
  const { local: form, patch } = useLocal({
    time: doc.sa.time,
    minTemp: doc.sa.minTemp,
    temp: doc.sa.temp,
    cycle: doc.sa.cycle,
    tempCoef: doc.sa.tempCoef,
    cycleCoef: doc.sa.cycleCoef,
  });

  const solving = doc.solving.action === 0;
  const s = doc.solving;

  // DDV ranges (SADlg.cpp:54-59)
  const commit = () => ({
    time: clamp(Math.trunc(form.time), 1, 10000),
    minTemp: clamp(Math.trunc(form.minTemp), 0, 10000),
    temp: clamp(form.temp, 1, 10000),
    cycle: clamp(Math.trunc(form.cycle), 1, 10000),
    tempCoef: clamp(form.tempCoef, 0, 0.999999),
    cycleCoef: clamp(Math.trunc(form.cycleCoef), 0, 100),
  });

  return (
    <Dialog title="Simulované žíhání" w={328} h={241}>
      <GroupBox label="Vstupní parametry výpočtu" x={6} y={6} w={162} h={132} />
      <GroupBox label="Průběh výpočtu" x={174} y={6} w={150} h={132} />
      {!props.paramsOnly && (
        <GroupBox label="Výsledky" x={6} y={144} w={162} h={90} />
      )}

      {
        /* fixed-length constant list: position IS the identity, and a key
          here would mix with the dialog's unkeyed static controls */
      }
      {FIELDS.map(([label, , y]) => (
        <Static text={label} x={18} y={y + 6} w={90} h={8} />
      ))}
      {FIELDS.map(([label, key, y]) => (
        <Edit
          label={label}
          value={form[key as keyof typeof form]}
          x={108}
          y={y}
          w={40}
          disabled={solving}
          onInput={(v) => patch({ [key]: toNum(v) } as never)}
        />
      ))}

      <Static text="Aktuální teplota: " x={186} y={25} w={54} h={8} />
      <Static
        text={solving ? strNRDouble(s.temp) : ""}
        x={252}
        y={25}
        w={47}
        h={8}
      />
      <Static text="Průběh cyklů:" x={186} y={61} w={44} h={8} />
      <Static
        text={solving ? t("{0} z {1}", s.actCycle, s.maxCycle) : ""}
        x={240}
        y={61}
        w={66}
        h={8}
      />
      <Progress
        x={186}
        y={72}
        w={120}
        h={12}
        pos={s.actCycle}
        range={s.maxCycle}
      />
      <Static text="Čas výpočtu" x={186} y={91} w={40} h={8} />
      <Static
        text={solving ? t("{0} z {1} s", s.runtime, doc.sa.time) : ""}
        x={240}
        y={91}
        w={72}
        h={8}
      />
      <Progress
        x={186}
        y={102}
        w={120}
        h={12}
        pos={s.runtime}
        range={doc.sa.time}
      />

      {!props.paramsOnly && (
        <>
          <Static text="Aktuální kritérium:" x={18} y={158} w={57} h={8} />
          <Static
            pre
            text={strCriterion(doc.crit.criterion, 2, doc.crit.critMaxOrSum, t)}
            x={18}
            y={168}
            w={138}
            h={30}
          />
          <Static
            text="Nejlepší dosažená hodnota:"
            x={18}
            y={211}
            w={89}
            h={8}
          />
          <Static
            text={solving
              ? strNRDouble(s.xBest)
              : doc.results.ableResult & 1
              ? strNRDouble(doc.results.saSchedule?.voof ?? 0)
              : ""}
            x={114}
            y={211}
            w={48}
            h={8}
          />
        </>
      )}

      <Btn
        text="&Kritérium"
        x={198}
        y={156}
        disabled={solving}
        onClick={() => view.openDialog("criteria")}
      />
      {!props.paramsOnly && (
        <Btn
          text="&Výpočet"
          x={258}
          y={156}
          def
          disabled={solving}
          onClick={async () => {
            await doc.setSAParams(commit());
            void doc.solveSA();
          }}
        />
      )}
      <Btn
        text="Cancel"
        x={198}
        y={198}
        disabled={solving}
        onClick={() => view.closeDialog()}
      />
      <Btn
        text="OK"
        x={258}
        y={198}
        def={props.paramsOnly}
        disabled={solving}
        onClick={async () => {
          await doc.setSAParams(commit());
          view.closeDialog();
        }}
      />
    </Dialog>
  );
}
