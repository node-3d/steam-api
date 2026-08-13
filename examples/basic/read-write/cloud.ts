import { cloud } from '@node-3d/steam-api';
import { hasFlag, initSteam, option } from '../helpers.ts';

initSteam();

const fileName = option('file') ?? 'node3d_cloud_example.txt';
const content = option('content') ?? `Steam Cloud example written at ${new Date().toISOString()}`;

console.log('Cloud enabled:', cloud.isEnabled());
console.log('Cloud enabled for user:', cloud.isEnabledForUser());
console.log('Cloud quota:', cloud.getQuota() ?? '<unavailable>');

console.log('Saving file:', fileName, cloud.saveTextToFile(fileName, content));
console.log('Reading file:', fileName, cloud.readTextFromFile(fileName) ?? '<missing>');

const fileCount = cloud.getFileCount();
console.log('Cloud files:', fileCount);
for (let index = 0; index < fileCount; index++) {
	console.log(index, cloud.getFileNameAndSize(index));
}

if (hasFlag('delete')) {
	console.log('Deleting file:', fileName, cloud.deleteFile(fileName));
}
