#pragma once

#include "common.hpp"

namespace steam_api::user {
JS_METHOD(getHSteamUser);
JS_METHOD(isLoggedOn);
JS_METHOD(getSteamId);
JS_METHOD(getAuthSessionTicket);
JS_METHOD(getAuthTicketForWebApi);
JS_METHOD(cancelAuthTicket);
JS_METHOD(beginAuthSession);
JS_METHOD(endAuthSession);
JS_METHOD(requestEncryptedAppTicket);
JS_METHOD(getEncryptedAppTicket);
JS_METHOD(decryptAppTicket);
JS_METHOD(isTicketForApp);
JS_METHOD(getTicketIssueTime);
JS_METHOD(getTicketSteamId);
JS_METHOD(getTicketAppId);

void handleGetTicketForWebApiResponse(GetTicketForWebApiResponse_t *param);
void rejectPendingPromises(const std::string &message);
void clearPendingPromises();

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::user
