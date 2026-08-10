#pragma once

#include "common.hpp"

namespace steam_api::networking {
JS_METHOD(sendP2PPacket);
JS_METHOD(isP2PPacketAvailable);
JS_METHOD(readP2PPacket);
JS_METHOD(acceptP2PSessionWithUser);
JS_METHOD(getP2PSessionState);
JS_METHOD(closeP2PSessionWithUser);
JS_METHOD(closeP2PChannelWithUser);
JS_METHOD(isBehindNat);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::networking
