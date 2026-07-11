import crypto from 'node:crypto';
import fs from 'node:fs/promises';
import { execFile as execFileCallback } from 'node:child_process';
import { promisify } from 'node:util';
import { SDK_ARCHIVE_PATH, SDK_CACHE_DIR, SDK_DIR, sdkExists } from './paths.ts';

const execFile = promisify(execFileCallback);

// oxlint-disable-next-line node/no-process-env
const env = process.env;
const getEnv = (name: string): string => env[name]?.trim() || '';

const pathExists = async (filePath: string): Promise<boolean> => {
	try {
		await fs.access(filePath);
		return true;
	} catch {
		return false;
	}
};

const assertSdk = (): void => {
	if (!sdkExists()) {
		throw new Error(
			[
				'Steamworks SDK is missing or incomplete.',
				`Expected an unpacked SDK at ${SDK_DIR}`,
				`Provide ${SDK_ARCHIVE_PATH} or STEAM_SDK_GZ_LINK.`,
			].join('\n'),
		);
	}
};

const normalizeDownloadLink = (value: string): string => {
	const trimmed = value.trim();
	if (!trimmed) {
		return '';
	}

	let url: URL | null = null;
	try {
		url = new URL(trimmed);
	} catch {
		return trimmed;
	}

	if (!url.hostname.endsWith('drive.google.com')) {
		return trimmed;
	}

	const filePathMatch = /\/file\/d\/([^/?]+)/u.exec(trimmed);
	if (filePathMatch) {
		return `https://drive.google.com/uc?export=download&confirm=t&id=${filePathMatch[1]}`;
	}

	const idQueryMatch = /[?&]id=([^&]+)/u.exec(trimmed);
	if (idQueryMatch) {
		return `https://drive.google.com/uc?export=download&confirm=t&id=${idQueryMatch[1]}`;
	}

	return trimmed;
};

const getDownloadUrl = (): string => normalizeDownloadLink(getEnv('STEAM_SDK_GZ_LINK'));

const getExpectedHash = (): string => getEnv('STEAM_SDK_GZ_SHA');

const verifyHash = async (filePath: string, isRequired: boolean): Promise<void> => {
	const expected = getExpectedHash();
	if (!expected) {
		if (isRequired) {
			throw new Error(
				'STEAM_SDK_GZ_SHA must be set when downloading the Steamworks SDK archive.',
			);
		}
		return;
	}

	const hash = crypto.createHash('sha256');
	const data = await fs.readFile(filePath);
	hash.update(data);

	const actual = hash.digest('hex');
	if (actual.toLowerCase() !== expected.toLowerCase()) {
		throw new Error('Steamworks SDK archive SHA-256 mismatch.');
	}
};

const unpackArchive = async (archivePath: string): Promise<void> => {
	await fs.rm(SDK_DIR, { recursive: true, force: true });
	await fs.mkdir(SDK_DIR, { recursive: true });
	await execFile('tar', ['-xzf', archivePath, '--directory', SDK_DIR]);
	assertSdk();
};

const downloadArchive = async (url: string): Promise<void> => {
	await fs.mkdir(SDK_CACHE_DIR, { recursive: true });

	const response = await fetch(url);
	if (!response.ok) {
		throw new Error(`Could not download Steamworks SDK archive: HTTP ${response.status}`);
	}

	const bytes = Buffer.from(await response.arrayBuffer());
	const contentType = response.headers.get('content-type') || '';
	const prefix = bytes.subarray(0, 64).toString('utf8').trimStart().toLowerCase();

	if (
		contentType.includes('text/html') ||
		prefix.startsWith('<!doctype html') ||
		prefix.startsWith('<html')
	) {
		throw new Error(
			[
				'Steamworks SDK download returned HTML instead of a gzip archive.',
				'Use a link that returns the archive bytes without an interactive browser page.',
				'If the file is private, use an authenticated direct download URL in STEAM_SDK_GZ_LINK instead.',
			].join('\n'),
		);
	}

	await fs.writeFile(SDK_ARCHIVE_PATH, bytes);
	await verifyHash(SDK_ARCHIVE_PATH, true);
	await unpackArchive(SDK_ARCHIVE_PATH);
};

const ensureSdk = async (): Promise<void> => {
	if (sdkExists()) {
		return;
	}

	if (await pathExists(SDK_ARCHIVE_PATH)) {
		await verifyHash(SDK_ARCHIVE_PATH, false);
		await unpackArchive(SDK_ARCHIVE_PATH);
		return;
	}

	const downloadUrl = getDownloadUrl();
	if (downloadUrl) {
		await downloadArchive(downloadUrl);
		return;
	}

	assertSdk();
};

try {
	await ensureSdk();
} catch (error) {
	// oxlint-disable-next-line no-console
	console.error(error);
	process.exitCode = 1;
}
