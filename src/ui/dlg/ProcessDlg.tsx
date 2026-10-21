// Úprava operací — IDD_PROCESSDLG 412×265 (FJSSM.rc), exact coordinates:
// PT fields stacked a(1)..a(4) at x=210, trapezoid bitmap on the right.
import { useLocal } from "aio/air";
import { Btn, Dialog, dlu, Edit, GroupBox, Static } from "./Dialog.tsx";
import { doc } from "../../cell/doc.ts";
import { view } from "../view-store.ts";
import { FN } from "../../type/fn.ts";
import { isFuzzyPT } from "../../type/model.ts";
import { t } from "../i18n.ts";

const toInt = (s: string): number => {
  const v = parseInt(s, 10);
  return Number.isFinite(v) ? v : 0;
};

const PT_Y = [156, 174, 192, 211]; // a(1..4) edit rows from the .rc
const PT_LABEL_Y = [163, 181, 199, 217];

function IconBtn(
  props: {
    src: string;
    x: number;
    y: number;
    w: number;
    h: number;
    disabled?: boolean;
    onClick: () => void;
  },
) {
  return (
    <button
      type="button"
      style={{
        ...dlu(props.x, props.y, props.w, props.h),
        minWidth: 0,
        minHeight: 0,
        padding: 0,
      }}
      disabled={props.disabled}
      onClick={props.onClick}
    >
      <img
        src={props.src}
        alt=""
        style={{ width: "16px", height: "16px", imageRendering: "pixelated" }}
      />
    </button>
  );
}

export function ProcessDlg() {
  const fpt = isFuzzyPT(doc.project.type);
  const { local: jobIdx, set: setJobIdx } = useLocal(0);
  const { local: sel, set: setSel } = useLocal(0);
  const ops = doc.project.jobOps[jobIdx] ?? [];
  const op = ops[sel];
  const { local: form, set: setForm, patch } = useLocal({
    name: op?.name ?? "",
    machine: op?.machine ?? 0,
    pt: (op ? [...op.pt] : [0, 0, 0, 0]) as FN,
  });

  const loadOp = (j: number, i: number) => {
    const o = doc.project.jobOps[j]?.[i];
    setForm({
      name: o?.name ?? "",
      machine: o?.machine ?? 0,
      pt: (o ? [...o.pt] : [0, 0, 0, 0]) as FN,
    });
  };
  const selectOp = (i: number) => {
    setSel(i);
    loadOp(jobIdx, i);
  };
  const selectJob = (j: number) => {
    if (j < 0 || j >= doc.project.nJob) return;
    setJobIdx(j);
    setSel(0);
    loadOp(j, 0);
  };

  const apply = () => {
    if (op) {
      doc.updateProcess(jobIdx, sel, { ...form, pt: [...form.pt] as FN });
    }
  };

  return (
    <Dialog title="Úprava operací" w={412} h={265}>
      <GroupBox label="Seznam operací" x={6} y={6} w={156} h={252} />
      <GroupBox label="Job" x={168} y={6} w={234} h={66} />
      <GroupBox label="Operace" x={168} y={78} w={234} h={180} />

      <select
        aria-label={t("Seznam operací")}
        size={14}
        style={{ ...dlu(24, 30, 102, 186) }}
        onInput={(e) => selectOp((e.target as HTMLSelectElement).selectedIndex)}
      >
        {ops.map((o, i) => (
          <option key={i} selected={i === sel}>{o.name}</option>
        ))}
      </select>
      <IconBtn
        src="/assets/up.png"
        x={138}
        y={84}
        w={13}
        h={42}
        disabled={!op || sel === 0}
        onClick={async () => {
          await doc.moveProcess(jobIdx, sel, -1);
          setSel(sel - 1);
        }}
      />
      <IconBtn
        src="/assets/down.png"
        x={138}
        y={126}
        w={13}
        h={42}
        disabled={!op || sel >= ops.length - 1}
        onClick={async () => {
          await doc.moveProcess(jobIdx, sel, 1);
          setSel(sel + 1);
        }}
      />
      <Btn
        text="&Odebrat"
        x={24}
        y={228}
        disabled={!op}
        onClick={async () => {
          const len = ops.length;
          await doc.removeProcess(jobIdx, sel);
          selectOp(Math.max(0, Math.min(sel, len - 2)));
        }}
      />
      <Btn
        text="&Přidat"
        x={86}
        y={228}
        onClick={async () => {
          const newIdx = ops.length;
          await doc.addProcess(jobIdx);
          setSel(newIdx);
          setForm({
            name: t("Operace {0}-{1}", jobIdx, newIdx),
            machine: 0,
            pt: [0, 0, 0, 0] as FN,
          });
        }}
      />

      <Static text="Index jobu" x={186} y={19} w={34} h={8} />
      <Static text={String(jobIdx)} x={252} y={19} w={54} h={8} />
      <Static text="Název jobu" x={186} y={37} w={37} h={8} />
      <Static
        text={doc.project.jobs[jobIdx]?.name ?? ""}
        x={252}
        y={37}
        w={138}
        h={8}
      />
      <IconBtn
        src="/assets/left.png"
        x={240}
        y={54}
        w={48}
        h={14}
        disabled={jobIdx === 0}
        onClick={() => selectJob(jobIdx - 1)}
      />
      <IconBtn
        src="/assets/right.png"
        x={294}
        y={54}
        w={48}
        h={14}
        disabled={jobIdx >= doc.project.nJob - 1}
        onClick={() => selectJob(jobIdx + 1)}
      />

      <Static text="Index operace" x={186} y={91} w={46} h={8} />
      <Static text={op ? String(sel) : ""} x={246} y={91} w={54} h={8} />
      <Static text="Název operace" x={186} y={109} w={50} h={8} />
      <Edit
        label="Název operace"
        value={form.name}
        x={246}
        y={102}
        w={150}
        disabled={!op}
        onInput={(v) => patch({ name: v.slice(0, 30) })}
      />
      <Static text="Stroj operace" x={186} y={127} w={43} h={8} />
      <select
        aria-label={t("Stroj operace")}
        style={{ ...dlu(246, 126, 150, 14) }}
        disabled={!op}
        onInput={(e) =>
          patch({ machine: (e.target as HTMLSelectElement).selectedIndex })}
      >
        {doc.project.machines.map((m, i) => (
          <option key={i} selected={i === form.machine}>{i} - {m.name}</option>
        ))}
      </select>
      <Static text="Čas zpracování" x={186} y={145} w={52} h={8} />
      {
        /* fixed-length constant list: position IS the identity, and a key
          here would mix with the dialog's unkeyed static controls */
      }
      {[0, 1, 2, 3].map((i) => (
        <Static
          text={`a(${i + 1})`}
          x={186}
          y={PT_LABEL_Y[i]!}
          w={13}
          h={8}
        />
      ))}
      {[0, 1, 2, 3].map((i) => (
        <Edit
          label={t("Čas zpracování a({0})", i + 1)}
          value={form.pt[i]!}
          x={210}
          y={PT_Y[i]!}
          w={40}
          disabled={!op || (!fpt && i > 0)}
          onInput={(v) => {
            const pt = [...form.pt] as FN;
            pt[i] = toInt(v);
            patch({ pt });
          }}
        />
      ))}
      <div
        style={{
          ...dlu(258, 144, 138, 60),
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
        }}
      >
        <img
          src="/assets/processdlg.png"
          alt=""
          style={{ maxWidth: "100%", maxHeight: "100%" }}
        />
      </div>
      <Btn text="&Upravit" x={342} y={210} disabled={!op} onClick={apply} />
      <Btn text="Cancel" x={282} y={234} onClick={() => view.closeDialog()} />
      <Btn
        text="OK"
        x={342}
        y={234}
        def
        onClick={() => {
          apply();
          view.closeDialog();
        }}
      />
    </Dialog>
  );
}
