# Greenworks Migration Notes

This document is for projects migrating from
[`greenheartgames/greenworks`](https://github.com/greenheartgames/greenworks)
to `@node-3d/steam-api`.

`@node-3d/steam-api` targets capability parity with Greenworks, not drop-in API
compatibility. Code should import the Node3D ESM API and use grouped namespaces
instead of Greenworks' flat CommonJS module.

## Import and Initialization

Greenworks:

```js
const greenworks = require('greenworks');

if (!greenworks.init()) {
	throw new Error('Steam initialization failed');
}
```

Node3D:

```ts
import { init, steam } from '@node-3d/steam-api';

init();

// For non-throwing initialization:
const result = steam.initEx();
if (!result.ok) {
	throw new Error(result.errorMessage);
}
```

## Callback Pumping

Greenworks exposes an EventEmitter-style API. Node3D currently uses explicit
callback polling. Event names use Greenworks' kebab-case strings, and
Greenworks positional event arguments become named object fields:

```ts
import { update } from '@node-3d/steam-api';

for (const event of update()) {
	switch (event.type) {
		case 'game-overlay-activated':
			console.log(event.active);
			break;
		case 'steam-server-connect-failure':
			console.log(event.result, event.stillRetrying);
			break;
		case 'dlc-installed':
			console.log(event.appId);
			break;
	}
}
```

`update()` calls `steam.runCallbacks()` and then drains
`callbacks.pollCallbacks()`.

Implemented event mappings:

| Greenworks event | Node3D callback payload |
| --- | --- |
| `game-overlay-activated`, `isActive` | `{ type: 'game-overlay-activated', active, userInitiated, appId, overlayPid }` |
| `persona-state-change`, `steamId`, `personaChangeFlag` | `{ type: 'persona-state-change', steamId, changeFlags }` |
| `avatar-image-loaded`, `steamId`, `image`, `height`, `width` | `{ type: 'avatar-image-loaded', steamId, image, width, height }` |
| `game-connected-friend-chat-message`, `steamId`, `messageId` | `{ type: 'game-connected-friend-chat-message', steamId, messageId }` |
| `rich-presence-join-requested`, `steamIdFriend`, `connect` | `{ type: 'rich-presence-join-requested', steamIdFriend, connect }` |
| `lobby-created`, `statusCode`, `steamIdLobby` | `{ type: 'lobby-created', result, lobbyId }` |
| `lobby-data-update`, `steamIdLobby`, `steamIdMember`, `success` | `{ type: 'lobby-data-update', lobbyId, steamIdMember, success }` |
| `lobby-enter`, `steamIdLobby`, `chatPermissions`, `locked`, `chatRoomEnterResponse` | `{ type: 'lobby-enter', lobbyId, chatPermissions, locked, response }` |
| `lobby-invite`, `steamIdUser`, `steamIdLobby`, `gameId` | `{ type: 'lobby-invite', steamIdUser, lobbyId, gameId }` |
| `lobby-join-requested`, `steamIdLobby`, `steamIdUser` | `{ type: 'lobby-join-requested', lobbyId, steamIdFriend }` |
| `lobby-match-list`, `lobbiesMatching` | `{ type: 'lobby-match-list', lobbiesMatching }` |
| `lobby-chat-update`, `steamIdLobby`, `steamIdUserChanged`, `steamIdMakingChange`, `chatMemberStateChange` | `{ type: 'lobby-chat-update', lobbyId, steamIdUserChanged, steamIdMakingChange, stateChange }` |
| `lobby-chat-msg`, `steamIdLobby`, `steamIdUser`, `chatEntryType`, `chatId` | `{ type: 'lobby-chat-msg', lobbyId, steamIdUser, chatEntryType, chatId }` |
| `p2p-session-request`, `steamIdRemote` | `{ type: 'p2p-session-request', steamIdRemote }` |
| `p2p-session-connect-fail`, `steamIdRemote`, `p2pSessionError` | `{ type: 'p2p-session-connect-fail', steamIdRemote, sessionError }` |
| `validate-auth-ticket`, `steamId`, `authSessionResponse`, `ownerSteamId` | `{ type: 'validate-auth-ticket', steamId, authSessionResponse, ownerSteamId }` |
| `steam-servers-connected` | `{ type: 'steam-servers-connected' }` |
| `steam-servers-disconnected` | `{ type: 'steam-servers-disconnected', result }` |
| `steam-server-connect-failure`, `statusCode` | `{ type: 'steam-server-connect-failure', result, stillRetrying }` |
| `steam-shutdown` | `{ type: 'steam-shutdown' }` |
| `dlc-installed`, `appId` | `{ type: 'dlc-installed', appId }` |
| `new-url-launch-parameters` | `{ type: 'new-url-launch-parameters' }` |
| `floating-gamepad-text-input-dismissed` | `{ type: 'floating-gamepad-text-input-dismissed' }` |

## Current Capability Mappings

| Greenworks | Node3D |
| --- | --- |
| `greenworks.init()` | `init()` |
| `greenworks.initAPI()` | `steam.initEx()` |
| `greenworks.restartAppIfNecessary(appId)` | `steam.restartAppIfNecessary(appId)` |
| `greenworks.isSteamRunning()` | `steam.isSteamRunning()` |
| `greenworks.isSteamRunningOnSteamDeck()` | `utils.isSteamRunningOnSteamDeck()` |
| `greenworks.showFloatingGamepadTextInput(keyboardMode, x, y, width, height)` | `utils.showFloatingGamepadTextInput(keyboardMode, x, y, width, height)` |
| `greenworks.getSteamId()` | `user.getSteamId()` |
| `SteamID.isAnonymous()` | `steamId.isAnonymous(id)` |
| `SteamID.isAnonymousGameServer()` | `steamId.isAnonymousGameServer(id)` |
| `SteamID.isAnonymousGameServerLogin()` | `steamId.isAnonymousGameServerLogin(id)` |
| `SteamID.isAnonymousUser()` | `steamId.isAnonymousUser(id)` |
| `SteamID.isChatAccount()` | `steamId.isChatAccount(id)` |
| `SteamID.isClanAccount()` | `steamId.isClanAccount(id)` |
| `SteamID.isConsoleUserAccount()` | `steamId.isConsoleUserAccount(id)` |
| `SteamID.isContentServerAccount()` | `steamId.isContentServerAccount(id)` |
| `SteamID.isGameServerAccount()` | `steamId.isGameServerAccount(id)` |
| `SteamID.isIndividualAccount()` | `steamId.isIndividualAccount(id)` |
| `SteamID.isPersistentGameServerAccount()` | `steamId.isPersistentGameServerAccount(id)` |
| `SteamID.isLobby()` | `steamId.isLobby(id)` |
| `SteamID.getAccountID()` | `steamId.getAccountId(id)` |
| `SteamID.getRawSteamID()` | `steamId.getRawSteamId(id)` |
| `SteamID.getAccountType()` | `steamId.getAccountType(id)` |
| `SteamID.isValid()` | `steamId.isValid(id)` |
| `SteamID.getStaticAccountKey()` | `steamId.getStaticAccountKey(id)` |
| `SteamID.getPersonaName()` | `steamId.getPersonaName(id)` |
| `SteamID.getNickname()` | `steamId.getNickname(id)` |
| `SteamID.getRelationship()` | `steamId.getRelationship(id)` |
| `SteamID.getSteamLevel()` | `steamId.getSteamLevel(id)` |
| `greenworks.getAppId()` | `utils.getAppId()` |
| `greenworks.getAppBuildId()` | `apps.getAppBuildId()` |
| `greenworks.getCurrentGameLanguage()` | `apps.getCurrentGameLanguage()` |
| `greenworks.getCurrentUILanguage()` | `utils.getSteamUiLanguage()` |
| `greenworks.getAppInstallDir(appId, buffer, buffer_size)` | `apps.getAppInstallDir(appId)` |
| `greenworks.getCurrentGameInstallDir()` | `apps.getCurrentGameInstallDir()` |
| `greenworks.getLaunchCommandLine()` | `apps.getLaunchCommandLine()` |
| `greenworks.activateGameOverlay(option)` | `friends.activateGameOverlay(dialog)` |
| `greenworks.activateGameOverlayToWebPage(url)` | `friends.activateGameOverlayToWebPage(url)` |
| `greenworks.activateGameOverlayToStore(appId, storeFlag)` | `friends.activateGameOverlayToStore(appId, flag)` |
| `greenworks.getFriendCount(friendFlag)` | `friends.getFriendCount(friendFlag)` |
| `greenworks.getFriends(friendFlag)` | `friends.getFriends(friendFlag)` |
| `greenworks.requestUserInformation(id, requireNameOnly)` | `friends.requestUserInformation(id, requireNameOnly)` |
| `greenworks.getSmallFriendAvatar(id)` | `friends.getSmallFriendAvatar(id)` |
| `greenworks.getMediumFriendAvatar(id)` | `friends.getMediumFriendAvatar(id)` |
| `greenworks.getLargeFriendAvatar(id)` | `friends.getLargeFriendAvatar(id)` |
| `greenworks.setListenForFriendsMessage(interceptEnabled)` | `friends.setListenForFriendsMessage(interceptEnabled)` |
| `greenworks.replyToFriendMessage(id, message)` | `friends.replyToFriendMessage(id, message)` |
| `greenworks.getFriendMessage(id, messageId, maximumMessageSize)` | `friends.getFriendMessage(id, messageId, maximumMessageSize)` |
| `greenworks.getFriendPersonaName(id)` | `friends.getFriendPersonaName(id)` |
| `greenworks.setRichPresence(key, value)` | `friends.setRichPresence(key, value)` |
| `greenworks.clearRichPresence()` | `friends.clearRichPresence()` |
| `greenworks.getFriendRichPresence(id, key)` | `friends.getFriendRichPresence(id, key)` |
| `greenworks.setPlayedWith(id)` | `friends.setPlayedWith(id)` |
| `greenworks.getFriendGamePlayed(id)` | `friends.getFriendGamePlayed(id)` |
| `greenworks.activateGameOverlayInviteDialog(lobbyId)` | `friends.activateGameOverlayInviteDialog(lobbyId)` |
| `greenworks.activateGameOverlayToUser(dialog, id)` | `friends.activateGameOverlayToUser(dialog, id)` |
| `greenworks.createLobby(lobbyType, maxMembers)` | `matchmaking.createLobby(lobbyType, maxMembers)` |
| `greenworks.deleteLobbyData(lobbyId, key)` | `matchmaking.deleteLobbyData(lobbyId, key)` |
| `greenworks.getLobbyByIndex(index)` | `matchmaking.getLobbyByIndex(index)` |
| `greenworks.getLobbyData(lobbyId, key)` | `matchmaking.getLobbyData(lobbyId, key)` |
| `greenworks.getLobbyMemberByIndex(lobbyId, index)` | `matchmaking.getLobbyMemberByIndex(lobbyId, index)` |
| `greenworks.getNumLobbyMembers(lobbyId)` | `matchmaking.getNumLobbyMembers(lobbyId)` |
| `greenworks.getLobbyOwner(lobbyId)` | `matchmaking.getLobbyOwner(lobbyId)` |
| `greenworks.inviteUserToLobby(lobbyId, inviteeId)` | `matchmaking.inviteUserToLobby(lobbyId, inviteeId)` |
| `greenworks.joinLobby(lobbyId)` | `matchmaking.joinLobby(lobbyId)` |
| `greenworks.leaveLobby(lobbyId)` | `matchmaking.leaveLobby(lobbyId)` |
| `greenworks.setLobbyData(lobbyId, key, value)` | `matchmaking.setLobbyData(lobbyId, key, value)` |
| `greenworks.setLobbyJoinable(lobbyId, joinable)` | `matchmaking.setLobbyJoinable(lobbyId, joinable)` |
| `greenworks.setLobbyOwner(lobbyId, ownerId)` | `matchmaking.setLobbyOwner(lobbyId, ownerId)` |
| `greenworks.setLobbyType(lobbyId, lobbyType)` | `matchmaking.setLobbyType(lobbyId, lobbyType)` |
| `greenworks.requestLobbyList()` | `matchmaking.requestLobbyList()` |
| `greenworks.getLobbyMemberLimit(lobbyId)` | `matchmaking.getLobbyMemberLimit(lobbyId)` |
| `greenworks.setLobbyMemberLimit(lobbyId, limit)` | `matchmaking.setLobbyMemberLimit(lobbyId, limit)` |
| `greenworks.getLobbyMemberData(lobbyId, memberId, key)` | `matchmaking.getLobbyMemberData(lobbyId, memberId, key)` |
| `greenworks.setLobbyMemberData(lobbyId, key, value)` | `matchmaking.setLobbyMemberData(lobbyId, key, value)` |
| `greenworks.getLobbyDataCount(lobbyId)` | `matchmaking.getLobbyDataCount(lobbyId)` |
| `greenworks.getLobbyDataByIndex(lobbyId, index)` | `matchmaking.getLobbyDataByIndex(lobbyId, index)` |
| `greenworks.sendLobbyChatMsg(lobbyId, data)` | `matchmaking.sendLobbyChatMsg(lobbyId, data)` |
| `greenworks.getLobbyChatEntry(lobbyId, chatId)` | `matchmaking.getLobbyChatEntry(lobbyId, chatId)` |
| `greenworks.sendP2PPacket(steamId, sendType, data, channel)` | `networking.sendP2PPacket(steamId, sendType, data, channel)` |
| `greenworks.isP2PPacketAvailable(channel)` | `networking.isP2PPacketAvailable(channel)` |
| `greenworks.readP2PPacket(size, channel)` | `networking.readP2PPacket(size, channel)` |
| `greenworks.acceptP2PSessionWithUser(steamId)` | `networking.acceptP2PSessionWithUser(steamId)` |
| `greenworks.getP2PSessionState(steamId)` | `networking.getP2PSessionState(steamId)` |
| `greenworks.closeP2PSessionWithUser(steamId)` | `networking.closeP2PSessionWithUser(steamId)` |
| `greenworks.closeP2PChannelWithUser(steamId, channel)` | `networking.closeP2PChannelWithUser(steamId, channel)` |
| `greenworks.isBehindNAT()` | `networking.isBehindNat()` |
| `greenworks.isGameOverlayEnabled()` | `utils.isOverlayEnabled()` |
| `greenworks.isSteamInBigPictureMode()` | `utils.isSteamInBigPictureMode()` |
| `greenworks.isSubscribedApp(appId)` | `apps.isSubscribedApp(appId)` |
| `greenworks.isAppInstalled(appId)` | `apps.isAppInstalled(appId)` |
| `greenworks.getImageSize(handle)` | `utils.getImageSize(image)` |
| `greenworks.getImageRGBA(handle)` | `utils.getImageRgba(image)` |
| `greenworks.getIPCountry()` | `utils.getIpCountry()` |
| `greenworks.getDLCCount()` | `apps.getDlcCount()` |
| `greenworks.getDLCDataByIndex(index)` | `apps.getDlcDataByIndex(index)` |
| `greenworks.isDLCInstalled(appId)` | `apps.isDlcInstalled(appId)` |
| `greenworks.installDLC(appId)` | `apps.installDlc(appId)` |
| `greenworks.uninstallDLC(appId)` | `apps.uninstallDlc(appId)` |
| `greenworks.getAuthSessionTicket(success, error)` | `user.getAuthSessionTicket()` |
| `greenworks.getAuthSessionTicketForWebAPI(identity, success, error)` | `user.getAuthTicketForWebApi(identity)` |
| `greenworks.cancelAuthTicket(handle)` | `user.cancelAuthTicket(handle)` |
| `greenworks.beginAuthSessionAsUser(ticket, steamId)` | `user.beginAuthSession(ticket, steamId)` |
| `greenworks.endAuthSessionAsUser(steamId)` | `user.endAuthSession(steamId)` |
| `greenworks.getEncryptedAppTicket(userData, success, error)` | `user.requestEncryptedAppTicket(userData)` |
| `greenworks.decryptAppTicket(encryptedTicket, key)` | `user.decryptAppTicket(encryptedTicket, key)` |
| `greenworks.isTicketForApp(decryptedTicket, appId)` | `user.isTicketForApp(decryptedTicket, appId)` |
| `greenworks.getTicketIssueTime(decryptedTicket)` | `user.getTicketIssueTime(decryptedTicket)` |
| `greenworks.getTicketSteamId(decryptedTicket)` | `user.getTicketSteamId(decryptedTicket)` |
| `greenworks.getTicketAppId(decryptedTicket)` | `user.getTicketAppId(decryptedTicket)` |
| `greenworks.EncryptedAppTicketSymmetricKeyLength` | `EncryptedAppTicketSymmetricKeyLength` |
| `greenworks.activateAchievement(name, success, error)` | `userStats.setAchievement(name)` |
| `greenworks.indicateAchievementProgress(name, current, max)` | `userStats.indicateAchievementProgress(name, current, max)` |
| `greenworks.getAchievement(name, success, error)` | `userStats.getAchievement(name)` |
| `greenworks.clearAchievement(name, success, error)` | `userStats.clearAchievement(name)` |
| `greenworks.getAchievementNames()` | `userStats.getAchievementNames()` |
| `greenworks.getNumberOfAchievements()` | `userStats.getNumAchievements()` |
| `greenworks.getNumberOfPlayers(success, error)` | `userStats.getNumberOfCurrentPlayers()` |
| `greenworks.getStatInt(name)` | `userStats.getStatInt(name)` |
| `greenworks.getStatFloat(name)` | `userStats.getStatFloat(name)` |
| `greenworks.setStat(name, value)` | `userStats.setStat(name, value)` |
| `greenworks.storeStats(success, error)` | `userStats.storeStats()` |
| `greenworks.resetAllStats(achievementsToo)` | `userStats.resetAllStats(achievementsToo)` |
| `greenworks.saveTextToFile(fileName, content, success, error)` | `cloud.saveTextToFile(fileName, content)` |
| `greenworks.readTextFromFile(fileName, success, error)` | `cloud.readTextFromFile(fileName)` |
| `greenworks.deleteFile(fileName, success, error)` | `cloud.deleteFile(fileName)` |
| `greenworks.saveFilesToCloud(filePaths, success, error)` | `cloud.saveFilesToCloud(filePaths)`; also available as `ugc.saveFilesToCloud(filePaths)` for workshop helpers |
| `greenworks.isCloudEnabledForUser()` | `cloud.isEnabledForUser()` |
| `greenworks.isCloudEnabled()` | `cloud.isEnabled()` |
| `greenworks.enableCloud(flag)` | `cloud.setEnabled(enabled)` |
| `greenworks.getCloudQuota(success, error)` | `cloud.getQuota()` |
| `greenworks.getFileCount()` | `cloud.getFileCount()` |
| `greenworks.getFileNameAndSize(index)` | `cloud.getFileNameAndSize(index)` |
| `greenworks.fileShare(filePath, success, error)` | `ugc.fileShare(filePath)` |
| `greenworks.publishWorkshopFile([options,] filePath, imagePath, title, description, success, error)` | `ugc.publishWorkshopFile(options, filePath, imagePath, title, description)` |
| `greenworks.updatePublishedWorkshopFile([options,] publishedFileId, filePath, imagePath, title, description, success, error)` | `ugc.updatePublishedWorkshopFile(options, publishedFileId, filePath, imagePath, title, description)` |
| `greenworks.ugcPublish(filePath, title, description, imagePath, success, error, progress)` | `ugc.publish(filePath, title, description, imagePath, options)` |
| `greenworks.ugcPublishUpdate(publishedFileId, filePath, title, description, imagePath, success, error, progress)` | `ugc.publishUpdate(publishedFileId, filePath, title, description, imagePath, options)` |
| `greenworks.ugcGetItems([options,] matchingType, queryType, success, error)` | `ugc.getItems(options, matchingType, queryType)` |
| `greenworks.ugcGetUserItems([options,] matchingType, sortOrder, list, success, error)` | `ugc.getUserItems(options, matchingType, sortOrder, list)` |
| `greenworks.ugcDownloadItem(file, downloadDir, success, error)` | `ugc.download(file, downloadDir)` |
| `greenworks.ugcSynchronizeItems([options,] syncDir, success, error)` | `ugc.synchronizeItems(options, syncDir)` |
| `greenworks.ugcUnsubscribe(publishedFileId, success, error)` | `ugc.unsubscribe(publishedFileId)` |
| `greenworks.ugcShowOverlay([publishedFileId])` | `ugc.showOverlay(publishedFileId?)` |
| `greenworks.ugcGetItemState(publishedFileId)` | `ugc.getItemState(publishedFileId)` |
| `greenworks.ugcGetItemInstallInfo(publishedFileId)` | `ugc.getItemInstallInfo(publishedFileId)` |

## Enum Mappings

Greenworks-compatible enum objects are available as top-level Node3D exports,
and Node3D also exposes Steamworks-oriented enums for newer APIs:

```ts
import { FriendFlags, LobbyType, Result } from '@node-3d/steam-api';
```

Available enum objects include:

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

`FriendFlags.Suggested` is not exported because it was removed from the
Steamworks SDK and Greenworks had already disabled that member in source.

## Stat and Achievement Differences

Greenworks combines integer and floating point stat writes:

```js
greenworks.setStat('score', 10);
```

Node3D supports the combined helper and also exposes the underlying type choice
explicitly:

```ts
import { userStats } from '@node-3d/steam-api';

userStats.setStat('score', 10);
userStats.setStatInt('score', 10);
userStats.setStatFloat('accuracy', 0.75);
```

Greenworks uses callback-style achievement helpers for set/get/clear. Node3D
exposes direct-return equivalents:

```ts
userStats.setAchievement('ACH_WIN_ONE_GAME');
const achieved = userStats.getAchievement('ACH_WIN_ONE_GAME');
userStats.clearAchievement('ACH_WIN_ONE_GAME');
```

Greenworks' callback-style `getNumberOfPlayers()` maps to Steamworks'
`GetNumberOfCurrentPlayers()` and returns a promise:

```ts
const players = await userStats.getNumberOfCurrentPlayers();
```

## UGC Differences

Greenworks exposes UGC helpers on the flat module:

```js
greenworks.ugcGetItems(
	{ app_id: appId, page_num: 1 },
	greenworks.UGCMatchingType.Items,
	greenworks.UGCQueryType.RankedByPublicationDate,
	(items) => console.log(items),
);
```

Node3D groups these under `ugc`, uses promises for Steam call results, and
returns `null` when install info is not available:

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

const query = waitForSteamCall(
	ugc.getItems(
		{
			appId,
			page: 1,
			requiredTags: ['level'],
			requiredTagGroups: [['challenge', 'puzzle']],
			excludedTags: ['spoiler'],
			returnMetadata: true,
			returnAdditionalPreviews: true,
			returnChildren: true,
			returnKeyValueTags: true,
		},
		UGCMatchingType.Items,
		UGCQueryType.RankedByPublicationDate,
	),
);
const state = ugc.getItemState(publishedFileId);
const info = ugc.getItemInstallInfo(publishedFileId);

ugc.showOverlay();
ugc.showOverlay(publishedFileId);

await waitForSteamCall(ugc.download((await query).items[0].file, './workshop'));
await waitForSteamCall(ugc.unsubscribe(publishedFileId));

const share = await waitForSteamCall(ugc.fileShare('item.dat'));
const publish = await waitForSteamCall(
	ugc.publish('item.dat', 'Workshop Item', 'Description', 'preview.png', {
		appId,
		tags: ['level'],
		onProgress: (progress) => console.log(progress.stage),
	}),
);

const sync = await waitForSteamCall(ugc.synchronizeItems({ appId, page: 1 }, './workshop'));
```

Node3D also exposes modern `ISteamUGC` item creation, update, subscription, and
download methods, plus vote and favorite interactions, that Greenworks' legacy
workshop helpers do not expose directly:

```ts
import {
	ItemUpdateStatus,
	RemoteStoragePublishedFileVisibility,
	WorkshopFileType,
	ugc,
	update,
	utils,
} from '@node-3d/steam-api';

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
let submitted = false;
submitPromise.then(
	() => {
		submitted = true;
	},
	() => {
		submitted = true;
	},
);

while (!submitted) {
	const progress = ugc.getItemUpdateProgress(updateHandle);
	if (progress.status !== ItemUpdateStatus.Invalid) {
		console.log(progress.bytesProcessed, progress.bytesTotal);
	}
	update();
	await new Promise((resolve) => setTimeout(resolve, 16));
}

await submitPromise;

await waitForSteamCall(ugc.subscribeItem(created.publishedFileId));
await waitForSteamCall(ugc.setUserItemVote(created.publishedFileId, true));
console.log(await waitForSteamCall(ugc.getUserItemVote(created.publishedFileId)));
await waitForSteamCall(ugc.addItemToFavorites(created.publishedFileId));
await waitForSteamCall(ugc.removeItemFromFavorites(created.publishedFileId));
console.log(ugc.getSubscribedItems());

if (ugc.downloadItem(created.publishedFileId, true)) {
	let downloaded = false;
	while (!downloaded) {
		for (const event of update()) {
			if (
				event.type === 'download-item-result' &&
				event.publishedFileId === created.publishedFileId
			) {
				downloaded = true;
				console.log(event.result);
			}
		}
		await new Promise((resolve) => setTimeout(resolve, 16));
	}
}

await waitForSteamCall(ugc.unsubscribeItem(created.publishedFileId));
```

`ItemUpdateStatus.Invalid` means Steam is not reporting active progress for the
handle. During an active submit, Steam reports preparing/uploading/committing
states plus byte counters.

Node3D query options use camelCase names and map to Steam's `ISteamUGC` query
setter methods. Greenworks-compatible `app_id` and `page_num` aliases are still
accepted for `appId` and `page`; newer options such as `requiredTagGroups`,
`returnMetadata`, and `allowCachedResponseMaxAgeSeconds` use Node3D names.

Node3D UGC detail objects use `steamIdOwner`, `consumerAppId`, `creatorAppId`,
and `url` instead of Greenworks' `steamIDOwner`, `consumerAppID`,
`creatorAppID`, and `URL`. `download()` resolves with the saved local
`path` instead of calling a success callback with no arguments. `downloadItem()`
maps to the modern `ISteamUGC::DownloadItem` method and reports completion with
the `download-item-result` callback event. `saveFilesToCloud()`
returns the Steam Cloud filenames written from local paths. `fileShare()`
resolves with `{ file }`, and `publishWorkshopFile()` /
`updatePublishedWorkshopFile()` resolve with `{ publishedFileId }` plus Steam's
legal-agreement flag. `publish()` and `publishUpdate()` compose the Greenworks
save, share, and publish/update sequence and report progress as typed stages.
`synchronizeItems()` returns query metadata plus items with `isUpdated` and the
local `path`.

## Steam IDs

Greenworks returns `SteamID` wrapper objects in several friends/auth APIs.
Node3D returns Steam IDs as branded strings for implemented APIs. This avoids
rounding 64-bit identifiers through JavaScript numbers while keeping the value
easy to pass back into future Steam APIs.

Use the singular `steamId` namespace for operations that Greenworks exposes as
methods on a `SteamID` wrapper:

```ts
import { steamId, user } from '@node-3d/steam-api';

const id = user.getSteamId();
console.log(steamId.getRawSteamId(id));
console.log(steamId.getAccountType(id));
console.log(steamId.getPersonaName(id));
```

Published file IDs and UGC handles follow the same string-first rule.

## Friends Differences

`friends.getFriends()` returns Steam ID strings instead of Greenworks `SteamID`
wrapper objects. Use `steamId.*(id)` helpers for operations that Greenworks
called as methods on each returned object.

`friends.getFriendGamePlayed()` returns camelCase fields: `gameId`, `gameIp`,
`gamePort`, `queryPort`, and `steamIdLobby`.

## Authentication Differences

Auth APIs live under the `user` namespace because they map to `ISteamUser`.
Greenworks callback-style APIs map to direct returns or promises:

```ts
import { EncryptedAppTicketSymmetricKeyLength, user } from '@node-3d/steam-api';

const sessionTicket = user.getAuthSessionTicket();
const webApiTicket = await user.getAuthTicketForWebApi('service-name');
const encryptedTicket = await user.requestEncryptedAppTicket(Buffer.from('payload'));

const key = Buffer.alloc(EncryptedAppTicketSymmetricKeyLength);
const decryptedTicket = user.decryptAppTicket(encryptedTicket, key);
```

`getAuthSessionTicket()` returns `null` if Steam does not issue a valid auth
ticket handle. Greenworks users should treat that as the failure callback case.

`beginAuthSessionAsUser()` maps to `user.beginAuthSession(ticket, steamId)`,
and `endAuthSessionAsUser()` maps to `user.endAuthSession(steamId)`.
`validate-auth-ticket` is surfaced through `update()` with
`{ steamId, authSessionResponse, ownerSteamId }`.

## Lobby Differences

Lobby APIs live under the `matchmaking` namespace. `createLobby()`,
`joinLobby()`, and `requestLobbyList()` return promises for Steam call results;
continue pumping callbacks with `update()` while waiting for them, as with UGC
promises.

Lobby IDs are branded strings. APIs that return a potentially invalid Steam ID,
such as `getLobbyByIndex()`, `getLobbyMemberByIndex()`, and `getLobbyOwner()`,
return `null` instead of a Greenworks `SteamID` wrapper object.

Lobby result objects use camelCase fields. For example, `joinLobby()` resolves
with `{ lobbyId, chatPermissions, locked, response }`, and
`getLobbyChatEntry()` returns `{ steamIdUser, data, chatEntryType }` where
`data` is a `Buffer`.

## P2P Networking Differences

Greenworks exposes legacy `ISteamNetworking` P2P helpers as flat module
functions. Node3D groups them under `networking`, matching the Steamworks
interface name:

```ts
import { P2PSendType, networking } from '@node-3d/steam-api';

networking.sendP2PPacket(remoteId, P2PSendType.Reliable, Buffer.from('hello'));

const size = networking.isP2PPacketAvailable();
const packet = size > 0 ? networking.readP2PPacket(size) : null;
```

`readP2PPacket()` returns `{ data, steamIdRemote, messageSize }` or `null`.
The read size is capped to avoid unbounded native allocation from caller input.
`getP2PSessionState()` uses camelCase connection-state fields such as
`connectionActive`, `sessionError`, `usingRelay`, and `bytesQueuedForSend`.
Greenworks' `isBehindNAT()` maps to `networking.isBehindNat()`.

## Floating Gamepad Text Input Differences

Greenworks exposes `showFloatingGamepadTextInput()` on the flat module. Node3D
keeps it under `utils`, matching `ISteamUtils`:

```ts
import { FloatingGamepadTextInputMode, utils } from '@node-3d/steam-api';

utils.showFloatingGamepadTextInput(FloatingGamepadTextInputMode.SingleLine, x, y, width, height);
utils.dismissFloatingGamepadTextInput();
```
