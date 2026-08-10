#pragma once

#include "common.hpp"

#include <algorithm>
#include <utility>

namespace steam_api {
template <typename T, typename P> class PendingCallResultRequest {
  public:
	explicit PendingCallResultRequest(Napi::Env env)
	    : _env(env), _deferred(Napi::Promise::Deferred::New(env)) {}

	Napi::Promise promise() {
		return _deferred.Promise();
	}

	void setCallResult(SteamAPICall_t call) {
		_callResult.Set(call, static_cast<T *>(this), &T::onCompleted);
	}

	bool isCompleted() const {
		return _isCompleted;
	}

	void rejectPending(const std::string &message) {
		if (_isCompleted) {
			return;
		}

		_isCompleted = true;
		_callResult.Cancel();
		Napi::HandleScope scope(_env);
		_deferred.Reject(Napi::Error::New(_env, message).Value());
	}

  protected:
	Napi::Env env() {
		return _env;
	}

	Napi::Promise::Deferred &deferred() {
		return _deferred;
	}

	void markCompleted() {
		_isCompleted = true;
	}

  private:
	Napi::Env _env;
	Napi::Promise::Deferred _deferred;
	bool _isCompleted = false;
	CCallResult<T, P> _callResult;
};

template <typename T> void cleanupCompletedCallResults(std::vector<std::unique_ptr<T>> &requests) {
	requests.erase(
	    std::remove_if(
	        requests.begin(),
	        requests.end(),
	        [](const std::unique_ptr<T> &request) { return request->isCompleted(); }
	    ),
	    requests.end()
	);
}

template <typename T, typename... Args>
Napi::Promise trackCallResult(
    Napi::Env env, SteamAPICall_t call, std::vector<std::unique_ptr<T>> &requests, Args &&...args
) {
	cleanupCompletedCallResults(requests);

	std::unique_ptr<T> request = std::make_unique<T>(env, std::forward<Args>(args)...);
	Napi::Promise promise = request->promise();
	request->setCallResult(call);
	requests.push_back(std::move(request));
	return promise;
}

template <typename T>
void rejectPendingCallResults(std::vector<std::unique_ptr<T>> &requests, const std::string &message) {
	for (const std::unique_ptr<T> &request : requests) {
		request->rejectPending(message);
	}
	requests.clear();
}

template <typename T> void clearPendingCallResults(std::vector<std::unique_ptr<T>> &requests) {
	requests.clear();
}
} // namespace steam_api
