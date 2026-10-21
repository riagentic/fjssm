// Shared dialog chrome — modal frame with title-bar dragging, absolute
// dialog-unit positioning (controls placed at their exact .rc coordinates),
// Enter-activates-default-button and first-control autofocus, like DoModal.
// DLU scale for 8pt MS Sans Serif: x≈1.5px/DLU, y≈1.625px/DLU.
import { onCleanup, onMount, useLocal, useRef } from "aio/air";
import { view } from "../view-store.ts";
import { PALETTE } from "../../lib/palette.ts";

export const DX = 1.5;
export const DY = 1.625;

export function dlu(x: number, y: number, w?: number, h?: number) {
  const style: Record<string, string> = {
    position: "absolute",
    left: `${Math.round(x * DX)}px`,
    top: `${Math.round(y * DY)}px`,
  };
  if (w !== undefined) style.width = `${Math.round(w * DX)}px`;
  if (h !== undefined) style.height = `${Math.round(h * DY)}px`;
  return style;
}

export function Dialog(
  props: {
    title: string;
    w: number;
    h: number;
    children?: unknown;
    onClose?: () => void;
  },
) {
  const close = props.onClose ?? (() => view.closeDialog());
  const { local: pos, set: setPos } = useLocal<{ x: number; y: number } | null>(
    null,
  );
  const frameRef = useRef<HTMLElement>(null!);
  const drag = useRef<{ dx: number; dy: number } | null>(null);

  onMount(() => {
    // Center on open (DoModal), then focus the first enabled field. Deferred
    // one frame so the dialog is laid out (measuring in onMount can read
    // width 0, which would place it off-center).
    setTimeout(() => {
      const el = frameRef.current;
      if (!el) return;
      const r = el.getBoundingClientRect();
      if (r.width > 0) {
        setPos({
          x: Math.max(0, Math.round((globalThis.innerWidth - r.width) / 2)),
          y: Math.max(0, Math.round((globalThis.innerHeight - r.height) / 2)),
        });
      }
      // Never steal focus if the user is already interacting with a control
      // inside the dialog. Prefer a text field over a list box.
      if (!el.contains(document.activeElement)) {
        const field = el.querySelector<HTMLElement>(
          "input:not([disabled]), textarea:not([disabled])",
        ) ?? el.querySelector<HTMLElement>("select:not([disabled])");
        field?.focus();
      }
    }, 0);

    const onMove = (e: MouseEvent) => {
      if (!drag.current) return;
      setPos({
        x: e.clientX - drag.current.dx,
        y: e.clientY - drag.current.dy,
      });
    };
    const onUp = () => {
      drag.current = null;
    };
    globalThis.addEventListener("mousemove", onMove);
    globalThis.addEventListener("mouseup", onUp);
    onCleanup(() => {
      globalThis.removeEventListener("mousemove", onMove);
      globalThis.removeEventListener("mouseup", onUp);
    });
  });

  return (
    <div className="dlg-overlay">
      <div
        ref={frameRef as unknown as { current: HTMLElement | null }}
        className="dialog raised"
        style={{
          position: "absolute",
          left: pos ? `${pos.x}px` : "50%",
          top: pos ? `${pos.y}px` : "40%",
          transform: pos ? undefined : "translate(-50%, -40%)",
          width: `${Math.round(props.w * DX) + 8}px`,
        }}
        onKeyDown={(e) => {
          const ev = e as KeyboardEvent;
          if (ev.key !== "Enter") return;
          const target = ev.target as HTMLElement;
          if (target.tagName === "BUTTON") return;
          const def = (ev.currentTarget as HTMLElement).querySelector<
            HTMLButtonElement
          >("button.default:not([disabled])");
          if (def) {
            ev.preventDefault();
            def.click();
          }
        }}
      >
        <div
          className="dlg-title"
          onMouseDown={(e) => {
            const ev = e as MouseEvent;
            if ((ev.target as HTMLElement).closest(".dlg-close")) return;
            const r = frameRef.current!.getBoundingClientRect();
            drag.current = { dx: ev.clientX - r.left, dy: ev.clientY - r.top };
            ev.preventDefault();
          }}
        >
          <span>{props.title}</span>
          <button
            type="button"
            className="dlg-close raised-thin"
            onClick={close}
          >
            ✕
          </button>
        </div>
        <div
          className="dlg-body"
          style={{
            position: "relative",
            height: `${Math.round(props.h * DY) + 6}px`,
            margin: "0 3px 3px",
          }}
        >
          {props.children}
        </div>
      </div>
    </div>
  );
}

/** MFC group box: a bordered frame drawn UNDER sibling controls at its exact
 *  .rc rectangle — it is NOT a container; controls are positioned absolutely
 *  in dialog coordinates, exactly like the resource script lays them out. */
export function GroupBox(
  props: { label: string; x: number; y: number; w: number; h: number },
) {
  return (
    <div
      style={{
        ...dlu(props.x, props.y + 4, props.w, props.h - 4),
        border: "1px solid",
        borderColor:
          "var(--btnshadow) var(--btnhighlight) var(--btnhighlight) var(--btnshadow)",
        boxShadow:
          "1px 1px 0 var(--btnhighlight) inset, -1px -1px 0 var(--btnshadow)",
        pointerEvents: "none",
      }}
    >
      <span
        style={{
          position: "absolute",
          top: "-8px",
          left: `${Math.round(6 * DX)}px`,
          background: "var(--btnface)",
          padding: "0 3px",
          lineHeight: "14px",
          whiteSpace: "nowrap",
        }}
      >
        {props.label}
      </span>
    </div>
  );
}

export function Static(
  props: {
    text: string;
    x: number;
    y: number;
    w?: number;
    h?: number;
    pre?: boolean;
  },
) {
  return (
    <span
      style={{
        ...dlu(props.x, props.y, props.w, props.h),
        whiteSpace: props.pre ? "pre-line" : "nowrap",
        overflow: "hidden",
      }}
    >
      {props.text}
    </span>
  );
}

export function Btn(
  props: {
    text: string;
    x: number;
    y: number;
    w?: number;
    h?: number;
    def?: boolean;
    disabled?: boolean;
    onClick: () => void;
  },
) {
  return (
    <button
      type="button"
      className={props.def ? "default" : undefined}
      style={{
        ...dlu(props.x, props.y, props.w ?? 50, props.h ?? 16),
        minWidth: "0",
        minHeight: "0",
        padding: "0",
      }}
      disabled={props.disabled}
      onClick={props.onClick}
    >
      {props.text.replace("&", "")}
    </button>
  );
}

export function Edit(
  props: {
    value: string | number;
    x: number;
    y: number;
    w?: number;
    h?: number;
    disabled?: boolean;
    /** Accessible name. The visible <Static> beside the box is positioned,
     *  not associated, so the field needs to carry its own name. */
    label: string;
    onInput: (v: string) => void;
  },
) {
  return (
    <input
      type="text"
      aria-label={props.label}
      style={{ ...dlu(props.x, props.y, props.w ?? 40, props.h ?? 14) }}
      value={String(props.value)}
      disabled={props.disabled}
      onInput={(e) => props.onInput((e.target as HTMLInputElement).value)}
    />
  );
}

/** owner-drawn color button + 20-swatch picker (CColorBtn) */
export function ColorBtn(
  props: {
    color: number;
    x: number;
    y: number;
    w?: number;
    h?: number;
    onPick: (index: number) => void;
  },
) {
  const { local: open, set: setOpen } = useLocal(false);
  return (
    <div style={dlu(props.x, props.y, props.w ?? 50, props.h ?? 16)}>
      <button
        type="button"
        style={{
          width: "100%",
          height: "100%",
          minWidth: 0,
          minHeight: 0,
          padding: "2px",
          display: "flex",
          alignItems: "center",
          gap: "4px",
        }}
        onClick={() => setOpen(!open)}
      >
        <span
          style={{
            display: "inline-block",
            width: "14px",
            height: "10px",
            background: PALETTE[props.color],
            border: "1px solid #000",
          }}
        />
        Color
      </button>
      {open && (
        <div
          className="raised"
          style={{
            position: "absolute",
            top: "100%",
            left: 0,
            zIndex: 10,
            background: "var(--btnface)",
            display: "grid",
            gridTemplateColumns: "repeat(5, 18px)",
            gap: "2px",
            padding: "4px",
          }}
        >
          {PALETTE.map((c, i) => (
            <span
              key={i}
              style={{
                width: "16px",
                height: "14px",
                background: c,
                border: i === props.color
                  ? "2px solid #000"
                  : "1px solid #808080",
              }}
              onClick={() => {
                props.onPick(i);
                setOpen(false);
              }}
            />
          ))}
        </div>
      )}
    </div>
  );
}
