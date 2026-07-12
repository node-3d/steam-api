#include "apps.hpp"
#include "callbacks.hpp"
#include "common.hpp"
#include "steam-api.hpp"
#include "user-stats.hpp"
#include "user.hpp"
#include "utils.hpp"

#define JS_STEAM_CONSTANT(name) exports.Set(#name, static_cast<int32_t>(name));

Napi::Object initModule(Napi::Env env, Napi::Object exports) {
	exports.Set("steam", steam_api::steam::createNamespace(env));
	exports.Set("callbacks", steam_api::callbacks::createNamespace(env));
	exports.Set("user", steam_api::user::createNamespace(env));
	exports.Set("utils", steam_api::utils::createNamespace(env));
	exports.Set("apps", steam_api::apps::createNamespace(env));
	exports.Set("userStats", steam_api::user_stats::createNamespace(env));

	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_OK);
	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_FailedGeneric);
	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_NoSteamClient);
	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_VersionMismatch);
	JS_STEAM_CONSTANT(k_EResultOK);
	JS_STEAM_CONSTANT(k_EResultFail);
	JS_STEAM_CONSTANT(k_EUniverseInvalid);
	JS_STEAM_CONSTANT(k_EUniversePublic);
	JS_STEAM_CONSTANT(k_EUniverseBeta);
	JS_STEAM_CONSTANT(k_EUniverseInternal);
	JS_STEAM_CONSTANT(k_EUniverseDev);

	return exports;
}

NODE_API_MODULE(steam_api, initModule)
