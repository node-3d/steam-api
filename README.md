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
helpers, auth tickets, DLC metadata, image utilities, and user
stats/achievements.

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

`user`:

- `user.getHSteamUser()`
- `user.getSteamId()`
- `user.isLoggedOn()`
- `user.getAuthSessionTicket()`
- `user.cancelAuthTicket(handle)`

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

`steam.runCallbacks()` maps to `SteamAPI_RunCallbacks`.
`callbacks.pollCallbacks()` drains the Node3D callback queue.
`update()` does both and returns queued typed callback payloads:

- `user-stats-received`: `{ gameId, userId, result }`
- `user-stats-stored`: `{ gameId, result }`
- `user-achievement-stored`: `{ gameId, name, currentProgress, maxProgress }`
- `auth-session-ticket-response`: `{ handle, result }`
- `game-overlay-activated`: `{ active, userInitiated, appId, overlayPid }`
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

Steam IDs and game IDs are exposed as branded strings because Steamworks uses
64-bit identifiers that should not be rounded through JavaScript numbers.

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
