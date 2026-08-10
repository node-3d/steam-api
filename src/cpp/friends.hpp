#pragma once

#include "common.hpp"

namespace steam_api::friends {
JS_METHOD(activateGameOverlay);
JS_METHOD(activateGameOverlayToWebPage);
JS_METHOD(activateGameOverlayToStore);
JS_METHOD(getFriendCount);
JS_METHOD(getFriends);
JS_METHOD(requestUserInformation);
JS_METHOD(getSmallFriendAvatar);
JS_METHOD(getMediumFriendAvatar);
JS_METHOD(getLargeFriendAvatar);
JS_METHOD(setListenForFriendsMessage);
JS_METHOD(replyToFriendMessage);
JS_METHOD(getFriendMessage);
JS_METHOD(getFriendPersonaName);
JS_METHOD(setRichPresence);
JS_METHOD(clearRichPresence);
JS_METHOD(getFriendRichPresence);
JS_METHOD(setPlayedWith);
JS_METHOD(getFriendGamePlayed);
JS_METHOD(activateGameOverlayInviteDialog);
JS_METHOD(activateGameOverlayToUser);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::friends
