// Browser file open/save helpers (the web equivalent of CFileDialog)

export function pickFile(
  accept: string,
): Promise<{ name: string; bytes: Uint8Array } | null> {
  return new Promise((resolve) => {
    const input = document.createElement("input");
    input.type = "file";
    input.accept = accept;
    input.onchange = async () => {
      const file = input.files?.[0];
      if (!file) return resolve(null);
      const buf = await file.arrayBuffer();
      resolve({ name: file.name, bytes: new Uint8Array(buf) });
    };
    // cancel: no reliable event — resolve(null) on window focus after dialog
    input.addEventListener("cancel", () => resolve(null));
    input.click();
  });
}

export function downloadFile(bytes: Uint8Array, name: string): void {
  const blob = new Blob([bytes.slice() as unknown as BlobPart], {
    type: "application/octet-stream",
  });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = name;
  a.click();
  setTimeout(() => URL.revokeObjectURL(a.href), 5000);
}
