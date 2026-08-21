# Steamworks Coverage Checklist

This checklist tracks `@node-3d/steam-api` coverage against Steamworks API
areas, not Greenworks compatibility. Greenworks migration notes live in
`GREENWORKS.md`.

## Current Milestone: Modern UGC and Workshop

These items are prioritized because they cover the modern `ISteamUGC` item
lifecycle, subscription/download flow, query filtering, and user item
interactions.

### Item Creation and Update Lifecycle

- [x] `ISteamUGC::CreateItem`
- [x] `ISteamUGC::StartItemUpdate`
- [x] `ISteamUGC::SetItemTitle`
- [x] `ISteamUGC::SetItemDescription`
- [x] `ISteamUGC::SetItemMetadata`
- [x] `ISteamUGC::SetItemVisibility`
- [x] `ISteamUGC::SetItemTags`
- [x] `ISteamUGC::SetItemContent`
- [x] `ISteamUGC::SetItemPreview`
- [x] `ISteamUGC::SubmitItemUpdate`
- [x] `ISteamUGC::GetItemUpdateProgress`

### Modern Subscription and Download

- [x] `ISteamUGC::SubscribeItem`
- [x] `ISteamUGC::UnsubscribeItem`
- [x] `ISteamUGC::GetNumSubscribedItems`
- [x] `ISteamUGC::GetSubscribedItems`
- [x] `ISteamUGC::GetItemDownloadInfo`
- [x] `ISteamUGC::DownloadItem`
- [x] `ISteamUGC::GetItemState`
- [x] `ISteamUGC::GetItemInstallInfo`

### User Item Interactions

- [x] `ISteamUGC::SetUserItemVote`
- [x] `ISteamUGC::GetUserItemVote`
- [x] `ISteamUGC::AddItemToFavorites`
- [x] `ISteamUGC::RemoveItemFromFavorites`

### Query Filtering and Return Controls

- [x] `ISteamUGC::AddRequiredTag`
- [x] `ISteamUGC::AddExcludedTag`
- [x] `ISteamUGC::AddRequiredTagGroup`
- [x] `ISteamUGC::AddRequiredKeyValueTag`
- [x] `ISteamUGC::SetMatchAnyTag`
- [x] `ISteamUGC::SetAdminQuery`
- [x] `ISteamUGC::SetReturnOnlyIDs`
- [x] `ISteamUGC::SetReturnMetadata`
- [x] `ISteamUGC::SetReturnLongDescription`
- [x] `ISteamUGC::SetReturnAdditionalPreviews`
- [x] `ISteamUGC::SetReturnChildren`
- [x] `ISteamUGC::SetReturnKeyValueTags`
- [x] `ISteamUGC::SetReturnPlaytimeStats`
- [x] `ISteamUGC::SetReturnTotalOnly`
- [x] `ISteamUGC::SetCloudFileNameFilter`
- [x] `ISteamUGC::SetRankedByTrendDays`
- [x] `ISteamUGC::SetSearchText`
- [x] `ISteamUGC::SetLanguage`
- [x] `ISteamUGC::SetAllowCachedResponse`
- [x] `ISteamUGC::GetQueryUGCStatistic`

### Existing Legacy and Greenworks-Oriented UGC

- [x] `ISteamUGC::CreateQueryAllUGCRequest`
- [x] `ISteamUGC::CreateQueryUserUGCRequest`
- [x] `ISteamUGC::CreateQueryUGCDetailsRequest`
- [x] `ISteamUGC::SendQueryUGCRequest`
- [x] `ISteamUGC::GetQueryUGCResult`
- [x] `ISteamUGC::RemoveAllItemKeyValueTags`
- [x] `ISteamUGC::RemoveItemKeyValueTags`
- [x] `ISteamUGC::AddItemKeyValueTag`
- [x] `ISteamUGC::AddDependency`
- [x] `ISteamUGC::RemoveDependency`
- [x] `ISteamUGC::ReleaseQueryUGCRequest`
- [x] `ISteamRemoteStorage::FileShare`
- [x] `ISteamRemoteStorage::PublishWorkshopFile`
- [x] `ISteamRemoteStorage::CreatePublishedFileUpdateRequest`
- [x] `ISteamRemoteStorage::UpdatePublishedFileFile`
- [x] `ISteamRemoteStorage::UpdatePublishedFilePreviewFile`
- [x] `ISteamRemoteStorage::UpdatePublishedFileTitle`
- [x] `ISteamRemoteStorage::UpdatePublishedFileDescription`
- [x] `ISteamRemoteStorage::UpdatePublishedFileVisibility`
- [x] `ISteamRemoteStorage::UpdatePublishedFileTags`
- [x] `ISteamRemoteStorage::UpdatePublishedFileSetChangeDescription`
- [x] `ISteamRemoteStorage::CommitPublishedFileUpdate`
- [x] `ISteamRemoteStorage::UGCDownload`
- [x] `ISteamRemoteStorage::UGCRead`
- [x] `ISteamRemoteStorage::UnsubscribePublishedFile`
