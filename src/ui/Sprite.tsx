// 16×16 glyph from the original toolbar bitmap strips
import { MenuIcon } from "./menu-def.ts";

export function Sprite(props: { icon: MenuIcon; disabled?: boolean }) {
  const [strip, index] = props.icon;
  const url = strip === 1 ? "/assets/toolbar.png" : "/assets/toolbar2.png";
  return (
    <span
      className="tb-ico"
      style={{
        display: "inline-block",
        backgroundImage: `url(${url})`,
        backgroundPosition: `${-16 * index}px 0`,
        width: "16px",
        height: "16px",
        imageRendering: "pixelated",
        filter: props.disabled ? "grayscale(1) opacity(0.4)" : undefined,
      }}
    />
  );
}
