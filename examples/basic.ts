import { getAppId, getSteamId, initEx, isLoggedOn, update } from '@node-3d/steam-api';

const result = initEx();

if (!result.ok) {
	throw new Error(`Steamworks init failed: ${result.errorMessage}`);
}

console.log('app', getAppId());
console.log('user', getSteamId(), isLoggedOn() ? 'logged on' : 'offline');

for (const event of update()) {
	console.log(event);
}
