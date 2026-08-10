#pragma once

#include "common.hpp"

namespace steam_api::ugc {
JS_METHOD(showOverlay);
JS_METHOD(getItems);
JS_METHOD(getUserItems);
JS_METHOD(downloadItem);
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
