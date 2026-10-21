// Projekt — IDD_PROJECTDLG 226×114 (FJSSM.rc), controls at exact coordinates
import { useLocal } from "aio/air";
import { Btn, Dialog, Edit, GroupBox, Static } from "./Dialog.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";

export function ProjectDlg() {
  const { local: name, set: setName } = useLocal(doc.project.name);
  const { local: maker, set: setMaker } = useLocal(doc.project.maker);

  return (
    <Dialog title="Projekt" w={226} h={114}>
      <GroupBox label=" Projekt " x={6} y={0} w={216} h={78} />
      <Static text="Název projektu" x={24} y={19} w={49} h={8} />
      <Edit
        label="Název projektu"
        value={name}
        x={90}
        y={18}
        w={120}
        onInput={setName}
      />
      <Static text="Vytvořil" x={24} y={37} w={24} h={8} />
      <Edit
        label="Vytvořil"
        value={maker}
        x={90}
        y={36}
        w={84}
        onInput={setMaker}
      />
      <Static text="Datum vytvoření" x={24} y={55} w={54} h={8} />
      <Static text={doc.project.date} x={90} y={55} w={84} h={8} />
      <Btn
        text="OK"
        x={168}
        y={90}
        def
        onClick={() => {
          doc.setProjectInfo(name, maker);
          view.closeDialog();
        }}
      />
      <Btn text="Cancel" x={108} y={90} onClick={() => view.closeDialog()} />
    </Dialog>
  );
}
