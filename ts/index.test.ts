import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync } from 'node:fs';
import { join } from 'node:path';
import { getBin } from '@node-3d/addon-tools';
import type { TSteamId } from './index.ts';

const nativeBinaryPath = join(import.meta.dirname, '..', getBin(), 'steam-api.node');
const nativeSkip = existsSync(nativeBinaryPath) ? false : 'native binary is not built';
const loadSteamApi = () => import('./index.ts');

test(
	'native addon contract is loadable when a prebuilt binary is present',
	{ skip: nativeSkip },
	async () => {
		const steamApi = await loadSteamApi();
		assert.equal(typeof steamApi.steam.initEx, 'function');
		assert.equal(typeof steamApi.callbacks.pollCallbacks, 'function');
		assert.equal(typeof steamApi.steamId.isAnonymous, 'function');
		assert.equal(typeof steamApi.steamId.isAnonymousGameServer, 'function');
		assert.equal(typeof steamApi.steamId.isAnonymousGameServerLogin, 'function');
		assert.equal(typeof steamApi.steamId.isAnonymousUser, 'function');
		assert.equal(typeof steamApi.steamId.isChatAccount, 'function');
		assert.equal(typeof steamApi.steamId.isClanAccount, 'function');
		assert.equal(typeof steamApi.steamId.isConsoleUserAccount, 'function');
		assert.equal(typeof steamApi.steamId.isContentServerAccount, 'function');
		assert.equal(typeof steamApi.steamId.isGameServerAccount, 'function');
		assert.equal(typeof steamApi.steamId.isIndividualAccount, 'function');
		assert.equal(typeof steamApi.steamId.isPersistentGameServerAccount, 'function');
		assert.equal(typeof steamApi.steamId.isLobby, 'function');
		assert.equal(typeof steamApi.steamId.getAccountId, 'function');
		assert.equal(typeof steamApi.steamId.getRawSteamId, 'function');
		assert.equal(typeof steamApi.steamId.getAccountType, 'function');
		assert.equal(typeof steamApi.steamId.isValid, 'function');
		assert.equal(typeof steamApi.steamId.getStaticAccountKey, 'function');
		assert.equal(typeof steamApi.steamId.getPersonaName, 'function');
		assert.equal(typeof steamApi.steamId.getNickname, 'function');
		assert.equal(typeof steamApi.steamId.getRelationship, 'function');
		assert.equal(typeof steamApi.steamId.getSteamLevel, 'function');
		assert.equal(typeof steamApi.friends.activateGameOverlay, 'function');
		assert.equal(typeof steamApi.friends.getFriendCount, 'function');
		assert.equal(typeof steamApi.friends.getFriends, 'function');
		assert.equal(typeof steamApi.friends.requestUserInformation, 'function');
		assert.equal(typeof steamApi.friends.getSmallFriendAvatar, 'function');
		assert.equal(typeof steamApi.friends.getMediumFriendAvatar, 'function');
		assert.equal(typeof steamApi.friends.getLargeFriendAvatar, 'function');
		assert.equal(typeof steamApi.friends.setListenForFriendsMessage, 'function');
		assert.equal(typeof steamApi.friends.replyToFriendMessage, 'function');
		assert.equal(typeof steamApi.friends.getFriendMessage, 'function');
		assert.equal(typeof steamApi.friends.getFriendPersonaName, 'function');
		assert.equal(typeof steamApi.friends.setRichPresence, 'function');
		assert.equal(typeof steamApi.friends.clearRichPresence, 'function');
		assert.equal(typeof steamApi.friends.getFriendRichPresence, 'function');
		assert.equal(typeof steamApi.friends.setPlayedWith, 'function');
		assert.equal(typeof steamApi.friends.getFriendGamePlayed, 'function');
		assert.equal(typeof steamApi.friends.activateGameOverlayInviteDialog, 'function');
		assert.equal(typeof steamApi.friends.activateGameOverlayToUser, 'function');
		assert.equal(typeof steamApi.matchmaking.createLobby, 'function');
		assert.equal(typeof steamApi.matchmaking.deleteLobbyData, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyByIndex, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyData, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyMemberByIndex, 'function');
		assert.equal(typeof steamApi.matchmaking.getNumLobbyMembers, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyOwner, 'function');
		assert.equal(typeof steamApi.matchmaking.inviteUserToLobby, 'function');
		assert.equal(typeof steamApi.matchmaking.joinLobby, 'function');
		assert.equal(typeof steamApi.matchmaking.leaveLobby, 'function');
		assert.equal(typeof steamApi.matchmaking.setLobbyData, 'function');
		assert.equal(typeof steamApi.matchmaking.setLobbyJoinable, 'function');
		assert.equal(typeof steamApi.matchmaking.setLobbyOwner, 'function');
		assert.equal(typeof steamApi.matchmaking.setLobbyType, 'function');
		assert.equal(typeof steamApi.matchmaking.requestLobbyList, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyMemberLimit, 'function');
		assert.equal(typeof steamApi.matchmaking.setLobbyMemberLimit, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyMemberData, 'function');
		assert.equal(typeof steamApi.matchmaking.setLobbyMemberData, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyDataCount, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyDataByIndex, 'function');
		assert.equal(typeof steamApi.matchmaking.sendLobbyChatMsg, 'function');
		assert.equal(typeof steamApi.matchmaking.getLobbyChatEntry, 'function');
		assert.equal(typeof steamApi.networking.sendP2PPacket, 'function');
		assert.equal(typeof steamApi.networking.isP2PPacketAvailable, 'function');
		assert.equal(typeof steamApi.networking.readP2PPacket, 'function');
		assert.equal(typeof steamApi.networking.acceptP2PSessionWithUser, 'function');
		assert.equal(typeof steamApi.networking.getP2PSessionState, 'function');
		assert.equal(typeof steamApi.networking.closeP2PSessionWithUser, 'function');
		assert.equal(typeof steamApi.networking.closeP2PChannelWithUser, 'function');
		assert.equal(typeof steamApi.networking.isBehindNat, 'function');
		assert.equal(typeof steamApi.utils.showFloatingGamepadTextInput, 'function');
		assert.equal(typeof steamApi.utils.dismissFloatingGamepadTextInput, 'function');
		assert.equal(typeof steamApi.cloud.saveTextToFile, 'function');
		assert.equal(typeof steamApi.cloud.readTextFromFile, 'function');
		assert.equal(typeof steamApi.cloud.deleteFile, 'function');
		assert.equal(typeof steamApi.cloud.saveFilesToCloud, 'function');
		assert.equal(typeof steamApi.cloud.isEnabledForUser, 'function');
		assert.equal(typeof steamApi.cloud.isEnabled, 'function');
		assert.equal(typeof steamApi.cloud.setEnabled, 'function');
		assert.equal(typeof steamApi.cloud.getQuota, 'function');
		assert.equal(typeof steamApi.cloud.getFileCount, 'function');
		assert.equal(typeof steamApi.cloud.getFileNameAndSize, 'function');
		assert.equal(typeof steamApi.ugc.getItemState, 'function');
		assert.equal(typeof steamApi.ugc.getItems, 'function');
		assert.equal(typeof steamApi.ugc.downloadItem, 'function');
		assert.equal(typeof steamApi.ugc.unsubscribe, 'function');
		assert.equal(typeof steamApi.ugc.saveFilesToCloud, 'function');
		assert.equal(typeof steamApi.ugc.fileShare, 'function');
		assert.equal(typeof steamApi.ugc.publishWorkshopFile, 'function');
		assert.equal(typeof steamApi.ugc.updatePublishedWorkshopFile, 'function');
		assert.equal(typeof steamApi.ugc.publish, 'function');
		assert.equal(typeof steamApi.ugc.publishUpdate, 'function');
		assert.equal(typeof steamApi.ugc.synchronizeItems, 'function');
		assert.equal(Object.isFrozen(steamApi.FriendFlags), true);
		assert.equal(typeof steamApi.LobbyType.Public, 'number');
		assert.equal(typeof steamApi.AuthSessionResponse.OK, 'number');
		assert.equal(typeof steamApi.BeginAuthSessionResult.OK, 'number');
		assert.equal(typeof steamApi.EncryptedAppTicketSymmetricKeyLength, 'number');
		assert.equal(typeof steamApi.user.getAuthSessionTicket, 'function');
		assert.equal(typeof steamApi.user.getAuthTicketForWebApi, 'function');
		assert.equal(typeof steamApi.user.cancelAuthTicket, 'function');
		assert.equal(typeof steamApi.user.beginAuthSession, 'function');
		assert.equal(typeof steamApi.user.endAuthSession, 'function');
		assert.equal(typeof steamApi.user.requestEncryptedAppTicket, 'function');
		assert.equal(typeof steamApi.user.getEncryptedAppTicket, 'function');
		assert.equal(typeof steamApi.user.decryptAppTicket, 'function');
		assert.equal(typeof steamApi.user.isTicketForApp, 'function');
		assert.equal(typeof steamApi.user.getTicketIssueTime, 'function');
		assert.equal(typeof steamApi.user.getTicketSteamId, 'function');
		assert.equal(typeof steamApi.user.getTicketAppId, 'function');
		assert.equal(typeof steamApi.userStats.getNumberOfCurrentPlayers, 'function');
	},
);

test('Steam lifecycle methods are safe without a Steam session', { skip: nativeSkip }, async () => {
	const steamApi = await loadSteamApi();

	assert.equal(typeof steamApi.steam.isSteamRunning(), 'boolean');
	assert.deepEqual(steamApi.callbacks.pollCallbacks(), []);
	assert.doesNotThrow(() => steamApi.steam.releaseCurrentThreadMemory());

	const result = steamApi.steam.initEx();
	try {
		assert.equal(typeof result.result, 'number');
		assert.equal(typeof result.ok, 'boolean');
		assert.equal(typeof result.errorMessage, 'string');
		assert.doesNotThrow(() => steamApi.steam.runCallbacks());
		assert.ok(Array.isArray(steamApi.callbacks.pollCallbacks()));
	} finally {
		assert.doesNotThrow(() => steamApi.steam.shutdown());
	}
});

test('SteamID helpers operate without a Steam session', { skip: nativeSkip }, async () => {
	const steamApi = await loadSteamApi();
	const individualSteamId = '76561197960287930' as TSteamId;
	const nilSteamId = '0' as TSteamId;

	assert.equal(steamApi.steamId.isValid(individualSteamId), true);
	assert.equal(steamApi.steamId.getRawSteamId(individualSteamId), individualSteamId);
	assert.equal(steamApi.steamId.getAccountId(individualSteamId), 22202);
	assert.equal(
		steamApi.steamId.getAccountType(individualSteamId),
		steamApi.AccountType.Individual,
	);
	assert.equal(steamApi.steamId.getStaticAccountKey(individualSteamId), '76561193665320634');
	assert.equal(steamApi.steamId.isIndividualAccount(individualSteamId), true);
	assert.equal(steamApi.steamId.isAnonymous(individualSteamId), false);
	assert.equal(steamApi.steamId.isLobby(individualSteamId), false);

	assert.equal(steamApi.steamId.isValid(nilSteamId), false);
	assert.equal(steamApi.steamId.getRawSteamId(nilSteamId), nilSteamId);
	assert.equal(steamApi.steamId.getAccountId(nilSteamId), 0);
	assert.equal(steamApi.steamId.getAccountType(nilSteamId), steamApi.AccountType.Invalid);
	assert.equal(steamApi.steamId.getStaticAccountKey(nilSteamId), '0');
	assert.equal(steamApi.steamId.isIndividualAccount(nilSteamId), false);
	assert.equal(steamApi.steamId.isAnonymous(nilSteamId), false);
	assert.equal(steamApi.steamId.isLobby(nilSteamId), false);
});

test('SteamID helpers validate uint64 decimal strings', { skip: nativeSkip }, async () => {
	const steamApi = await loadSteamApi();

	assert.throws(
		() => steamApi.steamId.isValid('' as TSteamId),
		/steamId must be a non-empty uint64 decimal string/u,
	);
	assert.throws(
		() => steamApi.steamId.isValid('not-a-steamid' as TSteamId),
		/steamId must be a uint64 decimal string/u,
	);
	assert.throws(
		() => steamApi.steamId.isValid('18446744073709551616' as TSteamId),
		/steamId is outside the uint64 range/u,
	);
});
