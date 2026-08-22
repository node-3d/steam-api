# Workshop and UGC

Official reference: [ISteamUGC](https://partner.steamgames.com/doc/api/ISteamUGC).

UGC methods separate into three Node.js patterns: query/call-result promises,
synchronous update-handle setters, and callback-event downloads. Pump callbacks
while any returned promise is pending. See [Lifecycle and callbacks](lifecycle.md)
for a reusable waiting helper.

## Queries

`getItems(options, matchingType, queryType)`,
`getUserItems(options, matchingType, sortOrder, list)`, and
`getItemsByIds(options, publishedFileIds)` resolve with a normalized query
result. The binding creates, configures, sends, reads, and releases Steam query
handles; consumers never own a native query handle.

Use query options for `appId`, `page`, tags and key-value filters, return-field
flags, language, cache age, and the query-specific options documented below.

- `getItems()` supports `matchAnyTag`, `searchText`, `rankedByTrendDays`, and a
  deep-pagination `cursor`. Do not combine `cursor` with `page`.
- `getUserItems()` supports `userId` for another public user and
  `cloudFileNameFilter`; without `userId`, it selects the current user.
- `getItemsByIds()` accepts one to 50 known published file IDs and return-field
  options; it does not support user or cursor options. Passing more than 50 IDs
  throws before Steam creates a query.

The result includes `items`, `totalMatchingResults`, `cachedData`, and
`nextCursor`. Optional fields such as metadata, children, previews, key-value
tags, and playtime stats are present only when their corresponding return option
was requested.

### Public users, known IDs, and cursors

Set `userId` to query another user's public items; omitting it retains the
current-user behavior. Use `getItemsByIds()` for known Workshop IDs.

```ts
const userItems = await waitForSteamCall(
	ugc.getUserItems(
		{ userId: otherSteamId, returnMetadata: true },
		UGCMatchingType.Items,
		UserUGCListSortOrder.CreationOrderDesc,
		UserUGCList.Published,
	),
);

const items = await waitForSteamCall(
	ugc.getItemsByIds({ returnChildren: true }, [itemIdA, itemIdB]),
);
```

For deep pagination, pass a non-empty `nextCursor` from one all-item result to
the next `getItems()` call. Steam returns `nextCursor: ''` on the final page;
that empty terminal value must not be passed as `cursor` because the binding
throws for an empty cursor. Use a truthiness check such as the one below.

```ts
let result = await waitForSteamCall(
	ugc.getItems(
		{ searchText: 'maps' },
		UGCMatchingType.Items,
		UGCQueryType.RankedByTextSearch,
	),
);

while (result.nextCursor) {
	result = await waitForSteamCall(
		ugc.getItems(
			{ cursor: result.nextCursor },
			UGCMatchingType.Items,
			UGCQueryType.RankedByTextSearch,
		),
	);
}
```

## Create and update an item

`createItem(appId?, fileType?)` resolves with a published file ID. Begin a
multi-step mutation with `startItemUpdate(appId, publishedFileId)`, which returns
an opaque decimal-string update handle. Queue updates with:

- content and presentation: `setItemTitle`, `setItemDescription`,
  `setItemMetadata`, `setItemVisibility`, `setItemTags`, `setItemContent`, and
  `setItemPreview`;
- key-value tags: `addItemKeyValueTag`, `removeItemKeyValueTags`, and
  `removeAllItemKeyValueTags`;
- progress and commit: `getItemUpdateProgress` and
  `submitItemUpdate(updateHandle, changeNote?)`.

Setters return whether Steam accepted the staged change. Nothing becomes public
until `submitItemUpdate()` resolves successfully. `removeItemKeyValueTags()`
removes every value for one key because that is Steamworks' granularity.

```ts
const updateHandle = ugc.startItemUpdate(appId, itemId);
ugc.removeItemKeyValueTags(updateHandle, 'mode');
ugc.addItemKeyValueTag(updateHandle, 'mode', 'survival');
ugc.addItemKeyValueTag(updateHandle, 'difficulty', 'hard');
await waitForSteamCall(ugc.submitItemUpdate(updateHandle, 'Update item metadata'));
```

Call `removeAllItemKeyValueTags(updateHandle)` to clear all staged key-value
tags before submitting the update.

`publish()` and `publishUpdate()` are convenience workflows: they save local
files to Cloud, share them, report optional `onProgress` stages, and invoke the
legacy Workshop publication call. Use the update-handle API when the full UGC
update surface is required.

## Subscription, favorites, votes, and dependencies

`setUserItemVote`, `getUserItemVote`, `addItemToFavorites`,
`removeItemFromFavorites`, `subscribeItem`, `unsubscribeItem`,
`addDependency`, and `removeDependency` return promises with Steam result
objects. `getNumSubscribedItems()` and `getSubscribedItems()` are synchronous;
the latter bounds allocation to Steam's reported count.

```ts
await waitForSteamCall(ugc.addDependency(parentItemId, childItemId));
await waitForSteamCall(ugc.removeDependency(parentItemId, childItemId));
```

`getItemState()`, `getItemInstallInfo()`, and `getItemDownloadInfo()` inspect
local state. The information methods return `null` when unavailable.
`downloadItem(publishedFileId, highPriority?)` starts a managed Workshop
download and returns `boolean`; wait for a `download-item-result` event instead
of awaiting a promise. `download(file, downloadDir)` downloads a UGC handle to
a local directory and resolves with the resulting path. `unsubscribe()` is the
legacy remote-storage unsubscribe promise. `showOverlay(publishedFileId?)`
opens the Workshop or item page.
