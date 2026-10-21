// Nastavení parametrů konverze — IDD_CONVERTDLG 220×180 (FJSSM.rc)
import { useLocal } from "aio/air";
import { Btn, Dialog, Edit, GroupBox, Static } from "./Dialog.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";

const toNum = (s: string): number => {
  const v = parseFloat(s);
  return Number.isFinite(v) ? v : 0;
};
const clamp01 = (v: number): number => Math.min(1, Math.max(0, v));

export function ConvertDlg() {
  const { local: form, patch } = useLocal({
    ddCoef: doc.conv.ddCoef,
    ptCoef: doc.conv.ptCoef,
    jobCoef: doc.conv.jobCoef,
    machineCoef: doc.conv.machineCoef,
  });

  return (
    <Dialog title="Nastavení parametrů konverze" w={220} h={180}>
      <GroupBox label="Převod na fuzzy číslo" x={6} y={6} w={210} h={66} />
      <Static
        text="Koeficient pro termíny dokončení"
        x={18}
        y={31}
        w={105}
        h={8}
      />
      <Edit
        label="Koeficient pro termíny dokončení"
        value={form.ddCoef}
        x={144}
        y={24}
        w={40}
        onInput={(v) => patch({ ddCoef: toNum(v) })}
      />
      <Static
        text="Koeficient pro doby trvání operací"
        x={18}
        y={49}
        w={108}
        h={8}
      />
      <Edit
        label="Koeficient pro doby trvání operací"
        value={form.ptCoef}
        x={144}
        y={42}
        w={40}
        onInput={(v) => patch({ ptCoef: toNum(v) })}
      />
      <GroupBox
        label="Automatické generování termínů dokončení"
        x={6}
        y={84}
        w={210}
        h={66}
      />
      <Static text="Koeficient vlivu počtu jobů" x={18} y={109} w={84} h={8} />
      <Edit
        label="Koeficient vlivu počtu jobů"
        value={form.jobCoef}
        x={144}
        y={102}
        w={40}
        onInput={(v) => patch({ jobCoef: toNum(v) })}
      />
      <Static
        text="Koeficient vlivu počtu strojů"
        x={18}
        y={127}
        w={120}
        h={8}
      />
      <Edit
        label="Koeficient vlivu počtu strojů"
        value={form.machineCoef}
        x={144}
        y={120}
        w={40}
        onInput={(v) => patch({ machineCoef: toNum(v) })}
      />
      <Btn
        text="OK"
        x={162}
        y={162}
        def
        onClick={() => {
          doc.setConvertSettings({
            ddCoef: clamp01(form.ddCoef),
            ptCoef: clamp01(form.ptCoef),
            jobCoef: clamp01(form.jobCoef),
            machineCoef: clamp01(form.machineCoef),
          });
          view.closeDialog();
        }}
      />
      <Btn text="Cancel" x={102} y={162} onClick={() => view.closeDialog()} />
    </Dialog>
  );
}
