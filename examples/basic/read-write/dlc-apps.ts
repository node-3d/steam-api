import { apps, utils } from '@node-3d/steam-api';
import { hasFlag, initSteam, numberOption, option } from '../helpers.ts';

initSteam();

const currentAppId = utils.getAppId();
const appId = numberOption('app-id', currentAppId);

console.log('Current app id:', currentAppId);
console.log('App build id:', apps.getAppBuildId());
console.log('Subscribed:', apps.isSubscribed());
console.log('Subscribed app:', appId, apps.isSubscribedApp(appId));
console.log('App installed:', appId, apps.isAppInstalled(appId));
console.log('Current game language:', apps.getCurrentGameLanguage());
console.log('Available game languages:', apps.getAvailableGameLanguages());
console.log('Current game install dir:', apps.getCurrentGameInstallDir());
console.log('App install dir:', appId, apps.getAppInstallDir(appId));
console.log('Launch command line:', apps.getLaunchCommandLine());

const dlcCount = apps.getDlcCount();
console.log('DLC count:', dlcCount);
for (let index = 0; index < dlcCount; index++) {
	const dlc = apps.getDlcDataByIndex(index);
	console.log('DLC:', index, dlc, dlc ? apps.isDlcInstalled(dlc.appId) : null);
}

const dlc = option('dlc');
if (dlc) {
	const dlcAppId = Number(dlc);
	console.log('DLC installed:', dlcAppId, apps.isDlcInstalled(dlcAppId));
	if (hasFlag('install-dlc')) {
		apps.installDlc(dlcAppId);
		console.log('Install DLC requested:', dlcAppId);
	}
	if (hasFlag('uninstall-dlc')) {
		apps.uninstallDlc(dlcAppId);
		console.log('Uninstall DLC requested:', dlcAppId);
	}
}
