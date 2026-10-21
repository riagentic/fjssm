// Úprava strojů — IDD_MACHINEDLG 400×126 (FJSSM.rc), exact coordinates
import { useLocal } from "aio/air";
import {
  Btn,
  ColorBtn,
  Dialog,
  dlu,
  Edit,
  GroupBox,
  Static,
} from "./Dialog.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";

export function MachineDlg() {
  const { local: sel, set: setSel } = useLocal(0);
  const machine = doc.project.machines[sel];
  const { local: name, set: setName } = useLocal(machine?.name ?? "");
  const { local: color, set: setColor } = useLocal(machine?.color ?? 0);

  const select = (i: number) => {
    setSel(i);
    const m = doc.project.machines[i];
    setName(m?.name ?? "");
    setColor(m?.color ?? 0);
  };

  return (
    <Dialog title="Úprava strojů" w={400} h={126}>
      <GroupBox label="Seznam strojů" x={6} y={0} w={168} h={120} />
      <GroupBox label="Stroj" x={180} y={0} w={216} h={120} />
      <select
        aria-label="Seznam strojů"
        size={8}
        style={{ ...dlu(24, 12, 78, 96) }}
        onInput={(e) => select((e.target as HTMLSelectElement).selectedIndex)}
      >
        {doc.project.machines.map((m, i) => (
          <option key={i} selected={i === sel}>{m.name}</option>
        ))}
      </select>
      <img
        src="/assets/machinedlg.png"
        alt=""
        style={{ ...dlu(120, 18), imageRendering: "pixelated" }}
      />
      <Btn
        text="&Přidat"
        x={114}
        y={72}
        onClick={async () => {
          const newName = `Stroj${doc.project.nMachine + 1}`;
          const newIdx = doc.project.machines.length;
          await doc.addMachine(newName);
          setSel(newIdx);
          setName(newName);
          setColor(newIdx % 20);
        }}
      />
      <Btn
        text="&Odebrat"
        x={114}
        y={96}
        disabled={!machine}
        onClick={async () => {
          if (
            !confirm(
              "Opravdu chcete odstranit stroj včetně všech jemu přiřazených operací?",
            )
          ) return;
          const left = await doc.removeMachine(sel);
          select(Math.max(0, Math.min(sel, left - 1)));
        }}
      />
      <Static text="Index stroje" x={192} y={25} w={37} h={8} />
      <Static text={machine ? String(sel) : ""} x={258} y={25} w={37} h={8} />
      <Static text="Název stroje" x={192} y={49} w={40} h={8} />
      <Edit
        label="Název stroje"
        value={name}
        x={258}
        y={42}
        w={120}
        disabled={!machine}
        onInput={(v) => setName(v.slice(0, 30))}
      />
      <ColorBtn x={270} y={72} color={color} onPick={setColor} />
      <Btn
        text="&Upravit"
        x={330}
        y={72}
        disabled={!machine}
        onClick={() => doc.updateMachine(sel, name, color)}
      />
      <Btn
        text="OK"
        x={330}
        y={96}
        def
        onClick={() => {
          if (machine) doc.updateMachine(sel, name, color);
          view.closeDialog();
        }}
      />
      <Btn text="Cancel" x={270} y={96} onClick={() => view.closeDialog()} />
    </Dialog>
  );
}
