#pragma once

#include <addon-tools.hpp>
#include <steam/steam_api.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace steam_api {
inline Napi::String jsStringFromUint64(Napi::Env env, uint64 value) {
	return Napi::String::New(env, std::to_string(value));
}

inline Napi::Object makeEvent(Napi::Env env, const std::string &type) {
	Napi::Object event = JS_OBJECT;
	event.Set("type", type);
	return event;
}
} // namespace steam_api
