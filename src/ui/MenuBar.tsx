// Menu bar — BCMenu-style bitmapped menus (IDR_MAINFRAME)
import { useLocal } from "aio/air";
import { doc } from "../cell/doc.ts";
import { CRITERIA_SUBMENU, MenuItem, MENUS } from "./menu-def.ts";
import { commands, isEnabled, runCommand } from "./commands.ts";
import { clearStatus, setStatus } from "./status.ts";
import { Sprite } from "./Sprite.tsx";

/** render "&Nový" with the mnemonic underlined */
function Label(props: { text: string }) {
  const i = props.text.indexOf("&");
  if (i === -1) return <>{props.text}</>;
  return (
    <>
      {props.text.slice(0, i)}
      <u>{props.text[i + 1]}</u>
      {props.text.slice(i + 2)}
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

  const enabled = item.grayed
    ? false
    : item.command === "__submenu_criteria"
    ? true
    : item.command
    ? isEnabled(item.command)
    : false;

  const radioChecked = item.radio === "sa"
    ? selectedResult === 0
    : item.radio === "ga"
    ? selectedResult === 1
    : false;

  const { local: subOpen, set: setSubOpen } = useLocal(false);

  if (item.command === "__submenu_criteria") {
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
            {CRITERIA_SUBMENU.map((sub, i) =>
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
