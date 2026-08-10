#pragma once

#include "common.hpp"

namespace steam_api::cloud {
JS_METHOD(saveTextToFile);
JS_METHOD(readTextFromFile);
JS_METHOD(deleteFile);
JS_METHOD(saveFilesToCloud);
JS_METHOD(isEnabledForUser);
JS_METHOD(isEnabled);
JS_METHOD(setEnabled);
JS_METHOD(getQuota);
JS_METHOD(getFileCount);
JS_METHOD(getFileNameAndSize);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::cloud
