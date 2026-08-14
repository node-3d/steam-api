# Steam API for Node.js

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/@node-3d%2Fsteam-api.svg)](https://badge.fury.io/js/@node-3d%2Fsteam-api)
[![Lint](https://github.com/node-3d/steam-api/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/steam-api/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/test.yml)
[![Cpplint](https://github.com/node-3d/steam-api/actions/workflows/cpplint.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/cpplint.yml)

```
npm install @node-3d/steam-api
```

**Node.js** addon with **Steamworks** client API bindings.

* Exposes typed ESM bindings grouped by Steamworks interface.
* Covers lifecycle, callback pumping, app/user/friends helpers, matchmaking
  lobbies, P2P packet networking, auth tickets, DLC metadata, image utilities,
  UGC item helpers, and user stats/achievements.
* Keeps API names close to Valve's Steamworks interfaces instead of exporting a
  Greenworks-style flat module.
* Provides Greenworks migration notes in [docs/GREENWORKS.md](docs/GREENWORKS.md).
* Includes repository examples for read-only probes, read-write workflows, and
  a Spacewar AppID 480 Node3D/Core demo.

> Note: this **addon uses N-API**, and therefore its Steam API binary is
ABI-compatible across different Node.js versions. Published packages install
prebuilt native binaries from this repository's GitHub releases. There is no
compilation step or local Steamworks SDK requirement during `npm install` when
matching archives are available.

Applications still need the normal Steamworks runtime context: the Steam client
must be available where the app runs, and development builds launched outside
Steam need an app id context such as `steam_appid.txt` in the current working
directory.

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

## API

Migrating from Greenworks? See [docs/GREENWORKS.md](docs/GREENWORKS.md) for
capability mappings and migration notes.

Development coverage is tracked in [docs/CHECKLIST.md](docs/CHECKLIST.md).

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

`getAuthSessionTicket()` returns `null` when Steam does not issue a valid auth
ticket handle. Async auth ticket requests fail immediately if Steam refuses to
start the request.

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
`{ data, steamIdRemote, messageSize }`. The requested read size is capped to
avoid unbounded native allocation.

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
- `ugc.createItem(appId?, fileType?)`
- `ugc.startItemUpdate(appId, publishedFileId)`
- `ugc.setItemTitle(updateHandle, title)`
- `ugc.setItemDescription(updateHandle, description)`
- `ugc.setItemMetadata(updateHandle, metadata)`
- `ugc.setItemVisibility(updateHandle, visibility)`
- `ugc.setItemTags(updateHandle, tags, allowAdminTags?)`
- `ugc.setItemContent(updateHandle, contentFolder)`
- `ugc.setItemPreview(updateHandle, previewFile)`
- `ugc.submitItemUpdate(updateHandle, changeNote?)`
- `ugc.getItemUpdateProgress(updateHandle)`
- `ugc.setUserItemVote(publishedFileId, voteUp)`
- `ugc.getUserItemVote(publishedFileId)`
- `ugc.addItemToFavorites(publishedFileId, appId?)`
- `ugc.removeItemFromFavorites(publishedFileId, appId?)`
- `ugc.subscribeItem(publishedFileId)`
- `ugc.unsubscribeItem(publishedFileId)`
- `ugc.getNumSubscribedItems(includeLocallyDisabled?)`
- `ugc.getSubscribedItems(maxEntries?, includeLocallyDisabled?)`
- `ugc.getItemDownloadInfo(publishedFileId)`
- `ugc.downloadItem(publishedFileId, highPriority?)`
- `ugc.download(file, downloadDir)`
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
`steam.runCallbacks()` or `update()` while a UGC promise is pending.
`downloadItem()` maps to `ISteamUGC::DownloadItem`, returns `boolean`, and
reports completion through the `download-item-result` callback event:

`getItems()` and `getUserItems()` query options map to Steam's query setter
methods. `requiredTags`, `requiredTagGroups`, `excludedTags`, return toggles,
`language`, and `allowCachedResponseMaxAgeSeconds` are applied before
`SendQueryUGCRequest()`. `matchAnyTag` is only valid for `getItems()`.

```ts
import {
	RemoteStoragePublishedFileVisibility,
	UGCMatchingType,
	UGCQueryType,
	WorkshopFileType,
	ugc,
	update,
	utils,
} from '@node-3d/steam-api';

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
		{
			page: 1,
			requiredTags: ['level'],
			requiredTagGroups: [['challenge', 'puzzle']],
			excludedTags: ['spoiler'],
			matchAnyTag: false,
			returnMetadata: true,
			returnAdditionalPreviews: true,
			returnChildren: true,
			returnKeyValueTags: true,
			language: 'english',
			allowCachedResponseMaxAgeSeconds: 60,
		},
		UGCMatchingType.Items,
		UGCQueryType.RankedByPublicationDate,
	),
);
if (queryResult.items.length === 0) {
	throw new Error('UGC query returned no items');
}

const download = await waitForSteamCall(ugc.download(queryResult.items[0].file, './workshop'));
console.log(download.path);
console.log(queryResult.items[0].metadata);
console.log(queryResult.items[0].additionalPreviews);

const publishedFileId = queryResult.items[0].publishedFileId;
await waitForSteamCall(ugc.subscribeItem(publishedFileId));

await waitForSteamCall(ugc.setUserItemVote(publishedFileId, true));
console.log(await waitForSteamCall(ugc.getUserItemVote(publishedFileId)));
await waitForSteamCall(ugc.addItemToFavorites(publishedFileId));
await waitForSteamCall(ugc.removeItemFromFavorites(publishedFileId));

console.log(ugc.getNumSubscribedItems());
console.log(ugc.getSubscribedItems());

if (ugc.downloadItem(publishedFileId, true)) {
	let downloaded = false;
	while (!downloaded) {
		for (const event of update()) {
			if (event.type === 'download-item-result' && event.publishedFileId === publishedFileId) {
				downloaded = true;
				console.log(event.result);
			}
		}
		await new Promise((resolve) => setTimeout(resolve, 16));
	}
}

console.log(ugc.getItemDownloadInfo(publishedFileId));
await waitForSteamCall(ugc.unsubscribeItem(publishedFileId));

const created = await waitForSteamCall(ugc.createItem(utils.getAppId(), WorkshopFileType.Community));
const updateHandle = ugc.startItemUpdate(utils.getAppId(), created.publishedFileId);
ugc.setItemTitle(updateHandle, 'Workshop Item');
ugc.setItemDescription(updateHandle, 'Description');
ugc.setItemMetadata(updateHandle, JSON.stringify({ version: 1 }));
ugc.setItemVisibility(updateHandle, RemoteStoragePublishedFileVisibility.Public);
ugc.setItemTags(updateHandle, ['level']);
ugc.setItemContent(updateHandle, './workshop-content');
ugc.setItemPreview(updateHandle, './preview.png');

const submitPromise = ugc.submitItemUpdate(updateHandle, 'Initial upload');
console.log(ugc.getItemUpdateProgress(updateHandle));
const submitted = await waitForSteamCall(submitPromise);
console.log(submitted.publishedFileId);

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

`getSubscribedItems(maxEntries?)` clamps `maxEntries` to Steam's current
subscribed item count before allocating its result buffer.

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
- `ItemUpdateStatus`
- `ItemPreviewType`
- `RemoteStoragePublishedFileVisibility`
- `WorkshopFileType`
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
- `download-item-result`: `{ appId, publishedFileId, result }`

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
