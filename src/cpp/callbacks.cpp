#include "callbacks.hpp"

namespace steam_api::callbacks {
struct Event {
	std::string type;
	uint64 gameId = 0;
	uint64 userId = 0;
	std::string name;
	int32_t result = 0;
	uint32_t currentProgress = 0;
	uint32_t maxProgress = 0;
};

class CallbackBridge {
  public:
	CallbackBridge() = default;

	void registerAll() {
		if (_isRegistered) {
			return;
		}

		_userStatsReceived.Register(this, &CallbackBridge::onUserStatsReceived);
		_userStatsStored.Register(this, &CallbackBridge::onUserStatsStored);
		_userAchievementStored.Register(this, &CallbackBridge::onUserAchievementStored);
		_authTicketResponse.Register(this, &CallbackBridge::onAuthTicketResponse);
		_isRegistered = true;
	}

	void unregisterAll() {
		if (!_isRegistered) {
			return;
		}

		_userStatsReceived.Unregister();
		_userStatsStored.Unregister();
		_userAchievementStored.Unregister();
		_authTicketResponse.Unregister();
		_isRegistered = false;
		_events.clear();
	}

	Napi::Array poll(Napi::Env env) {
		Napi::Array events = Napi::Array::New(env, _events.size());

		for (size_t i = 0; i < _events.size(); i++) {
			const Event &source = _events[i];
			Napi::Object event = makeEvent(env, source.type);

			if (source.gameId != 0) {
				event.Set("gameId", jsStringFromUint64(env, source.gameId));
			}
			if (source.userId != 0) {
				event.Set("userId", jsStringFromUint64(env, source.userId));
			}
			if (!source.name.empty()) {
				event.Set("name", source.name);
			}
			event.Set("result", source.result);
			event.Set("currentProgress", source.currentProgress);
			event.Set("maxProgress", source.maxProgress);

			events.Set(i, event);
		}

		_events.clear();
		return events;
	}

  private:
	void onUserStatsReceived(UserStatsReceived_t *param) {
		Event event;
		event.type = "userStatsReceived";
		event.gameId = param->m_nGameID;
		event.userId = param->m_steamIDUser.ConvertToUint64();
		event.result = static_cast<int32_t>(param->m_eResult);
		_events.push_back(event);
	}

	void onUserStatsStored(UserStatsStored_t *param) {
		Event event;
		event.type = "userStatsStored";
		event.gameId = param->m_nGameID;
		event.result = static_cast<int32_t>(param->m_eResult);
		_events.push_back(event);
	}

	void onUserAchievementStored(UserAchievementStored_t *param) {
		Event event;
		event.type = "userAchievementStored";
		event.gameId = param->m_nGameID;
		event.name = param->m_rgchAchievementName;
		event.currentProgress = param->m_nCurProgress;
		event.maxProgress = param->m_nMaxProgress;
		_events.push_back(event);
	}

	void onAuthTicketResponse(GetAuthSessionTicketResponse_t *param) {
		Event event;
		event.type = "authSessionTicketResponse";
		event.result = static_cast<int32_t>(param->m_eResult);
		event.currentProgress = static_cast<uint32_t>(param->m_hAuthTicket);
		_events.push_back(event);
	}

	bool _isRegistered = false;
	std::vector<Event> _events;
	CCallbackManual<CallbackBridge, UserStatsReceived_t> _userStatsReceived;
	CCallbackManual<CallbackBridge, UserStatsStored_t> _userStatsStored;
	CCallbackManual<CallbackBridge, UserAchievementStored_t> _userAchievementStored;
	CCallbackManual<CallbackBridge, GetAuthSessionTicketResponse_t> _authTicketResponse;
};

std::unique_ptr<CallbackBridge> callbackBridge;

void ensureBridge() {
	if (!callbackBridge) {
		callbackBridge = std::make_unique<CallbackBridge>();
	}
	callbackBridge->registerAll();
}

void unregisterBridge() {
	if (callbackBridge) {
		callbackBridge->unregisterAll();
	}
}

JS_METHOD(pollCallbacks) {
	NAPI_ENV;
	if (!callbackBridge) {
		RET_VALUE(JS_ARRAY);
	}
	RET_VALUE(callbackBridge->poll(env));
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("pollCallbacks", Napi::Function::New(env, pollCallbacks));
	return value;
}
} // namespace steam_api::callbacks
