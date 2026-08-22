# Node3D Steam API usage guide

The Steamworks reference defines Steam's feature semantics. These guides define
how `@node-3d/steam-api` presents those features to Node.js: JavaScript values,
promise completion, callback delivery, and convenience helpers.

## Start here

1. [Lifecycle and callbacks](lifecycle.md) — initialize once, pump callbacks,
   and await Steam call results safely.
2. [IDs and result values](ids-and-results.md) — branded decimal-string IDs,
   enum objects, `boolean`, and `null` conventions.
3. Select the interface guide that owns the Steam operation.

## Interfaces

| Node3D namespace | Guide | Official Steamworks reference |
| --- | --- | --- |
| `steam`, `callbacks` | [Lifecycle and callbacks](lifecycle.md) | [Steamworks API overview](https://partner.steamgames.com/doc/sdk/api) |
| `steamId` | [Steam IDs](steam-id.md) | [SteamID](https://partner.steamgames.com/doc/api/steam_api#CSteamID) |
| `apps` | [Apps](apps.md) | [ISteamApps](https://partner.steamgames.com/doc/api/ISteamApps) |
| `user` | [User and authentication](user.md) | [ISteamUser](https://partner.steamgames.com/doc/api/ISteamUser) |
| `utils` | [Utilities and overlay](utils.md) | [ISteamUtils](https://partner.steamgames.com/doc/api/ISteamUtils) |
| `friends` | [Friends and presence](friends.md) | [ISteamFriends](https://partner.steamgames.com/doc/api/ISteamFriends) |
| `matchmaking` | [Lobbies](matchmaking.md) | [ISteamMatchmaking](https://partner.steamgames.com/doc/api/ISteamMatchmaking) |
| `networking` | [Legacy P2P networking](networking.md) | [ISteamNetworking](https://partner.steamgames.com/doc/api/ISteamNetworking) |
| `cloud` | [Cloud files](cloud.md) | [ISteamRemoteStorage](https://partner.steamgames.com/doc/api/ISteamRemoteStorage) |
| `ugc` | [Workshop and UGC](ugc.md) | [ISteamUGC](https://partner.steamgames.com/doc/api/ISteamUGC) |
| `userStats` | [Stats and achievements](user-stats.md) | [ISteamUserStats](https://partner.steamgames.com/doc/api/ISteamUserStats) |

Every guide assumes `steam.initEx()` has succeeded unless it states otherwise.
