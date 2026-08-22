# Apps

Official reference: [ISteamApps](https://partner.steamgames.com/doc/api/ISteamApps).

`apps` is synchronous and reads the running Steam application context.

- Ownership and installation: `isSubscribed()`, `isSubscribedApp(appId)`,
  `isAppInstalled(appId)`, and `isDlcInstalled(appId)`.
- DLC changes: `installDlc(appId)` and `uninstallDlc(appId)` request a state
  change. Observe the `dlc-installed` callback event before treating an install
  as ready.
- App metadata: `getAppBuildId()`, `getCurrentGameLanguage()`,
  `getAvailableGameLanguages()`, and `getLaunchCommandLine()`.
- Paths and indexed records: `getCurrentGameInstallDir()`,
  `getAppInstallDir(appId)`, and `getDlcDataByIndex(index)` return `null` when
  Steam has no value. Iterate indexes from `0` to `getDlcCount() - 1`.

```ts
for (let index = 0; index < apps.getDlcCount(); index++) {
	const dlc = apps.getDlcDataByIndex(index);
	if (dlc?.available && !apps.isDlcInstalled(dlc.appId)) {
		apps.installDlc(dlc.appId);
	}
}
```

Use the current app ID from `utils.getAppId()` rather than embedding it in
consumer code when the app may run under multiple Steam configurations.
