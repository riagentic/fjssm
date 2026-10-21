// Úprava jobů — IDD_JOBDLG 436×241 (FJSSM.rc), exact coordinates:
// due-date fields in a 2×2 grid, Alfa/Beta + Lam1-3 on the right.
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
import { FN } from "../../type/fn.ts";
import { isFuzzyDD } from "../../type/model.ts";
import { t } from "../i18n.ts";

const toNum = (s: string): number => {
  const v = parseFloat(s);
  return Number.isFinite(v) ? v : 0;
};

// d(1..4) label/edit positions from the .rc — a 2×2 grid
const DD_LABEL: [number, number][] = [[168, 163], [252, 163], [168, 181], [
  252,
  181,
]];
const DD_EDIT: [number, number][] = [[192, 157], [270, 156], [192, 174], [
  270,
  174,
]];

export function JobDlg() {
  const fdd = isFuzzyDD(doc.project.type);
  const { local: sel, set: setSel } = useLocal(0);
  const job = doc.project.jobs[sel];
  const { local: form, set: setForm, patch } = useLocal({
    name: job?.name ?? "",
    dd: (job ? [...job.dd] : [0, 0, 0, 0]) as FN,
    alfa: job?.alfa ?? 0,
    beta: job?.beta ?? 0,
    lambda1: job?.lambda1 ?? 0,
    lambda2: job?.lambda2 ?? 0,
    lambda3: job?.lambda3 ?? 0,
    color: job?.color ?? 0,
  });

  const select = (i: number) => {
    setSel(i);
    const j = doc.project.jobs[i];
    setForm({
      name: j?.name ?? "",
      dd: (j ? [...j.dd] : [0, 0, 0, 0]) as FN,
      alfa: j?.alfa ?? 0,
      beta: j?.beta ?? 0,
      lambda1: j?.lambda1 ?? 0,
      lambda2: j?.lambda2 ?? 0,
      lambda3: j?.lambda3 ?? 0,
      color: j?.color ?? 0,
    });
  };

  const apply = () => {
    if (job) doc.updateJob(sel, { ...form, dd: [...form.dd] as FN });
  };

  const setDD = (i: number, v: string) => {
    const dd = [...form.dd] as FN;
    dd[i] = Math.trunc(toNum(v));
    patch({ dd });
  };

  return (
    <Dialog title="Úprava jobů" w={436} h={241}>
      <GroupBox label="Seznam jobů" x={6} y={6} w={138} h={228} />
      <GroupBox label="Job" x={150} y={6} w={282} h={228} />
      <GroupBox label="Termín dokončení" x={162} y={144} w={162} h={54} />
      <GroupBox label="Koeficienty" x={330} y={60} w={90} h={60} />
      <GroupBox label="Váhy" x={330} y={126} w={90} h={72} />

      <select
        aria-label={t("Seznam jobů")}
        size={12}
        style={{ ...dlu(24, 24, 102, 144) }}
        onInput={(e) => select((e.target as HTMLSelectElement).selectedIndex)}
      >
        {doc.project.jobs.map((j, i) => (
          <option key={i} selected={i === sel}>{j.name}</option>
        ))}
      </select>
      <Btn
        text="&Přidat"
        x={78}
        y={180}
        onClick={async () => {
          const newName = `Job${doc.project.nJob + 1}`;
          const newIdx = doc.project.jobs.length;
          await doc.addJob(newName);
          setSel(newIdx);
          setForm({
            name: newName,
            dd: [0, 0, 0, 0] as FN,
            alfa: 0,
            beta: 0,
            lambda1: 0,
            lambda2: 0,
            lambda3: 0,
            color: newIdx % 20,
          });
        }}
      />
      <Btn
        text="&Odebrat"
        x={78}
        y={204}
        disabled={!job}
        onClick={async () => {
          if (
            !confirm(
              t("Opravdu chcete odstranit job včetně všech jeho operací?"),
            )
          ) return;
          const left = await doc.removeJob(sel);
          select(Math.max(0, Math.min(sel, left - 1)));
        }}
      />

      <Static text="Index jobu" x={168} y={25} w={42} h={8} />
      <Static text={job ? String(sel) : ""} x={210} y={25} w={24} h={8} />
      <Static text="Název" x={168} y={43} w={24} h={8} />
      <Edit
        label="Název jobu"
        value={form.name}
        x={210}
        y={42}
        w={198}
        disabled={!job}
        onInput={(v) => patch({ name: v.slice(0, 30) })}
      />
      <div
        style={{
          ...dlu(162, 66, 162, 72),
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
        }}
      >
        <img
          src="/assets/jobdlg.png"
          alt=""
          style={{ maxWidth: "100%", maxHeight: "100%" }}
        />
      </div>

      {
        /* fixed-length constant list: position IS the identity, and a key
          here would mix with the dialog's unkeyed static controls */
      }
      {[0, 1, 2, 3].map((i) => (
        <Static
          text={`d(${i + 1})`}
          x={DD_LABEL[i]![0]}
          y={DD_LABEL[i]![1]}
          w={13}
          h={8}
        />
      ))}
      {[0, 1, 2, 3].map((i) => (
        <Edit
          label={t("Termín dokončení d({0})", i + 1)}
          value={form.dd[i]!}
          x={DD_EDIT[i]![0]}
          y={DD_EDIT[i]![1]}
          w={40}
          disabled={!job || (!fdd && i > 0)}
          onInput={(v) => setDD(i, v)}
        />
      ))}

      <Static text="Alfa" x={336} y={83} w={13} h={8} />
      <Edit
        label="Alfa"
        value={form.alfa}
        x={366}
        y={78}
        w={40}
        disabled={!job || !fdd}
        onInput={(v) => patch({ alfa: toNum(v) })}
      />
      <Static text="Beta" x={336} y={103} w={16} h={8} />
      <Edit
        label="Beta"
        value={form.beta}
        x={366}
        y={95}
        w={40}
        disabled={!job || !fdd}
        onInput={(v) => patch({ beta: toNum(v) })}
      />

      <Static text="Lam1" x={336} y={144} w={18} h={8} />
      <Edit
        label="Lam1"
        value={form.lambda1}
        x={366}
        y={138}
        w={40}
        disabled={!job || !fdd}
        onInput={(v) => patch({ lambda1: toNum(v) })}
      />
      <Static text="Lam2" x={336} y={163} w={18} h={8} />
      <Edit
        label="Lam2"
        value={form.lambda2}
        x={366}
        y={156}
        w={40}
        disabled={!job || !fdd}
        onInput={(v) => patch({ lambda2: toNum(v) })}
      />
      <Static text="Lam3" x={336} y={181} w={18} h={8} />
      <Edit
        label="Lam3"
        value={form.lambda3}
        x={366}
        y={174}
        w={40}
        disabled={!job || !fdd}
        onInput={(v) => patch({ lambda3: toNum(v) })}
      />

      <ColorBtn
        x={168}
        y={210}
        color={form.color}
        onPick={(c) => patch({ color: c })}
      />
      <Btn text="Cancel" x={246} y={210} onClick={() => view.closeDialog()} />
      <Btn
        text="OK"
        x={306}
        y={210}
        def
        onClick={() => {
          apply();
          view.closeDialog();
        }}
      />
      <Btn text="&Upravit" x={366} y={210} disabled={!job} onClick={apply} />
    </Dialog>
  );
}
