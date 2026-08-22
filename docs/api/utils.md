# Utilities and overlay

Official reference: [ISteamUtils](https://partner.steamgames.com/doc/api/ISteamUtils).

`utils` methods are synchronous. Use `getAppId()`, `getConnectedUniverse()`,
`getIpCountry()`, `getServerRealTime()`, and `getSteamUiLanguage()` as runtime
context values, not persistent account data.

Image handles from friends callbacks or avatar lookup methods can be decoded
without native pointer access:

```ts
const image = friends.getMediumFriendAvatar(steamId);
const size = image >= 0 ? utils.getImageSize(image) : null;
const rgba = image >= 0 ? utils.getImageRgba(image) : null;

if (size && rgba) {
	console.log(size.width, size.height, rgba.length);
}
```

`getImageSize()` and `getImageRgba()` return `null` if the image is unavailable.
Check `isOverlayEnabled()` before presenting overlay-dependent UI. Hardware and
mode helpers (`isSteamInBigPictureMode()`, `isSteamRunningOnSteamDeck()`,
`isRunningOnSteamHardware()`, and `getSteamHardwareDefaultConfig()`) return
Steam enum values; compare with the exported hardware constants.

`showFloatingGamepadTextInput(mode, x, y, width, height)` opens Steam's text
input UI and returns whether the request was accepted. Handle completion via
the `floating-gamepad-text-input-dismissed` callback; call
`dismissFloatingGamepadTextInput()` to close it programmatically.

