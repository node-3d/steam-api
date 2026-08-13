import { matchmaking } from '@node-3d/steam-api';
import { initSteam, waitForSteamCall } from '../helpers.ts';

initSteam();

const result = await waitForSteamCall(matchmaking.requestLobbyList());

console.log('Lobbies matching:', result.lobbiesMatching);
for (let index = 0; index < result.lobbiesMatching; index++) {
	const lobbyId = matchmaking.getLobbyByIndex(index);
	if (!lobbyId) {
		console.log(index, '<invalid lobby>');
		continue;
	}

	console.log({
		index,
		lobbyId,
		owner: matchmaking.getLobbyOwner(lobbyId),
		members: matchmaking.getNumLobbyMembers(lobbyId),
		memberLimit: matchmaking.getLobbyMemberLimit(lobbyId),
		dataCount: matchmaking.getLobbyDataCount(lobbyId),
	});
}
