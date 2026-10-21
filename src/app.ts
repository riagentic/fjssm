// FJSS Maker — entry point (boot only)
import { aio } from "aio";
import { doc } from "./cell/doc.ts";

await aio.run({
  appId: "fjssm",
  cells: [doc],
  baseDir: import.meta.dirname!,
  ui: { title: "FJSS Maker", width: 1024, height: 768 },
  // every asset is local; a `default-src 'self'` policy costs nothing here and
  // silences Electron's "no Content-Security-Policy" renderer warning
  security: { csp: "strict" },
  // checkpoint snapshots are never applied here (doc cell is persist:"none",
  // no onCheckpointRestore) — skip them to avoid stale-snapshot warnings
  diagnostics: { dev: { checkpoint: false } },
});
