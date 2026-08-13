import { FriendFlags, friends, steamId, utils } from '@node-3d/steam-api';
import { initSteam, numberOption, option, pumpFor, steamIdFromOption } from '../helpers.ts';

initSteam();

console.log('Overlay enabled:', utils.isOverlayEnabled());

const friendIds = friends.getFriends(FriendFlags.Immediate);
console.log('Immediate friends:', friendIds.length);

for (const id of friendIds.slice(0, numberOption('limit', 10))) {
	const avatar = friends.getMediumFriendAvatar(id);
	const size = avatar > 0 ? utils.getImageSize(avatar) : null;
	const rgba = avatar > 0 ? utils.getImageRgba(avatar) : null;

	console.log({
		id,
		name: friends.getFriendPersonaName(id),
		relationship: steamId.getRelationship(id),
		level: steamId.getSteamLevel(id),
		avatar,
		avatarSize: size,
		avatarBytes: rgba?.byteLength ?? 0,
		game: friends.getFriendGamePlayed(id),
	});
}

const richPresence = option('rich-presence');
if (richPresence) {
	const [key, value] = richPresence.split('=', 2);
	if (!key || value === undefined) {
		throw new Error('--rich-presence must use key=value format.');
	}
	console.log('Set rich presence:', key, friends.setRichPresence(key, value));
}

const overlay = option('overlay');
if (overlay) {
	console.log('Opening overlay dialog:', overlay);
	friends.activateGameOverlay(overlay);
}

const web = option('web');
if (web) {
	console.log('Opening overlay web page:', web);
	friends.activateGameOverlayToWebPage(web);
}

const store = option('store');
if (store) {
	console.log('Opening overlay store page:', store);
	friends.activateGameOverlayToStore(Number(store), 0);
}

if (option('user')) {
	const target = steamIdFromOption('user');
	const dialog = option('user-dialog') ?? 'steamid';
	console.log('Opening overlay to user:', dialog, target);
	friends.activateGameOverlayToUser(dialog, target);
}

await pumpFor(numberOption('seconds', 5) * 1000);

if (richPresence) {
	friends.clearRichPresence();
	console.log('Cleared rich presence.');
}
