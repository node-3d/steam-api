import { UGCMatchingType, UserUGCList, UserUGCListSortOrder, ugc, utils } from '@node-3d/steam-api';
import { initSteam, numberOption, waitForSteamCall } from '../helpers.ts';

initSteam();

const limit = numberOption('limit', 50);
const queryOptions = {
	appId: numberOption('app-id', utils.getAppId()),
	page: numberOption('page', 1),
	returnMetadata: true,
	returnAdditionalPreviews: true,
	returnKeyValueTags: true,
	allowCachedResponseMaxAgeSeconds: numberOption('cache-seconds', 60),
};

console.log('Subscribed item count:', ugc.getNumSubscribedItems());
console.log('Subscribed item ids:', ugc.getSubscribedItems(limit));

const query = await waitForSteamCall(
	ugc.getUserItems(
		queryOptions,
		UGCMatchingType.Items,
		UserUGCListSortOrder.SubscriptionDateDesc,
		UserUGCList.Subscribed,
	),
);

console.log('Subscribed query:', {
	result: query.result,
	totalMatchingResults: query.totalMatchingResults,
	cachedData: query.cachedData,
	items: query.items.length,
});

for (const item of query.items.slice(0, limit)) {
	console.log({
		publishedFileId: item.publishedFileId,
		title: item.title,
		tags: item.tags,
		state: ugc.getItemState(item.publishedFileId),
		installInfo: ugc.getItemInstallInfo(item.publishedFileId),
		downloadInfo: ugc.getItemDownloadInfo(item.publishedFileId),
		metadata: item.metadata,
	});
}
