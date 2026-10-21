// Konvergence metod — IDD_CONVEXPDLG 412×234 (FJSSM.rc), exact coordinates.
// Original quirks preserved: lower-left group captioned "Průběh výpočů",
// arrows are "---------->" (ten dashes).
import { useLocal } from "aio/air";
import { Btn, Dialog, dlu, Edit, GroupBox, Static } from "./Dialog.tsx";
import { Progress } from "./Progress.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";
import { strCriterion } from "../../lib/format.ts";
import { t } from "../i18n.ts";

const toInt = (s: string): number => {
  const v = parseInt(s, 10);
  return Number.isFinite(v) ? v : 0;
};
const clamp = (v: number, lo: number, hi: number): number =>
  Math.min(hi, Math.max(lo, v));

export function ConvExpDlg() {
  const { local: form, patch } = useLocal({
    solveCount: doc.coe.solveCount,
    solveTime: doc.coe.solveTime,
    allCriterion: doc.coe.allCriterion,
  });

  const solving = doc.solving.action === 2;
  const s = doc.solving;

  const commit = () =>
    doc.setCOE({
      solveCount: clamp(form.solveCount, 1, 100),
      solveTime: clamp(form.solveTime, 1, 1000),
      allCriterion: form.allCriterion,
    });

  return (
    <Dialog title="Konvergence metod" w={412} h={234}>
      <GroupBox
        label="Nastavení parametrů výpočtu"
        x={6}
        y={6}
        w={228}
        h={138}
      />
      <GroupBox label="Průběh výpočů" x={6} y={150} w={228} h={78} />
      <GroupBox label="Průběh výpočtu" x={240} y={6} w={168} h={222} />

      <Static
        text="Počet výpočtů pro každou metodu"
        x={36}
        y={25}
        w={110}
        h={8}
      />
      <Edit
        label="Počet výpočtů pro každou metodu"
        value={form.solveCount}
        x={174}
        y={18}
        w={40}
        disabled={solving}
        onInput={(v) => patch({ solveCount: toInt(v) })}
      />
      <Static text="Čas jednoho výpočtu" x={36} y={43} w={68} h={8} />
      <Edit
        label="Čas jednoho výpočtu"
        value={form.solveTime}
        x={174}
        y={36}
        w={40}
        disabled={solving}
        onInput={(v) => patch({ solveTime: toInt(v) })}
      />
      <label style={{ ...dlu(36, 66, 119, 10), whiteSpace: "nowrap" }}>
        <input
          type="checkbox"
          aria-label={t("Použít všechna dostupná kritéria")}
          checked={form.allCriterion}
          disabled={solving}
          onInput={() => patch({ allCriterion: !form.allCriterion })}
        />
        {t("Použít všechna dostupná kritéria")}
      </label>
      <Btn
        text="&Kritérium"
        x={36}
        y={90}
        disabled={solving || form.allCriterion}
        onClick={() => view.openDialog("criteria")}
      />
      <Btn
        text="Param. SA"
        x={36}
        y={114}
        disabled={solving}
        onClick={() => view.openDialog("sa-params")}
      />
      <Btn
        text="Param. GA"
        x={90}
        y={114}
        disabled={solving}
        onClick={() => view.openDialog("ga-params")}
      />
      <Btn
        text="Výpočet"
        x={168}
        y={114}
        def
        disabled={solving}
        onClick={async () => {
          if (
            !confirm(
              t("Výpočet může trvat i NĚKOLIK MINUT, opravdu chcete pokračovat ?"),
            )
          ) return;
          await commit();
          void doc.solveConvergence();
        }}
      />

      <Static text="Varianta" x={30} y={168} w={36} h={8} />
      <Static
        text={solving && s.critMaxOrSum === 0 ? "---------->" : ""}
        x={72}
        y={168}
        w={25}
        h={8}
      />
      <Static text="Varianta MAX" x={108} y={168} w={44} h={8} />
      <Static
        text={solving && s.critMaxOrSum === 1 ? "---------->" : ""}
        x={72}
        y={180}
        w={25}
        h={8}
      />
      <Static text="Varianta SUMA" x={108} y={180} w={50} h={8} />
      <Static text="Metoda" x={30} y={198} w={36} h={8} />
      <Static
        text={solving && s.subAction === 0 ? "---------->" : ""}
        x={72}
        y={198}
        w={25}
        h={8}
      />
      <Static text="Simulované žíhání" x={108} y={198} w={70} h={8} />
      <Static
        text={solving && s.subAction === 1 ? "---------->" : ""}
        x={72}
        y={210}
        w={25}
        h={8}
      />
      <Static text="Genetický algoritmus" x={108} y={210} w={70} h={8} />

      <Static text="Kritérium" x={246} y={25} w={42} h={8} />
      <Static
        pre
        text={strCriterion(
          solving ? s.criterion : doc.crit.criterion,
          2,
          solving ? s.critMaxOrSum : doc.crit.critMaxOrSum,
          t,
        )}
        x={252}
        y={36}
        w={144}
        h={48}
      />
      <Static text="Aktuální výpočet" x={252} y={114} w={72} h={8} />
      <Progress
        x={252}
        y={126}
        w={144}
        h={12}
        pos={s.partPos}
        range={s.partRange}
      />
      <Static text="Celkový průběh" x={252} y={150} w={60} h={8} />
      <Progress
        x={252}
        y={162}
        w={144}
        h={12}
        pos={s.allPos}
        range={s.allRange}
      />

      <Btn
        text="OK"
        x={336}
        y={204}
        disabled={solving}
        onClick={async () => {
          await commit();
          view.closeDialog();
        }}
      />
      <Btn
        text="Cancel"
        x={264}
        y={204}
        disabled={solving}
        onClick={() => view.closeDialog()}
      />
    </Dialog>
  );
}
