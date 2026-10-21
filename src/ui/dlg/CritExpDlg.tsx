// Srovnání kritérií — IDD_CRITEXP 238×265 (FJSSM.rc), exact coordinates
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

export function CritExpDlg() {
  const { local: form, patch } = useLocal({
    iteration: doc.cre.iteration,
    method: doc.cre.method as 0 | 1,
  });

  const solving = doc.solving.action === 3;
  const s = doc.solving;

  const commit = () =>
    doc.setCRE({
      iteration: Math.max(1, form.iteration),
      method: form.method,
    });

  return (
    <Dialog title="Srovnání kritérií" w={238} h={265}>
      <GroupBox
        label="Nastavení řídících parametrů"
        x={6}
        y={6}
        w={228}
        h={252}
      />

      <Static text="Počet iterací" x={18} y={25} w={41} h={8} />
      <Edit
        label="Počet iterací"
        value={form.iteration}
        x={72}
        y={18}
        w={40}
        disabled={solving}
        onInput={(v) => patch({ iteration: toInt(v) })}
      />
      <Static text="Použít metodu" x={18} y={42} w={46} h={8} />
      <label style={{ ...dlu(18, 54, 74, 10), whiteSpace: "nowrap" }}>
        <input
          type="radio"
          aria-label={t("Simulované žíhání")}
          checked={form.method === 0}
          disabled={solving}
          onInput={() => patch({ method: 0 as const })}
        />
        {t("Simulované žíhání")}
      </label>
      <label style={{ ...dlu(18, 66, 81, 10), whiteSpace: "nowrap" }}>
        <input
          type="radio"
          aria-label={t("Genetický algoritmus")}
          checked={form.method === 1}
          disabled={solving}
          onInput={() => patch({ method: 1 as const })}
        />
        {t("Genetický algoritmus")}
      </label>
      <Static text="Aktuální určující kritérium" x={18} y={84} w={80} h={8} />
      <Static
        pre
        text={strCriterion(doc.crit.criterion, 2, doc.crit.critMaxOrSum, t)}
        x={18}
        y={97}
        w={204}
        h={42}
      />
      <Static
        text="Průběh podle aktuálního kriteria"
        x={18}
        y={150}
        w={101}
        h={8}
      />
      <Progress
        x={18}
        y={162}
        w={204}
        h={12}
        pos={s.partPos}
        range={s.partRange}
      />
      <Static text="Celkový čas výpočtu" x={18} y={180} w={67} h={8} />
      <Progress
        x={18}
        y={192}
        w={204}
        h={12}
        pos={s.allPos}
        range={s.allRange}
      />

      <Btn
        text="Výpočet"
        x={174}
        y={210}
        def
        disabled={solving}
        onClick={async () => {
          if (
            !confirm(
              t("Výpočet může trvat i NĚKOLIK MINUT, opravdu chcete pokračovat ?"),
            )
          ) return;
          await commit();
          void doc.solveCriteria();
        }}
      />
      <Btn
        text="OK"
        x={174}
        y={234}
        def
        disabled={solving}
        onClick={async () => {
          await commit();
          view.closeDialog();
        }}
      />
      <Btn
        text="Cancel"
        x={114}
        y={234}
        disabled={solving}
        onClick={() => view.closeDialog()}
      />
    </Dialog>
  );
}
