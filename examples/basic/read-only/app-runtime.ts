import { apps, steam, utils } from '@node-3d/steam-api';
import { initSteam, numberOption } from '../helpers.ts';

initSteam();

const currentAppId = utils.getAppId();
const appId = numberOption('app-id', currentAppId);

console.log('Steam running:', steam.isSteamRunning());
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
console.log('Connected universe:', utils.getConnectedUniverse());
console.log('IP country:', utils.getIpCountry());
console.log('Steam UI language:', utils.getSteamUiLanguage());
console.log('Server real time:', new Date(utils.getServerRealTime() * 1000).toISOString());
console.log('Overlay enabled:', utils.isOverlayEnabled());
console.log('Big Picture mode:', utils.isSteamInBigPictureMode());
console.log('Steam Deck:', utils.isSteamRunningOnSteamDeck());
