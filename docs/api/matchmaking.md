# Lobbies

Official reference: [ISteamMatchmaking](https://partner.steamgames.com/doc/api/ISteamMatchmaking).

Lobby creation, joining, and listing use Steam call results, so await them with
callback pumping. The returned objects contain the lobby ID and Steam result.

```ts
const { lobbyId } = await waitForSteamCall(
	matchmaking.createLobby(LobbyType.Public, 4),
);

matchmaking.setLobbyData(lobbyId, 'mode', 'co-op');
matchmaking.setLobbyJoinable(lobbyId, true);
```

`joinLobby(lobbyId)` and `requestLobbyList()` follow the same promise rule.
Related callback events include `lobby-created`, `lobby-enter`,
`lobby-match-list`, `lobby-invite`, and `lobby-join-requested`.

## Synchronous lobby state

`getLobbyByIndex()`, `getLobbyMemberByIndex()`, `getLobbyOwner()`, and
`getLobbyDataByIndex()` return `null` for an invalid or missing index. The
string-returning `getLobbyData()` and `getLobbyMemberData()` use an empty string
for an absent key. Update fields with `setLobbyData()`, `deleteLobbyData()`,
`setLobbyOwner()`, `setLobbyType()`, `setLobbyJoinable()`, and
`setLobbyMemberLimit()`; methods returning `boolean` report whether Steam
accepted the change.

`getNumLobbyMembers()` and `getLobbyMemberLimit()` provide bounds for indexed
reads. Use `leaveLobby(lobbyId)` when the session ends.

## Lobby chat

Send binary data with `sendLobbyChatMsg(lobbyId, Buffer.from(...))`. On a
`lobby-chat-msg` event, use the supplied `chatId` with
`getLobbyChatEntry(lobbyId, chatId, maximumMessageSize?)`. It returns `null`
when Steam cannot read that entry; its `data` field is a Node.js `Buffer`.

