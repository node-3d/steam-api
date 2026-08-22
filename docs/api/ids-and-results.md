# IDs, enums, and result values

Steam identifiers are decimal strings in JavaScript, not `number` or `bigint`.
This preserves all 64 bits. TypeScript brands `TSteamId`, `TSteamLobbyId`,
`TSteamPublishedFileId`, `TGameId`, UGC handles, and update handles to prevent
accidental mixing; values returned by this package already have the right type.

```ts
const userId = user.getSteamId();
const { lobbyId } = await waitForSteamCall(matchmaking.createLobby(LobbyType.Public, 4));
```

When an ID originates outside the package, pass its decimal string after
validating it with `steamId.isValid(id)`. Use `steamId.getAccountId(id)`,
`getAccountType(id)`, and the `is*` helpers for SteamID classification.

The exported enum objects (`Result`, `FriendFlags`, `LobbyType`,
`UGCQueryType`, and so on) contain the numeric Steam constants expected by
methods. Do not hard-code their numeric values.

## Return conventions

- `boolean` means Steam accepted or completed a synchronous operation; inspect
  callback events or later state when Steamworks documents asynchronous work.
- `null` means a valid absence or lookup miss, for example no cloud file,
  image, installed item, lobby entry, or available P2P packet.
- Empty `string` is used where Steam returns no string value, notably missing
  lobby/friend rich-presence keys.
- A rejected `Promise` represents Steam call-result failure or an IO failure;
  errors include the Steam result code when one is available.
