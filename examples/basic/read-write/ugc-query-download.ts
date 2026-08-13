import { UGCMatchingType, UGCQueryType, ugc, utils } from '@node-3d/steam-api';
import {
	hasFlag,
	initSteam,
	listOption,
	numberOption,
	option,
	waitForSteamCall,
} from '../helpers.ts';

initSteam();

const queryOptions = {
	appId: numberOption('app-id', utils.getAppId()),
	page: numberOption('page', 1),
	requiredTags: listOption('tags'),
	excludedTags: listOption('excluded-tags'),
	matchAnyTag: hasFlag('match-any-tag'),
	returnMetadata: true,
	returnAdditionalPreviews: true,
	returnChildren: true,
	returnKeyValueTags: true,
	allowCachedResponseMaxAgeSeconds: numberOption('cache-seconds', 60),
};
const query = ugc.getItems(
	queryOptions,
	UGCMatchingType.Items,
	UGCQueryType.RankedByPublicationDate,
);
const result = await waitForSteamCall(query);

console.log('UGC query:', {
	result: result.result,
	totalMatchingResults: result.totalMatchingResults,
	cachedData: result.cachedData,
	nextCursor: result.nextCursor,
	items: result.items.length,
});

for (const item of result.items.slice(0, numberOption('limit', 10))) {
	console.log({
		publishedFileId: item.publishedFileId,
		title: item.title,
		owner: item.steamIdOwner,
		tags: item.tags,
		votesUp: item.votesUp,
		votesDown: item.votesDown,
		metadata: item.metadata,
		additionalPreviews: item.additionalPreviews,
		keyValueTags: item.keyValueTags,
		state: ugc.getItemState(item.publishedFileId),
		installInfo: ugc.getItemInstallInfo(item.publishedFileId),
		downloadInfo: ugc.getItemDownloadInfo(item.publishedFileId),
	});
}

const first = result.items[0];
const downloadDir = option('download-dir');
if (first && downloadDir) {
	const download = await waitForSteamCall(ugc.download(first.file, downloadDir));
	console.log('Downloaded legacy UGC file:', download);
}

if (first && hasFlag('subscribe-first')) {
	console.log('Subscribe:', await waitForSteamCall(ugc.subscribeItem(first.publishedFileId)));
}

if (first && hasFlag('vote-first')) {
	console.log('Vote:', await waitForSteamCall(ugc.setUserItemVote(first.publishedFileId, true)));
	console.log('Vote state:', await waitForSteamCall(ugc.getUserItemVote(first.publishedFileId)));
}

if (first && hasFlag('favorite-first')) {
	console.log('Favorite:', await waitForSteamCall(ugc.addItemToFavorites(first.publishedFileId)));
}

if (first && hasFlag('download-item-first')) {
	console.log('Download item requested:', ugc.downloadItem(first.publishedFileId, true));
}

if (first && hasFlag('unsubscribe-first')) {
	console.log('Unsubscribe:', await waitForSteamCall(ugc.unsubscribeItem(first.publishedFileId)));
}
