// Menu bar — BCMenu-style bitmapped menus (IDR_MAINFRAME)
import { useLocal } from "aio/air";
import { doc } from "../cell/doc.ts";
import { MenuItem, MENUS } from "./menu-def.ts";
import { commands, isEnabled, runCommand } from "./commands.ts";
import { clearStatus, setStatus } from "./status.ts";
import { Sprite } from "./Sprite.tsx";
import { lang, t } from "./i18n.ts";

/** render "&Nový" (translated) with the mnemonic underlined */
function Label(props: { text: string }) {
  const text = t(props.text);
  const i = text.indexOf("&");
  if (i === -1) return <>{text}</>;
  return (
    <>
      {text.slice(0, i)}
      <u>{text[i + 1]}</u>
      {text.slice(i + 2)}
    </>
  );
}

function PopupItem(
  props: { item: MenuItem; onRun: () => void },
) {
  const { item } = props;
  // Read on EVERY path, so no instance of this component subscribes to nothing
  // (a radio-less item used to read no signal at all and could never update).
  const selectedResult = doc.results.selectedResult;
  const current = lang.value;

  const enabled = item.grayed
    ? false
    : item.submenu
    ? true
    : item.command
    ? isEnabled(item.command)
    : false;

  const radioChecked = item.radio === "sa"
    ? selectedResult === 0
    : item.radio === "ga"
    ? selectedResult === 1
    : item.radio === current;

  const { local: subOpen, set: setSubOpen } = useLocal(false);

  if (item.submenu) {
    return (
      <div
        className="menu-item"
        role="menuitem"
        aria-haspopup="menu"
        aria-expanded={subOpen}
        tabIndex={0}
        onKeyDown={(e: KeyboardEvent) => {
          if (e.key === "Enter" || e.key === " ") {
            e.preventDefault();
            setSubOpen(!subOpen);
          }
        }}
        onMouseEnter={() => setSubOpen(true)}
        onMouseLeave={() => setSubOpen(false)}
      >
        <span className="mi-icon" />
        <span className="mi-label">
          <Label text={item.label!} />
        </span>
        <span>▸</span>
        {subOpen && (
          <div
            className="menu-popup"
            style={{ left: "100%", top: "-2px" }}
          >
            {item.submenu.map((sub, i) =>
              sub.separator
                ? <div key={i} className="menu-sep" />
                : <PopupItem key={i} item={sub} onRun={props.onRun} />
            )}
          </div>
        )}
      </div>
    );
  }

  // A greyed item carries NO handler at all: `aria-disabled` describes a
  // control, it does not disable it, and a <div> takes no `disabled`.
  const cmd = item.command;
  const run = enabled && cmd
    ? () => {
      props.onRun();
      runCommand(cmd);
    }
    : undefined;

  return (
    <div
      className={`menu-item${!enabled ? " disabled" : ""}`}
      role="menuitem"
      tabIndex={enabled ? 0 : -1}
      aria-disabled={!enabled}
      onKeyDown={run &&
        ((e: KeyboardEvent) => {
          if (e.key !== "Enter" && e.key !== " ") return;
          e.preventDefault();
          run();
        })}
      onMouseEnter={() =>
        cmd && commands[cmd] && setStatus(commands[cmd]!.help)}
      onMouseLeave={clearStatus}
      onClick={run}
    >
      <span className="mi-icon">
        {item.radio
          ? <span className="menu-check">{radioChecked ? "●" : ""}</span>
          : item.icon
          ? <Sprite icon={item.icon} disabled={!enabled} />
          : null}
      </span>
      <span className="mi-label">
        <Label text={item.label!} />
      </span>
      <span className="mi-accel">{item.accel ?? ""}</span>
    </div>
  );
}

export function MenuBar() {
  const { local: open, set: setOpen } = useLocal<number | null>(null);

  return (
    <div className="menubar">
      {MENUS.map((menu, i) => (
        <div
          key={menu.label}
          className={`menubar-item${open === i ? " open" : ""}`}
          style={{ position: "relative" }}
          role="button"
          tabIndex={0}
          aria-haspopup="menu"
          aria-expanded={open === i}
          onClick={() => setOpen(open === i ? null : i)}
          onKeyDown={(e: KeyboardEvent) => {
            if (e.key === "Enter" || e.key === " ") {
              e.preventDefault();
              setOpen(open === i ? null : i);
            } else if (e.key === "Escape") setOpen(null);
          }}
          onMouseEnter={() => {
            if (open !== null && open !== i) setOpen(i);
          }}
        >
          <Label text={menu.label} />
          {open === i && (
            <div className="menu-popup" style={{ left: 0 }}>
              {menu.items.map((item, j) =>
                item.separator
                  ? <div key={j} className="menu-sep" />
                  : (
                    <PopupItem
                      key={j}
                      item={item}
                      onRun={() => setOpen(null)}
                    />
                  )
              )}
            </div>
          )}
        </div>
      ))}
      <div
        key="scrim"
        role="presentation"
        style={{
          position: "fixed",
          inset: 0,
          zIndex: 90,
          display: open === null ? "none" : "block",
        }}
        onClick={() => setOpen(null)}
      />
    </div>
  );
}
