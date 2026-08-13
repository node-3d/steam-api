# Basic Examples

These examples run in Node without a browser host.

## Read Only

- `read-only/basic.ts` - initialization, app id, local user, and callback polling.
- `read-only/events.ts` - long-running callback pump.
- `read-only/account.ts` - local Steam account identity and SteamID helper output.
- `read-only/app-runtime.ts` - app, language, install path, overlay, and runtime flags.
- `read-only/dlc-inventory.ts` - DLC metadata and installed state.
- `read-only/friends.ts` - friend identities, game activity, levels, and avatar sizes.
- `read-only/cloud-inventory.ts` - Steam Cloud enablement, quota, and file list.
- `read-only/achievements-stats.ts` - achievement definitions/state and selected stat reads.
- `read-only/lobby-list.ts` - public lobby query and lobby metadata.
- `read-only/ugc-inventory.ts` - subscribed UGC ids, item details, install, and download state.

## Read Write

- `read-write/greenworks-sample.ts` - Node3D reproduction of Greenworks' Electron/NW API sample.
- `read-write/cloud.ts` - Steam Cloud text file, quota, and file enumeration.
- `read-write/achievements-stats.ts` - achievement and stat read/write flows.
- `read-write/friends-overlay.ts` - friends, avatars, rich presence, and overlay entry points.
- `read-write/lobbies.ts` - lobby list, create, join, metadata, member metadata, and chat.
- `read-write/p2p.ts` - legacy Steam P2P packets and session events.
- `read-write/auth.ts` - auth session tickets, Web API tickets, and encrypted app tickets.
- `read-write/dlc-apps.ts` - app metadata, install directories, language, and DLC metadata.
- `read-write/ugc-query-download.ts` - modern UGC query, subscribe, vote, favorite, and download helpers.
- `read-write/ugc-create-update.ts` - modern Workshop item creation/update lifecycle.
- `read-write/ugc-greenworks-legacy.ts` - Greenworks-compatible legacy Workshop helper flow.
- `read-write/steamdeck-input.ts` - Steam Deck/Big Picture state and floating gamepad text input.
