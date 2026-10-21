// Translation hook for the pure report/format code. Keys are the original
// Czech strings with `{0}`… placeholders; the UI passes its `t`, and the
// default `cs` keeps the original text, so lib output stays byte-faithful.
export type Tr = (key: string, ...args: (string | number)[]) => string;

export const fill = (s: string, args: (string | number)[]): string =>
  s.replace(
    /\{(\d+)\}/g,
    (m, i: string) => (Number(i) < args.length ? String(args[Number(i)]) : m),
  );

export const cs: Tr = (key, ...args) => fill(key, args);
