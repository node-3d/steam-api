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
JS_METHOD(showFloatingGamepadTextInput);
JS_METHOD(dismissFloatingGamepadTextInput);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::utils
