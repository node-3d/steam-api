# User and authentication

Official reference: [ISteamUser](https://partner.steamgames.com/doc/api/ISteamUser).

Use `getSteamId()` only after initialization; `isLoggedOn()` distinguishes a
usable Steam session from a merely loaded client. `getHSteamUser()` is the
numeric native handle for interoperability and is rarely needed in application
code.

## Session tickets

`getAuthSessionTicket()` returns `{ handle, ticket }` immediately, or `null`
when Steam cannot issue a ticket. Send `ticket` to the peer or service, retain
the `handle`, and call `cancelAuthTicket(handle)` when it is no longer valid.

`getAuthTicketForWebApi(identity?)` returns a promise. Pump callbacks while it
is pending; it rejects when Steam reports a non-OK response.

```ts
const { handle, ticket } = await waitForSteamCall(user.getAuthTicketForWebApi('match-service'));
try {
	await sendTicketToService(ticket);
} finally {
	user.cancelAuthTicket(handle);
}
```

To validate a remote session ticket, call
`beginAuthSession(ticket, remoteSteamId)` and compare the numeric result with
`BeginAuthSessionResult.OK`. Listen for the `validate-auth-ticket` event for the
final response, and always pair a successful session with
`endAuthSession(remoteSteamId)`.

## Encrypted app tickets

`requestEncryptedAppTicket(userData)` resolves with the encrypted ticket.
`getEncryptedAppTicket()` returns the latest ticket or `null`. Decrypt it with
`decryptAppTicket(encryptedTicket, key)`; inspect a non-null result using
`isTicketForApp()`, `getTicketIssueTime()`, `getTicketSteamId()`, and
`getTicketAppId()`. Keep the symmetric key outside the client application.

