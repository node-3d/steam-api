#pragma once

#include "common.hpp"

namespace steam_api::steam {
JS_METHOD(restartAppIfNecessary);
JS_METHOD(initEx);
JS_METHOD(shutdown);
JS_METHOD(isSteamRunning);
JS_METHOD(runCallbacks);
JS_METHOD(releaseCurrentThreadMemory);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::steam

namespace steam_api {
bool isInitialized();
void setInitialized(bool value);
} // namespace steam_api
