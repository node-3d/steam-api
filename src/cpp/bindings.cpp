#include "apps.hpp"
#include "callbacks.hpp"
#include "cloud.hpp"
#include "common.hpp"
#include "enums.hpp"
#include "friends.hpp"
#include "matchmaking.hpp"
#include "networking.hpp"
#include "steam-api.hpp"
#include "steam-id.hpp"
#include "ugc.hpp"
#include "user-stats.hpp"
#include "user.hpp"
#include "utils.hpp"

#include <steam/steamencryptedappticket.h>

#define JS_STEAM_CONSTANT(name) exports.Set(#name, static_cast<int32_t>(name));

void cleanupModule() {
	steam_api::ugc::clearPendingPromises();
	steam_api::matchmaking::clearPendingPromises();
	steam_api::user::clearPendingPromises();
	steam_api::user_stats::clearPendingPromises();
	steam_api::callbacks::unregisterBridge();
	if (steam_api::isInitialized()) {
		SteamAPI_Shutdown();
		steam_api::setInitialized(false);
	}
}

Napi::Object initModule(Napi::Env env, Napi::Object exports) {
	env.AddCleanupHook(cleanupModule);
	exports.Set("steam", steam_api::steam::createNamespace(env));
	exports.Set("callbacks", steam_api::callbacks::createNamespace(env));
	exports.Set("steamId", steam_api::steam_id::createNamespace(env));
	exports.Set("user", steam_api::user::createNamespace(env));
	exports.Set("utils", steam_api::utils::createNamespace(env));
	exports.Set("apps", steam_api::apps::createNamespace(env));
	exports.Set("friends", steam_api::friends::createNamespace(env));
	exports.Set("matchmaking", steam_api::matchmaking::createNamespace(env));
	exports.Set("networking", steam_api::networking::createNamespace(env));
	exports.Set("cloud", steam_api::cloud::createNamespace(env));
	exports.Set("ugc", steam_api::ugc::createNamespace(env));
	exports.Set("userStats", steam_api::user_stats::createNamespace(env));
	steam_api::enums::setEnumExports(env, exports);

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
	exports.Set("EncryptedAppTicketSymmetricKeyLength", k_nSteamEncryptedAppTicketSymmetricKeyLen);

	return exports;
}

NODE_API_MODULE(steam_api, initModule)
