#pragma once

#include "common.hpp"

namespace steam_api::matchmaking {
JS_METHOD(createLobby);
JS_METHOD(deleteLobbyData);
JS_METHOD(getLobbyByIndex);
JS_METHOD(getLobbyData);
JS_METHOD(getLobbyMemberByIndex);
JS_METHOD(getNumLobbyMembers);
JS_METHOD(getLobbyOwner);
JS_METHOD(inviteUserToLobby);
JS_METHOD(joinLobby);
JS_METHOD(leaveLobby);
JS_METHOD(setLobbyData);
JS_METHOD(setLobbyJoinable);
JS_METHOD(setLobbyOwner);
JS_METHOD(setLobbyType);
JS_METHOD(requestLobbyList);
JS_METHOD(getLobbyMemberLimit);
JS_METHOD(setLobbyMemberLimit);
JS_METHOD(getLobbyMemberData);
JS_METHOD(setLobbyMemberData);
JS_METHOD(getLobbyDataCount);
JS_METHOD(getLobbyDataByIndex);
JS_METHOD(sendLobbyChatMsg);
JS_METHOD(getLobbyChatEntry);

void rejectPendingPromises(const std::string &message);
void clearPendingPromises();

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::matchmaking
