import { existsSync } from 'node:fs';
import { resolve } from 'node:path';
import { apps, steam, user, userStats, utils } from '@node-3d/steam-api';
import { waitForSteamCall } from '../basic/helpers.ts';

const appIdPath = resolve(process.cwd(), 'steam_appid.txt');

if (!existsSync(appIdPath)) {
	throw new Error('Run this example from examples/spacewar so Steam can find steam_appid.txt.');
}

const result = steam.initEx();
if (!result.ok) {
	throw new Error(`SteamAPI_InitEx failed with ${result.result}: ${result.errorMessage}`);
}

const appId = utils.getAppId();

console.log('Spacewar app id:', appId);
console.log('Steam running:', steam.isSteamRunning());
console.log('Logged on:', user.isLoggedOn());
console.log('Steam ID:', user.getSteamId());
console.log('App build id:', apps.getAppBuildId());
console.log('Current game language:', apps.getCurrentGameLanguage());
console.log('Current game install dir:', apps.getCurrentGameInstallDir());
console.log('Overlay enabled:', utils.isOverlayEnabled());
console.log('Achievement names:', userStats.getAchievementNames());
console.log('Current players:', await waitForSteamCall(userStats.getNumberOfCurrentPlayers()));
