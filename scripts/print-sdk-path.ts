import { SDK_PUBLIC_DIR, SDK_REDIST_DIR, toForwardSlash } from './paths.ts';

const kind = process.argv[2];

if (kind === 'include') {
	// oxlint-disable-next-line no-console
	console.log(toForwardSlash(SDK_PUBLIC_DIR));
} else if (kind === 'redist') {
	// oxlint-disable-next-line no-console
	console.log(toForwardSlash(SDK_REDIST_DIR));
} else {
	// oxlint-disable-next-line no-console
	console.error('Usage: node scripts/print-sdk-path.ts <include|redist>');
	process.exitCode = 1;
}
