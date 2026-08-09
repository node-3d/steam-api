#pragma once

#include "common.hpp"

namespace steam_api::utils {
JS_METHOD(getAppId);
JS_METHOD(getConnectedUniverse);
JS_METHOD(getIpCountry);
JS_METHOD(getServerRealTime);
JS_METHOD(getSteamUiLanguage);
JS_METHOD(getImageSize);
JS_METHOD(getImageRgba);
JS_METHOD(isOverlayEnabled);
JS_METHOD(isSteamInBigPictureMode);
JS_METHOD(isSteamRunningOnSteamDeck);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::utils
