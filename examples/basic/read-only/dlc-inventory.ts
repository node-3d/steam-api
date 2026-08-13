import { apps } from '@node-3d/steam-api';
import { initSteam } from '../helpers.ts';

initSteam();

const dlcCount = apps.getDlcCount();
console.log('DLC count:', dlcCount);

for (let index = 0; index < dlcCount; index++) {
	const dlc = apps.getDlcDataByIndex(index);
	console.log('DLC:', index, dlc, dlc ? apps.isDlcInstalled(dlc.appId) : null);
}
