# @node-3d/steam-api

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/@node-3d%2Fsteam-api.svg)](https://badge.fury.io/js/@node-3d%2Fsteam-api)
[![Lint](https://github.com/node-3d/steam-api/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/steam-api/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/test.yml)

[WIP] Steamworks API bindings for Node.js.

## Install

```bash
npm install @node-3d/steam-api
```



Published packages include prebuilt native binaries.
A consumer install doesn't need a compiler or a local Steamworks SDK copy.

## Example

```ts
import { steam, update, user, utils } from '@node-3d/steam-api';

const result = steam.initEx();

if (!result.ok) {
	throw new Error(`Steamworks init failed: ${result.errorMessage}`);
}

console.log('app', utils.getAppId());
console.log('user', user.getSteamId(), user.isLoggedOn() ? 'logged on' : 'offline');

for (const event of update()) {
	console.log(event);
}
```

## API Surface

Migrating from Greenworks? See [GREENWORKS.md](GREENWORKS.md) for capability
mappings and migration notes.

The initial binding covers lifecycle, callback pumping, app/user/friends
helpers, matchmaking lobbies, P2P packet networking, auth tickets, DLC metadata,
image utilities, UGC item helpers, and user stats/achievements.

Methods are grouped by Steamworks interface rather than exported as one flat
module. This keeps names close to Valve's API documentation and avoids a second
Node3D-specific taxonomy for the full Steamworks surface.

`steam`:

- `steam.restartAppIfNecessary(appId)`
- `steam.initEx()`
- `steam.shutdown()`
- `steam.isSteamRunning()`
- `steam.runCallbacks()`
- `steam.releaseCurrentThreadMemory()`

Top-level helpers:

- `init()` - calls `steam.initEx()` and throws when initialization fails.
- `update()` - calls `steam.runCallbacks()` and then `callbacks.pollCallbacks()`.

`callbacks`:

- `callbacks.pollCallbacks()`

`steamId`:

- `steamId.isAnonymous(id)`
- `steamId.isAnonymousGameServer(id)`
- `steamId.isAnonymousGameServerLogin(id)`
- `steamId.isAnonymousUser(id)`
- `steamId.isChatAccount(id)`
- `steamId.isClanAccount(id)`
- `steamId.isConsoleUserAccount(id)`
- `steamId.isContentServerAccount(id)`
- `steamId.isGameServerAccount(id)`
- `steamId.isIndividualAccount(id)`
- `steamId.isPersistentGameServerAccount(id)`
- `steamId.isLobby(id)`
- `steamId.getAccountId(id)`
- `steamId.getRawSteamId(id)`
- `steamId.getAccountType(id)`
- `steamId.isValid(id)`
- `steamId.getStaticAccountKey(id)`
- `steamId.getPersonaName(id)`
- `steamId.getNickname(id)`
- `steamId.getRelationship(id)`
- `steamId.getSteamLevel(id)`

`user`:

- `user.getHSteamUser()`
- `user.getSteamId()`
- `user.isLoggedOn()`
- `user.getAuthSessionTicket()`
- `user.getAuthTicketForWebApi(identity?)`
- `user.cancelAuthTicket(handle)`
- `user.beginAuthSession(ticket, steamId)`
- `user.endAuthSession(steamId)`
- `user.requestEncryptedAppTicket(userData)`
- `user.getEncryptedAppTicket()`
- `user.decryptAppTicket(encryptedTicket, key)`
- `user.isTicketForApp(decryptedTicket, appId)`
- `user.getTicketIssueTime(decryptedTicket)`
- `user.getTicketSteamId(decryptedTicket)`
- `user.getTicketAppId(decryptedTicket)`

`utils`:

- `utils.getAppId()`
- `utils.getConnectedUniverse()`
- `utils.getIpCountry()`
- `utils.getServerRealTime()`
- `utils.getSteamUiLanguage()`
- `utils.getImageSize(image)`
- `utils.getImageRgba(image)`
- `utils.isOverlayEnabled()`
- `utils.isSteamInBigPictureMode()`
- `utils.isSteamRunningOnSteamDeck()`
- `utils.showFloatingGamepadTextInput(keyboardMode, x, y, width, height)`
- `utils.dismissFloatingGamepadTextInput()`

`apps`:

- `apps.isSubscribed()`
- `apps.isSubscribedApp(appId)`
- `apps.isDlcInstalled(appId)`
- `apps.isAppInstalled(appId)`
- `apps.installDlc(appId)`
- `apps.uninstallDlc(appId)`
- `apps.getAppBuildId()`
- `apps.getCurrentGameLanguage()`
- `apps.getAvailableGameLanguages()`
- `apps.getCurrentGameInstallDir()`
- `apps.getDlcCount()`
- `apps.getDlcDataByIndex(index)`
- `apps.getAppInstallDir(appId)`
- `apps.getLaunchCommandLine()`

`friends`:

- `friends.activateGameOverlay(dialog)`
- `friends.activateGameOverlayToWebPage(url, mode?)`
- `friends.activateGameOverlayToStore(appId, flag)`
- `friends.getFriendCount(friendFlag)`
- `friends.getFriends(friendFlag)`
- `friends.requestUserInformation(id, requireNameOnly)`
- `friends.getSmallFriendAvatar(id)`
- `friends.getMediumFriendAvatar(id)`
- `friends.getLargeFriendAvatar(id)`
- `friends.setListenForFriendsMessage(interceptEnabled)`
- `friends.replyToFriendMessage(id, message)`
- `friends.getFriendMessage(id, messageId, maximumMessageSize)`
- `friends.getFriendPersonaName(id)`
- `friends.setRichPresence(key, value)`
- `friends.clearRichPresence()`
- `friends.getFriendRichPresence(id, key)`
- `friends.setPlayedWith(id)`
- `friends.getFriendGamePlayed(id)`
- `friends.activateGameOverlayInviteDialog(lobbyId)`
- `friends.activateGameOverlayToUser(dialog, id)`

`matchmaking`:

- `matchmaking.createLobby(lobbyType, maxMembers)`
- `matchmaking.deleteLobbyData(lobbyId, key)`
- `matchmaking.getLobbyByIndex(index)`
- `matchmaking.getLobbyData(lobbyId, key)`
- `matchmaking.getLobbyMemberByIndex(lobbyId, index)`
- `matchmaking.getNumLobbyMembers(lobbyId)`
- `matchmaking.getLobbyOwner(lobbyId)`
- `matchmaking.inviteUserToLobby(lobbyId, inviteeId)`
- `matchmaking.joinLobby(lobbyId)`
- `matchmaking.leaveLobby(lobbyId)`
- `matchmaking.setLobbyData(lobbyId, key, value)`
- `matchmaking.setLobbyJoinable(lobbyId, joinable)`
- `matchmaking.setLobbyOwner(lobbyId, ownerId)`
- `matchmaking.setLobbyType(lobbyId, lobbyType)`
- `matchmaking.requestLobbyList()`
- `matchmaking.getLobbyMemberLimit(lobbyId)`
- `matchmaking.setLobbyMemberLimit(lobbyId, limit)`
- `matchmaking.getLobbyMemberData(lobbyId, memberId, key)`
- `matchmaking.setLobbyMemberData(lobbyId, key, value)`
- `matchmaking.getLobbyDataCount(lobbyId)`
- `matchmaking.getLobbyDataByIndex(lobbyId, index)`
- `matchmaking.sendLobbyChatMsg(lobbyId, data)`
- `matchmaking.getLobbyChatEntry(lobbyId, chatId, maximumMessageSize?)`

`createLobby()`, `joinLobby()`, and `requestLobbyList()` resolve from Steam
call results, so keep pumping callbacks with `steam.runCallbacks()` or
`update()` while their promises are pending.

`networking`:

- `networking.sendP2PPacket(steamId, sendType, data, channel?)`
- `networking.isP2PPacketAvailable(channel?)`
- `networking.readP2PPacket(size, channel?)`
- `networking.acceptP2PSessionWithUser(steamId)`
- `networking.getP2PSessionState(steamId)`
- `networking.closeP2PSessionWithUser(steamId)`
- `networking.closeP2PChannelWithUser(steamId, channel)`
- `networking.isBehindNat()`

`readP2PPacket()` returns `null` when no packet is available, otherwise
`{ data, steamIdRemote, messageSize }`.

`cloud`:

- `cloud.saveTextToFile(fileName, content)`
- `cloud.readTextFromFile(fileName)`
- `cloud.deleteFile(fileName)`
- `cloud.saveFilesToCloud(filePaths)`
- `cloud.isEnabledForUser()`
- `cloud.isEnabled()`
- `cloud.setEnabled(enabled)`
- `cloud.getQuota()`
- `cloud.getFileCount()`
- `cloud.getFileNameAndSize(index)`

`ugc`:

- `ugc.showOverlay(publishedFileId?)`
- `ugc.getItems(options, matchingType, queryType)`
- `ugc.getUserItems(options, matchingType, sortOrder, list)`
- `ugc.downloadItem(file, downloadDir)`
- `ugc.unsubscribe(publishedFileId)`
- `ugc.saveFilesToCloud(filePaths)`
- `ugc.fileShare(filePath)`
- `ugc.publishWorkshopFile(options, filePath, previewFilePath, title, description)`
- `ugc.updatePublishedWorkshopFile(options, publishedFileId, filePath, previewFilePath, title, description)`
- `ugc.publish(filePath, title, description, previewFilePath?, options?)`
- `ugc.publishUpdate(publishedFileId, filePath, title, description, previewFilePath?, options?)`
- `ugc.synchronizeItems(options, syncDir)`
- `ugc.getItemState(publishedFileId)`
- `ugc.getItemInstallInfo(publishedFileId)`

UGC promises resolve from Steam call results. Continue pumping callbacks with
`steam.runCallbacks()` or `update()` while a UGC promise is pending:

```ts
import { UGCMatchingType, UGCQueryType, ugc, update } from '@node-3d/steam-api';

async function waitForSteamCall<T>(promise: Promise<T>): Promise<T> {
	let settled = false;
	promise.then(
		() => {
			settled = true;
		},
		() => {
			settled = true;
		},
	);

	while (!settled) {
		update();
		await new Promise((resolve) => setTimeout(resolve, 16));
	}

	return promise;
}

const queryResult = await waitForSteamCall(
	ugc.getItems(
		{ page: 1 },
		UGCMatchingType.Items,
		UGCQueryType.RankedByPublicationDate,
	),
);
if (queryResult.items.length === 0) {
	throw new Error('UGC query returned no items');
}

const download = await waitForSteamCall(
	ugc.downloadItem(queryResult.items[0].file, './workshop'),
);
console.log(download.path);

const publish = await waitForSteamCall(
	ugc.publish('item.dat', 'Workshop Item', 'Description', 'preview.png', {
		tags: ['level'],
		onProgress: (progress) => console.log(progress.stage),
	}),
);
console.log(publish.publishedFileId);

const sync = await waitForSteamCall(ugc.synchronizeItems({ page: 1 }, './workshop'));
console.log(sync.items.map((item) => [item.path, item.isUpdated]));
```

`userStats`:

- `userStats.getStatInt(name)`
- `userStats.getStatFloat(name)`
- `userStats.setStat(name, value)`
- `userStats.setStatInt(name, value)`
- `userStats.setStatFloat(name, value)`
- `userStats.getAchievement(name)`
- `userStats.setAchievement(name)`
- `userStats.clearAchievement(name)`
- `userStats.indicateAchievementProgress(name, currentProgress, maxProgress)`
- `userStats.storeStats()`
- `userStats.resetAllStats(achievementsToo)`
- `userStats.getNumAchievements()`
- `userStats.getAchievementNames()`
- `userStats.getAchievementName(index)`
- `userStats.getAchievementDisplayAttribute(name, key)`
- `userStats.getAchievementAndUnlockTime(name)`
- `userStats.getNumberOfCurrentPlayers()`

`userStats.getNumberOfCurrentPlayers()` resolves from a Steam call result, so
keep pumping callbacks with `steam.runCallbacks()` or `update()` while its
promise is pending.

Top-level enum objects:

- `FriendFlags`
- `FriendRelationship`
- `PersonaChange`
- `AccountType`
- `ChatEntryType`
- `ChatMemberStateChange`
- `LobbyComparison`
- `LobbyDistanceFilter`
- `LobbyType`
- `Result`
- `UGCMatchingType`
- `UGCQueryType`
- `UserUGCList`
- `UserUGCListSortOrder`
- `UGCItemState`
- `FloatingGamepadTextInputMode`
- `P2PSendType`
- `BeginAuthSessionResult`
- `AuthSessionResponse`

Top-level constants:

- `EncryptedAppTicketSymmetricKeyLength`

`steam.runCallbacks()` maps to `SteamAPI_RunCallbacks`.
`callbacks.pollCallbacks()` drains the Node3D callback queue.
`update()` does both and returns queued typed callback payloads:

- `user-stats-received`: `{ gameId, userId, result }`
- `user-stats-stored`: `{ gameId, result }`
- `user-achievement-stored`: `{ gameId, name, currentProgress, maxProgress }`
- `auth-session-ticket-response`: `{ handle, result }`
- `auth-ticket-for-web-api-response`: `{ handle, result, ticketSize }`
- `validate-auth-ticket`: `{ steamId, authSessionResponse, ownerSteamId }`
- `game-overlay-activated`: `{ active, userInitiated, appId, overlayPid }`
- `persona-state-change`: `{ steamId, changeFlags }`
- `avatar-image-loaded`: `{ steamId, image, width, height }`
- `game-connected-friend-chat-message`: `{ steamId, messageId }`
- `rich-presence-join-requested`: `{ steamIdFriend, connect }`
- `lobby-created`: `{ result, lobbyId }`
- `lobby-data-update`: `{ lobbyId, steamIdMember, success }`
- `lobby-enter`: `{ lobbyId, chatPermissions, locked, response }`
- `lobby-invite`: `{ steamIdUser, lobbyId, gameId }`
- `lobby-join-requested`: `{ lobbyId, steamIdFriend }`
- `lobby-match-list`: `{ lobbiesMatching }`
- `lobby-chat-update`: `{ lobbyId, steamIdUserChanged, steamIdMakingChange, stateChange }`
- `lobby-chat-msg`: `{ lobbyId, steamIdUser, chatEntryType, chatId }`
- `p2p-session-request`: `{ steamIdRemote }`
- `p2p-session-connect-fail`: `{ steamIdRemote, sessionError }`
- `steam-servers-connected`: `{}`
- `steam-servers-disconnected`: `{ result }`
- `steam-server-connect-failure`: `{ result, stillRetrying }`
- `steam-shutdown`: `{}`
- `dlc-installed`: `{ appId }`
- `new-url-launch-parameters`: `{}`
- `floating-gamepad-text-input-dismissed`: `{}`

```ts
for (const event of update()) {
	switch (event.type) {
		case 'game-overlay-activated':
			console.log(event.active ? 'overlay opened' : 'overlay closed');
			break;
		case 'dlc-installed':
			console.log('DLC installed', event.appId);
			break;
		case 'auth-session-ticket-response':
			console.log('ticket response', event.handle, event.result);
			break;
	}
}
```

Steam IDs, game IDs, published file IDs, and UGC handles are exposed as branded
strings because Steamworks uses 64-bit identifiers that should not be rounded
through JavaScript numbers.

## Steamworks SDK Notice

Valve's Steamworks SDK files are not part of this repository.
This repository contains Node3D binding code only.

Maintainers building native binaries from a source checkout must provide the SDK
separately under Valve's Steamworks terms.

## Binary Origin

Release archives are built by this repository's public GitHub Actions workflows.

Attestations: https://github.com/node-3d/steam-api/attestations

To verify a downloaded archive:

```bash
gh release download <tag> -R node-3d/steam-api -p <platform>.gz
gh attestation verify <platform>.gz -R node-3d/steam-api
```
