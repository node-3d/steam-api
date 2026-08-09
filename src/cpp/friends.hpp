#pragma once

#include "common.hpp"

namespace steam_api::friends {
JS_METHOD(activateGameOverlay);
JS_METHOD(activateGameOverlayToWebPage);
JS_METHOD(activateGameOverlayToStore);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::friends
