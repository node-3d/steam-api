import { Buffer } from 'node:buffer';
import { LobbyType, matchmaking, user } from '@node-3d/steam-api';
import {
	hasFlag,
	initSteam,
	lobbyIdFromOption,
	numberOption,
	option,
	pumpFor,
	waitForSteamCall,
} from '../helpers.ts';

initSteam();

let lobbyId = option('lobby') ? lobbyIdFromOption('lobby') : null;

if (hasFlag('list')) {
	const result = await waitForSteamCall(matchmaking.requestLobbyList());
	console.log('Lobby list result:', result);
	for (let index = 0; index < result.lobbiesMatching; index++) {
		console.log(index, matchmaking.getLobbyByIndex(index));
	}
}

if (hasFlag('create')) {
	const maxMembers = numberOption('max-members', 4);
	const created = await waitForSteamCall(matchmaking.createLobby(LobbyType.Private, maxMembers));
	lobbyId = created.lobbyId;
	console.log('Created lobby:', created);
}

if (hasFlag('join')) {
	lobbyId = lobbyIdFromOption('join');
	console.log('Joined lobby:', await waitForSteamCall(matchmaking.joinLobby(lobbyId)));
}

if (lobbyId) {
	console.log('Lobby owner:', matchmaking.getLobbyOwner(lobbyId));
	console.log('Lobby members:', matchmaking.getNumLobbyMembers(lobbyId));
	console.log('Lobby member limit:', matchmaking.getLobbyMemberLimit(lobbyId));

	const data = option('data');
	if (data) {
		const [key, value] = data.split('=', 2);
		if (!key || value === undefined) {
			throw new Error('--data must use key=value format.');
		}
		console.log('Set lobby data:', matchmaking.setLobbyData(lobbyId, key, value));
	}

	console.log('Lobby data count:', matchmaking.getLobbyDataCount(lobbyId));
	for (let index = 0; index < matchmaking.getLobbyDataCount(lobbyId); index++) {
		console.log('Lobby data:', matchmaking.getLobbyDataByIndex(lobbyId, index));
	}

	const localUser = user.getSteamId();
	matchmaking.setLobbyMemberData(lobbyId, 'node3d-example-user', localUser);
	console.log(
		'Local member data:',
		matchmaking.getLobbyMemberData(lobbyId, localUser, 'node3d-example-user'),
	);

	const chat = option('chat');
	if (chat) {
		console.log('Send lobby chat:', matchmaking.sendLobbyChatMsg(lobbyId, Buffer.from(chat)));
	}

	await pumpFor(numberOption('seconds', 10) * 1000);

	if (!hasFlag('keep-open')) {
		matchmaking.leaveLobby(lobbyId);
		console.log('Left lobby:', lobbyId);
	}
}
