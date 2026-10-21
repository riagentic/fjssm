// UI language — English by default, Czech (the original) on request.
// Per-viewer preference: kept in localStorage, guarded, never required.
import { signal } from "aio/air";
import { fill, Tr } from "../lib/tr.ts";
import { EN } from "./i18n-en.ts";

export type Lang = "en" | "cs";

const KEY = "fjssm.lang";

const stored = ((): string | null => {
  try {
    return globalThis.localStorage?.getItem(KEY) ?? null;
  } catch {
    return null;
  }
})();

export const lang = signal<Lang>(stored === "cs" ? "cs" : "en");

const syncHtmlLang = (l: Lang): void => {
  if (typeof document !== "undefined") document.documentElement.lang = l;
};
syncHtmlLang(lang.peek());

export function setLang(l: Lang): void {
  lang.set(l);
  syncHtmlLang(l);
  try {
    globalThis.localStorage?.setItem(KEY, l);
  } catch {
    // storage blocked: the choice still holds for this session
  }
}

/** Translate a Czech source string; missing keys fall back to the Czech. */
export const t: Tr = (key, ...args) =>
  fill(lang.value === "cs" ? key : EN[key] ?? key, args);
