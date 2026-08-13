import { FriendFlags, cloud, friends, userStats } from '@node-3d/steam-api';
import { hasFlag, initSteam, logEvent, waitForSteamCall } from '../helpers.ts';

initSteam();

console.log('Steam API initialized successfully.');
console.log('Cloud enabled:', cloud.isEnabled());
console.log('Cloud enabled for user:', cloud.isEnabledForUser());

const sampleFile = 'node3d_greenworks_sample.txt';
if (cloud.saveTextToFile(sampleFile, 'test_content')) {
	console.log('Saved text to Steam Cloud:', sampleFile);
} else {
	console.log('Failed to save text to Steam Cloud:', sampleFile);
}

const content = cloud.readTextFromFile(sampleFile);
console.log('Read text from Steam Cloud:', content ?? '<missing>');

const quota = cloud.getQuota();
console.log('Cloud quota:', quota ?? '<unavailable>');

if (hasFlag('write-achievement')) {
	console.log('Setting ACH_WIN_ONE_GAME:', userStats.setAchievement('ACH_WIN_ONE_GAME'));
	console.log('Storing stats:', userStats.storeStats());
} else {
	console.log(
		'Skipping achievement write. Pass --write-achievement to mirror Greenworks sample.',
	);
}

const players = await waitForSteamCall(userStats.getNumberOfCurrentPlayers(), {
	onEvent: logEvent,
});
console.log('Number of current players:', players);

const friendIds = friends.getFriends(FriendFlags.Immediate);
console.log('Number of friends:', friendIds.length);
console.log(
	'Friends:',
	friendIds.map((id) => friends.getFriendPersonaName(id)),
);
