import fs from 'node:fs';
import path from 'node:path';

// oxlint-disable node/no-sync

type TPlatformRedist = Readonly<{
	dir: string;
	files: readonly string[];
}>;

export const PACKAGE_ROOT = path.resolve(import.meta.dirname, '..');
export const SDK_CACHE_DIR = path.join(PACKAGE_ROOT, '.sdk');
export const SDK_DIR = path.join(SDK_CACHE_DIR, 'sdk');
export const SDK_ARCHIVE_PATH = path.join(SDK_CACHE_DIR, 'steamworks-sdk.gz');

export const SDK_PUBLIC_DIR = path.join(SDK_DIR, 'public');
export const SDK_STEAM_INCLUDE_DIR = path.join(SDK_PUBLIC_DIR, 'steam');
export const SDK_REDIST_DIR = path.join(SDK_DIR, 'redistributable_bin');

export const toForwardSlash = (value: string): string => value.replaceAll(path.sep, '/');

export const sdkExists = (): boolean =>
	fs.existsSync(path.join(SDK_STEAM_INCLUDE_DIR, 'steam_api.h')) && fs.existsSync(SDK_REDIST_DIR);

export const getPlatformRedist = (): TPlatformRedist => {
	if (process.platform === 'win32') {
		return {
			dir: path.join(SDK_REDIST_DIR, 'win64'),
			files: ['steam_api64.dll'],
		};
	}

	if (process.platform === 'linux') {
		return {
			dir: path.join(SDK_REDIST_DIR, process.arch === 'arm64' ? 'linuxarm64' : 'linux64'),
			files: ['libsteam_api.so'],
		};
	}

	if (process.platform === 'darwin') {
		return {
			dir: path.join(SDK_REDIST_DIR, 'osx'),
			files: ['libsteam_api.dylib'],
		};
	}

	throw new Error(`Unsupported platform: ${process.platform}-${process.arch}`);
};
