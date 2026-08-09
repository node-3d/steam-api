#include "friends.hpp"

namespace steam_api::friends {
ISteamFriends *steamFriends(Napi::Env env) {
	ISteamFriends *value = SteamFriends();
	if (value == nullptr) {
		JS_THROW("SteamFriends is not available. Call steam.initEx() first.");
	}
	return value;
}

JS_METHOD(activateGameOverlay) {
	NAPI_ENV;
	REQ_STR_ARG(0, dialog);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlay(dialog.c_str());
	RET_UNDEFINED;
}

JS_METHOD(activateGameOverlayToWebPage) {
	NAPI_ENV;
	REQ_STR_ARG(0, url);
	USE_INT32_ARG(1, mode, k_EActivateGameOverlayToWebPageMode_Default);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlayToWebPage(url.c_str(), static_cast<EActivateGameOverlayToWebPageMode>(mode));
	RET_UNDEFINED;
}

JS_METHOD(activateGameOverlayToStore) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	REQ_INT32_ARG(1, flag);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlayToStore(appId, static_cast<EOverlayToStoreFlag>(flag));
	RET_UNDEFINED;
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("activateGameOverlay", Napi::Function::New(env, activateGameOverlay));
	value.Set("activateGameOverlayToWebPage", Napi::Function::New(env, activateGameOverlayToWebPage));
	value.Set("activateGameOverlayToStore", Napi::Function::New(env, activateGameOverlayToStore));
	return value;
}
} // namespace steam_api::friends
