import { cloud } from '@node-3d/steam-api';
import { initSteam } from '../helpers.ts';

initSteam();

console.log('Cloud enabled:', cloud.isEnabled());
console.log('Cloud enabled for user:', cloud.isEnabledForUser());
console.log('Cloud quota:', cloud.getQuota() ?? '<unavailable>');

const fileCount = cloud.getFileCount();
console.log('Cloud files:', fileCount);

for (let index = 0; index < fileCount; index++) {
	console.log(index, cloud.getFileNameAndSize(index));
}
