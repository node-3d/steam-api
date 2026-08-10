#pragma once

#include "common.hpp"

namespace steam_api::steam_id {
JS_METHOD(isAnonymous);
JS_METHOD(isAnonymousGameServer);
JS_METHOD(isAnonymousGameServerLogin);
JS_METHOD(isAnonymousUser);
JS_METHOD(isChatAccount);
JS_METHOD(isClanAccount);
JS_METHOD(isConsoleUserAccount);
JS_METHOD(isContentServerAccount);
JS_METHOD(isGameServerAccount);
JS_METHOD(isIndividualAccount);
JS_METHOD(isPersistentGameServerAccount);
JS_METHOD(isLobby);
JS_METHOD(getAccountId);
JS_METHOD(getRawSteamId);
JS_METHOD(getAccountType);
JS_METHOD(isValid);
JS_METHOD(getStaticAccountKey);
JS_METHOD(getPersonaName);
JS_METHOD(getNickname);
JS_METHOD(getRelationship);
JS_METHOD(getSteamLevel);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::steam_id
