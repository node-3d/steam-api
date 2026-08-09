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
| `greenworks.getSteamId()` | `user.getSteamId()` |
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
| `greenworks.cancelAuthTicket(handle)` | `user.cancelAuthTicket(handle)` |
| `greenworks.activateAchievement(name, success, error)` | `userStats.setAchievement(name)` |
| `greenworks.indicateAchievementProgress(name, current, max)` | `userStats.indicateAchievementProgress(name, current, max)` |
| `greenworks.getAchievement(name, success, error)` | `userStats.getAchievement(name)` |
| `greenworks.clearAchievement(name, success, error)` | `userStats.clearAchievement(name)` |
| `greenworks.getAchievementNames()` | `userStats.getAchievementNames()` |
| `greenworks.getNumberOfAchievements()` | `userStats.getNumAchievements()` |
| `greenworks.getStatInt(name)` | `userStats.getStatInt(name)` |
| `greenworks.getStatFloat(name)` | `userStats.getStatFloat(name)` |
| `greenworks.setStat(name, value)` | `userStats.setStat(name, value)` |
| `greenworks.storeStats(success, error)` | `userStats.storeStats()` |
| `greenworks.resetAllStats(achievementsToo)` | `userStats.resetAllStats(achievementsToo)` |

## Enum Mappings

Greenworks enum objects are available as top-level Node3D exports with the same
object names:

```ts
import { FriendFlags, LobbyType, Result } from '@node-3d/steam-api';
```

Available enum objects:

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

## Steam IDs

Greenworks returns `SteamID` wrapper objects in several friends/auth APIs.
Node3D currently returns Steam IDs as branded strings for implemented APIs. This
avoids rounding 64-bit identifiers through JavaScript numbers while keeping the
value easy to pass back into future Steam APIs.

## Missing Capability Families

The migration is not complete yet. Track implementation status in
`CHECKLIST.md`.

Currently missing major Greenworks capability families:

- cloud and remote storage,
- friends, avatars, chat, and rich presence,
- matchmaking and lobbies,
- P2P networking,
- workshop and UGC,
- archive utilities,
- floating gamepad text input,
- Greenworks event outcomes tied to missing API families,
- Greenworks constants that require additional SDK libraries.
