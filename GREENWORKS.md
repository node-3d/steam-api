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
callback polling:

```ts
import { update } from '@node-3d/steam-api';

for (const event of update()) {
	console.log(event);
}
```

`update()` calls `steam.runCallbacks()` and then drains
`callbacks.pollCallbacks()`.

## Current Capability Mappings

| Greenworks | Node3D |
| --- | --- |
| `greenworks.init()` | `init()` |
| `greenworks.initAPI()` | `steam.initEx()` |
| `greenworks.restartAppIfNecessary(appId)` | `steam.restartAppIfNecessary(appId)` |
| `greenworks.isSteamRunning()` | `steam.isSteamRunning()` |
| `greenworks.getSteamId()` | `user.getSteamId()` |
| `greenworks.getAppId()` | `utils.getAppId()` |
| `greenworks.getCurrentGameLanguage()` | `apps.getCurrentGameLanguage()` |
| `greenworks.getCurrentUILanguage()` | `utils.getSteamUiLanguage()` |
| `greenworks.getAppInstallDir(appId, buffer, buffer_size)` | `apps.getAppInstallDir(appId)` |
| `greenworks.isGameOverlayEnabled()` | `utils.isOverlayEnabled()` |
| `greenworks.isSteamInBigPictureMode()` | `utils.isSteamInBigPictureMode()` |
| `greenworks.isSubscribedApp(appId)` | `apps.isSubscribedApp(appId)` |
| `greenworks.isAppInstalled(appId)` | `apps.isAppInstalled(appId)` |
| `greenworks.getIPCountry()` | `utils.getIpCountry()` |
| `greenworks.getDLCCount()` | `apps.getDlcCount()` |
| `greenworks.getDLCDataByIndex(index)` | `apps.getDlcDataByIndex(index)` |
| `greenworks.isDLCInstalled(appId)` | `apps.isDlcInstalled(appId)` |
| `greenworks.getAuthSessionTicket(success, error)` | `user.getAuthSessionTicket()` |
| `greenworks.cancelAuthTicket(handle)` | `user.cancelAuthTicket(handle)` |
| `greenworks.getNumberOfAchievements()` | `userStats.getNumAchievements()` |
| `greenworks.getStatInt(name)` | `userStats.getStatInt(name)` |
| `greenworks.getStatFloat(name)` | `userStats.getStatFloat(name)` |
| `greenworks.resetAllStats(achievementsToo)` | `userStats.resetAllStats(achievementsToo)` |

## Stat and Achievement Differences

Greenworks combines integer and floating point stat writes:

```js
greenworks.setStat('score', 10);
```

Node3D exposes the underlying type choice explicitly:

```ts
import { userStats } from '@node-3d/steam-api';

userStats.setStatInt('score', 10);
userStats.setStatFloat('accuracy', 0.75);
```

Greenworks uses callback-style achievement helpers for set/get/clear. Node3D
currently exposes direct-return equivalents:

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
- many Greenworks event outcomes,
- Greenworks enum object exports.
