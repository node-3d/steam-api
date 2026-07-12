#pragma once

#include "common.hpp"

namespace steam_api::callbacks {
JS_METHOD(pollCallbacks);

void ensureBridge();
void unregisterBridge();
Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::callbacks
