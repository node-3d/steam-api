import { userStats } from '@node-3d/steam-api';
import { initSteam, option, waitForSteamCall } from '../helpers.ts';

initSteam();

const achievementNames = userStats.getAchievementNames();
console.log('Achievements:', achievementNames.length);

for (const name of achievementNames) {
	if (!name) {
		continue;
	}

	console.log({
		name,
		achieved: userStats.getAchievement(name),
		displayName: userStats.getAchievementDisplayAttribute(name, 'name'),
		description: userStats.getAchievementDisplayAttribute(name, 'desc'),
		unlockTime: userStats.getAchievementAndUnlockTime(name),
	});
}

const statInt = option('stat-int');
if (statInt) {
	console.log('Stat int:', statInt, userStats.getStatInt(statInt));
}

const statFloat = option('stat-float');
if (statFloat) {
	console.log('Stat float:', statFloat, userStats.getStatFloat(statFloat));
}

console.log(
	'Number of current players:',
	await waitForSteamCall(userStats.getNumberOfCurrentPlayers()),
);
