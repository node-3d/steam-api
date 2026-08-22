# Steam IDs

Official reference: [CSteamID](https://partner.steamgames.com/doc/api/steam_api#CSteamID).

All Steam identifiers are represented by decimal strings in JavaScript to avoid
loss of precision. Returned IDs carry branded TypeScript types; external IDs
must be supplied as their decimal-string representation.

`steamId` exposes classification and extraction helpers without requiring
native `CSteamID` objects:

- account-kind checks: `isAnonymous`, `isAnonymousGameServer`,
  `isAnonymousGameServerLogin`, `isAnonymousUser`, `isChatAccount`,
  `isClanAccount`, `isConsoleUserAccount`, `isContentServerAccount`,
  `isGameServerAccount`, `isIndividualAccount`,
  `isPersistentGameServerAccount`, and `isLobby`;
- structure and validation: `isValid`, `getRawSteamId`, `getAccountId`,
  `getAccountType`, and `getStaticAccountKey`;
- profile data: `getPersonaName`, `getNickname`, `getRelationship`, and
  `getSteamLevel`.

```ts
if (!steamId.isValid(value) || !steamId.isIndividualAccount(value)) {
	throw new Error('Expected an individual SteamID');
}

console.log(steamId.getAccountId(value), steamId.getPersonaName(value));
```

Persona/profile helpers follow Steam's normal cache rules. Request user
information through `friends.requestUserInformation()` and react to callback
events when current data matters. See [IDs and result values](ids-and-results.md)
for shared null/boolean/promise conventions.

