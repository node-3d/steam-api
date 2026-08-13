import { userStats } from '@node-3d/steam-api';
import { hasFlag, initSteam, numberOption, option, waitForSteamCall } from '../helpers.ts';

initSteam();

const achievement = option('achievement') ?? 'ACH_WIN_ONE_GAME';

console.log('Achievements:', userStats.getNumAchievements());
console.log('Achievement names:', userStats.getAchievementNames());
console.log('Achievement state:', achievement, userStats.getAchievement(achievement));
console.log(
	'Achievement display:',
	achievement,
	userStats.getAchievementDisplayAttribute(achievement, 'name'),
	userStats.getAchievementDisplayAttribute(achievement, 'desc'),
);
console.log('Achievement unlock time:', userStats.getAchievementAndUnlockTime(achievement));

if (hasFlag('write-achievement')) {
	console.log('Indicate progress:', userStats.indicateAchievementProgress(achievement, 1, 1));
	console.log('Set achievement:', userStats.setAchievement(achievement));
	console.log('Store stats:', userStats.storeStats());
}

const statInt = option('stat-int');
if (statInt) {
	console.log('Stat int before:', statInt, userStats.getStatInt(statInt));
	if (hasFlag('write-stats')) {
		console.log(
			'Set stat int:',
			statInt,
			userStats.setStatInt(statInt, numberOption('int-value', 1)),
		);
		console.log('Store stats:', userStats.storeStats());
	}
}

const statFloat = option('stat-float');
if (statFloat) {
	console.log('Stat float before:', statFloat, userStats.getStatFloat(statFloat));
	if (hasFlag('write-stats')) {
		console.log(
			'Set stat float:',
			statFloat,
			userStats.setStatFloat(statFloat, numberOption('float-value', 1)),
		);
		console.log('Store stats:', userStats.storeStats());
	}
}

if (hasFlag('reset')) {
	console.log('Reset all stats:', userStats.resetAllStats(hasFlag('reset-achievements')));
}

console.log(
	'Number of current players:',
	await waitForSteamCall(userStats.getNumberOfCurrentPlayers()),
);
