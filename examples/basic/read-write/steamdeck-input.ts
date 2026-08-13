import { FloatingGamepadTextInputMode, utils } from '@node-3d/steam-api';
import { hasFlag, initSteam, numberOption, option, pumpFor } from '../helpers.ts';

initSteam();

console.log('Steam UI language:', utils.getSteamUiLanguage());
console.log('IP country:', utils.getIpCountry());
console.log('Connected universe:', utils.getConnectedUniverse());
console.log('Server real time:', new Date(utils.getServerRealTime() * 1000).toISOString());
console.log('Overlay enabled:', utils.isOverlayEnabled());
console.log('Big Picture mode:', utils.isSteamInBigPictureMode());
console.log('Steam Deck:', utils.isSteamRunningOnSteamDeck());

if (hasFlag('show-input')) {
	const modeName = option('mode') ?? 'SingleLine';
	const mode =
		FloatingGamepadTextInputMode[modeName as keyof typeof FloatingGamepadTextInputMode];
	if (mode === undefined) {
		throw new Error(`Unknown floating gamepad text input mode: ${modeName}`);
	}

	console.log(
		'Show floating gamepad text input:',
		utils.showFloatingGamepadTextInput(
			mode,
			numberOption('x', 100),
			numberOption('y', 100),
			numberOption('width', 640),
			numberOption('height', 80),
		),
	);

	await pumpFor(numberOption('seconds', 10) * 1000);

	if (hasFlag('dismiss')) {
		console.log(
			'Dismiss floating gamepad text input:',
			utils.dismissFloatingGamepadTextInput(),
		);
	}
}
