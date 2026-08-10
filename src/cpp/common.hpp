#pragma once

#include <addon-tools.hpp>
#include <steam/steam_api.h>

#include <charconv>
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace steam_api {
inline Napi::String jsStringFromUint64(Napi::Env env, uint64 value) {
	return Napi::String::New(env, std::to_string(value));
}

inline bool uint64FromJsString(Napi::Env env, const std::string &source, const char *name, uint64 *target) {
	if (source.empty()) {
		JS_THROW(std::string(name) + " must be a non-empty uint64 decimal string.");
		return false;
	}

	uint64 value = 0;
	auto [ptr, error] = std::from_chars(source.data(), source.data() + source.size(), value);
	if (error == std::errc::result_out_of_range) {
		JS_THROW(std::string(name) + " is outside the uint64 range.");
		return false;
	}
	if (error != std::errc() || ptr != source.data() + source.size()) {
		JS_THROW(std::string(name) + " must be a uint64 decimal string.");
		return false;
	}

	*target = value;
	return true;
}

inline Napi::Object makeEvent(Napi::Env env, const std::string &type) {
	Napi::Object event = JS_OBJECT;
	event.Set("type", type);
	return event;
}
} // namespace steam_api
