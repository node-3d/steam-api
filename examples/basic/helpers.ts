import { setTimeout as delay } from 'node:timers/promises';
import { steam, update } from '@node-3d/steam-api';
import type {
	TSteamCallbackEvent,
	TSteamId,
	TSteamLobbyId,
	TSteamPublishedFileId,
} from '@node-3d/steam-api';

type TWaitOptions = Readonly<{
	intervalMs?: number;
	timeoutMs?: number;
	onEvent?: (event: TSteamCallbackEvent) => void;
	onTick?: () => void;
}>;

export const hasFlag = (name: string): boolean => process.argv.includes(`--${name}`);

export const option = (name: string): string | undefined => {
	const prefix = `--${name}=`;
	const inline = process.argv.find((arg) => arg.startsWith(prefix));
	if (inline) {
		return inline.slice(prefix.length);
	}

	const index = process.argv.indexOf(`--${name}`);
	return index === -1 ? undefined : process.argv[index + 1];
};

export const numberOption = (name: string, fallback: number): number => {
	const value = option(name);
	if (value === undefined) {
		return fallback;
	}

	const parsed = Number(value);
	if (!Number.isFinite(parsed)) {
		throw new TypeError(`--${name} must be a number.`);
	}

	return parsed;
};

export const listOption = (name: string): string[] =>
	(option(name) ?? '')
		.split(',')
		.map((value) => value.trim())
		.filter(Boolean);

export const requiredOption = (name: string): string => {
	const value = option(name);
	if (!value) {
		throw new Error(`Missing required --${name}=... option.`);
	}

	return value;
};

export const initSteam = (): void => {
	const result = steam.initEx();
	if (!result.ok) {
		throw new Error(`SteamAPI_InitEx failed with ${result.result}: ${result.errorMessage}`);
	}
};

export const logEvent = (event: TSteamCallbackEvent): void => {
	console.log('event', event);
};

// oxlint-disable promise/prefer-await-to-then, no-unmodified-loop-condition, no-await-in-loop
export const waitForSteamCall = async <T>(
	promise: Promise<T>,
	options: TWaitOptions = {},
): Promise<T> => {
	let settled = false;
	const tracked = promise.finally(() => {
		settled = true;
	});
	const intervalMs = options.intervalMs ?? 16;
	const timeoutAt = Date.now() + (options.timeoutMs ?? 60_000);

	while (!settled) {
		for (const event of update()) {
			options.onEvent?.(event);
		}
		options.onTick?.();

		if (Date.now() > timeoutAt) {
			throw new Error('Timed out waiting for Steam call result.');
		}

		await delay(intervalMs);
	}

	return tracked;
};
// oxlint-enable promise/prefer-await-to-then, no-unmodified-loop-condition, no-await-in-loop

export const pumpFor = async (
	durationMs: number,
	onEvent: (event: TSteamCallbackEvent) => void = logEvent,
): Promise<void> => {
	const deadline = Date.now() + durationMs;
	while (Date.now() < deadline) {
		for (const event of update()) {
			onEvent(event);
		}
		// oxlint-disable-next-line no-await-in-loop
		await delay(16);
	}
};

export const steamIdFromOption = (name: string): TSteamId => requiredOption(name) as TSteamId;

export const lobbyIdFromOption = (name: string): TSteamLobbyId =>
	requiredOption(name) as TSteamLobbyId;

export const publishedFileIdFromOption = (name: string): TSteamPublishedFileId =>
	requiredOption(name) as TSteamPublishedFileId;
