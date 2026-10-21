// Edit → Kopírovat routing (OnEditCopy, FJSSMView.cpp:669-676): the active
// view registers its copy handler; the command invokes it.
let handler: (() => void) | null = null;

export function registerCopy(fn: (() => void) | null): void {
  handler = fn;
}

export function runCopy(): void {
  handler?.();
}
