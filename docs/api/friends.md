# Friends and presence

Official reference: [ISteamFriends](https://partner.steamgames.com/doc/api/ISteamFriends).

Use the exported `FriendFlags` constants with `getFriendCount(flags)` and
`getFriends(flags)`. Friend IDs are decimal SteamID strings. Persona and avatar
data can arrive asynchronously: call
`requestUserInformation(steamId, requireNameOnly)` and react to
`persona-state-change` or `avatar-image-loaded` events before relying on
cached fields.

```ts
for (const steamId of friends.getFriends(FriendFlags.Immediate)) {
	friends.requestUserInformation(steamId, true);
	console.log(friends.getFriendPersonaName(steamId));
}
```

`getSmallFriendAvatar()`, `getMediumFriendAvatar()`, and
`getLargeFriendAvatar()` return numeric image handles. A negative handle means
the image is not ready; use `utils.getImageSize()` and `utils.getImageRgba()`
only after an image handle is available.

## Overlay, chat, and presence

- Overlay methods (`activateGameOverlay*`, `activateGameOverlayInviteDialog`,
  and `activateGameOverlayToUser`) request UI display and return immediately.
- Enable incoming friend chat with `setListenForFriendsMessage(true)`. On a
  `game-connected-friend-chat-message` event, read with
  `getFriendMessage(steamId, messageId, maximumMessageSize)`; it returns
  `null` if the entry is no longer available.
- `setRichPresence(key, value)` and `clearRichPresence()` change the local
  user's presence. Read a friend's string with `getFriendRichPresence()`.
  A `rich-presence-join-requested` event carries the join connection string.
- `getFriendGamePlayed()` returns `null` when the friend is not in a game.
  `setPlayedWith()` records a recent interaction for Steam social features.

