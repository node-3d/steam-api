import { FriendFlags, friends, steamId, utils } from '@node-3d/steam-api';
import { initSteam, numberOption } from '../helpers.ts';

initSteam();

const limit = numberOption('limit', 25);
const friendIds = friends.getFriends(FriendFlags.Immediate);

console.log('Friend count:', friends.getFriendCount(FriendFlags.Immediate));
console.log('Returned friends:', friendIds.length);

for (const id of friendIds.slice(0, limit)) {
	const smallAvatar = friends.getSmallFriendAvatar(id);
	const mediumAvatar = friends.getMediumFriendAvatar(id);
	const largeAvatar = friends.getLargeFriendAvatar(id);

	console.log({
		id,
		name: friends.getFriendPersonaName(id),
		accountId: steamId.getAccountId(id),
		relationship: steamId.getRelationship(id),
		level: steamId.getSteamLevel(id),
		game: friends.getFriendGamePlayed(id),
		smallAvatar: smallAvatar > 0 ? utils.getImageSize(smallAvatar) : null,
		mediumAvatar: mediumAvatar > 0 ? utils.getImageSize(mediumAvatar) : null,
		largeAvatar: largeAvatar > 0 ? utils.getImageSize(largeAvatar) : null,
	});
}
