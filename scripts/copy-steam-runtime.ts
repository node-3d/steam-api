import fs from 'node:fs/promises';
import path from 'node:path';
import { getBin } from '@node-3d/addon-tools';
import { getPlatformRedist, PACKAGE_ROOT } from './paths.ts';

const copySteamRuntime = async (): Promise<void> => {
	const { dir, files } = getPlatformRedist();
	const binDir = path.join(PACKAGE_ROOT, getBin());

	await fs.mkdir(binDir, { recursive: true });

	await Promise.all(
		files.map((file) => fs.copyFile(path.join(dir, file), path.join(binDir, file))),
	);
};

try {
	await copySteamRuntime();
} catch (error) {
	// oxlint-disable-next-line no-console
	console.error(error);
	process.exitCode = 1;
}
