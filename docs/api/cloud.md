# Cloud files

Official reference: [ISteamRemoteStorage](https://partner.steamgames.com/doc/api/ISteamRemoteStorage).

`cloud` uses simple text-file helpers over the current user's Steam Cloud.
`saveTextToFile(fileName, content)` and `deleteFile(fileName)` return `boolean`
for the immediate Steam operation. `readTextFromFile(fileName)` returns the
stored UTF-8 string or `null` when the file does not exist or cannot be read.

```ts
if (!cloud.saveTextToFile('settings.json', JSON.stringify(settings))) {
	throw new Error('Steam Cloud write failed');
}

const saved = cloud.readTextFromFile('settings.json');
const settings = saved ? JSON.parse(saved) : defaultSettings;
```

`saveFilesToCloud(filePaths)` reads local files and writes them under their
file names. It returns `{ files }` for the successful cloud file names; use it
before `ugc.fileShare()` or the higher-level `ugc.publish()` helpers.

Check `isEnabled()` for the application's global Cloud setting and
`isEnabledForUser()` for the user's setting before treating Cloud as durable
storage. `getQuota()` returns `{ totalBytes, availableBytes }` or `null` if
Steam cannot provide quota. Enumerate stored files with `getFileCount()` and
`getFileNameAndSize(index)`; the latter returns `null` outside the valid range.

