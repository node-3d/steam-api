#pragma once

#include "common.hpp"

namespace steam_api::ugc {
JS_METHOD(showOverlay);
JS_METHOD(getItems);
JS_METHOD(getUserItems);
JS_METHOD(createItem);
JS_METHOD(startItemUpdate);
JS_METHOD(setItemTitle);
JS_METHOD(setItemDescription);
JS_METHOD(setItemMetadata);
JS_METHOD(setItemVisibility);
JS_METHOD(setItemTags);
JS_METHOD(setItemContent);
JS_METHOD(setItemPreview);
JS_METHOD(submitItemUpdate);
JS_METHOD(getItemUpdateProgress);
JS_METHOD(setUserItemVote);
JS_METHOD(getUserItemVote);
JS_METHOD(addItemToFavorites);
JS_METHOD(removeItemFromFavorites);
JS_METHOD(subscribeItem);
JS_METHOD(unsubscribeItem);
JS_METHOD(getNumSubscribedItems);
JS_METHOD(getSubscribedItems);
JS_METHOD(getItemDownloadInfo);
JS_METHOD(downloadItem);
JS_METHOD(download);
JS_METHOD(unsubscribe);
JS_METHOD(saveFilesToCloud);
JS_METHOD(fileShare);
JS_METHOD(publishWorkshopFile);
JS_METHOD(updatePublishedWorkshopFile);
JS_METHOD(getItemState);
JS_METHOD(getItemInstallInfo);

void rejectPendingPromises(const std::string &message);
void clearPendingPromises();

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::ugc
