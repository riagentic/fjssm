// Status-bar message signal — MFC flyby help (long help string on hover)
import { signal } from "aio/air";

export const IDLE_MESSAGE = "Pro nápovědu stiskni  F1";

export const statusText = signal(IDLE_MESSAGE);

export const setStatus = (text: string): void => {
  statusText.set(text || IDLE_MESSAGE);
};
export const clearStatus = (): void => {
  statusText.set(IDLE_MESSAGE);
};
