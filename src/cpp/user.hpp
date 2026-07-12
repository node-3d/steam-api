#pragma once

#include "common.hpp"

namespace steam_api::user {
JS_METHOD(getHSteamUser);
JS_METHOD(isLoggedOn);
JS_METHOD(getSteamId);
JS_METHOD(getAuthSessionTicket);
JS_METHOD(cancelAuthTicket);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::user
