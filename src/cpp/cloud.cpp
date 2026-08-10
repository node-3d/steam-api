#include "cloud.hpp"

#include <filesystem>
#include <fstream>
#include <limits>

namespace steam_api::cloud {
ISteamRemoteStorage *steamRemoteStorage(Napi::Env env) {
	ISteamRemoteStorage *value = SteamRemoteStorage();
	if (value == nullptr) {
		JS_THROW("SteamRemoteStorage is not available. Call steam.initEx() first.");
	}
	return value;
}

std::string fileNameFromPath(const std::string &value) {
	size_t pos = value.find_last_of("/\\");
	if (pos == std::string::npos) {
		return value;
	}
	return value.substr(pos + 1);
}

bool readFileContent(Napi::Env env, const std::string &path, std::vector<char> *content) {
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input.is_open()) {
		JS_THROW("Steam Cloud source file could not be opened: " + path);
		return false;
	}

	std::ifstream::pos_type fileSize = input.tellg();
	if (fileSize < 0 || fileSize > static_cast<std::ifstream::pos_type>(std::numeric_limits<int32>::max())) {
		JS_THROW("Steam Cloud source file is too large: " + path);
		return false;
	}

	content->resize(static_cast<size_t>(fileSize));
	input.seekg(0, std::ios::beg);
	if (!content->empty()) {
		input.read(content->data(), fileSize);
	}

	if (!input.good()) {
		JS_THROW("Steam Cloud source file could not be read: " + path);
		return false;
	}

	return true;
}

JS_METHOD(saveTextToFile) {
	NAPI_ENV;
	REQ_STR_ARG(0, fileName);
	REQ_STR_ARG(1, content);

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(remoteStorage->FileWrite(fileName.c_str(), content.data(), static_cast<int32>(content.size())));
}

JS_METHOD(readTextFromFile) {
	NAPI_ENV;
	REQ_STR_ARG(0, fileName);

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	if (!remoteStorage->FileExists(fileName.c_str())) {
		RET_NULL;
	}

	int32 fileSize = remoteStorage->GetFileSize(fileName.c_str());
	if (fileSize < 0) {
		RET_NULL;
	}

	std::vector<char> content(static_cast<size_t>(fileSize));
	int32 bytesRead = remoteStorage->FileRead(fileName.c_str(), content.data(), fileSize);
	if (bytesRead < 0 || bytesRead != fileSize) {
		RET_NULL;
	}

	RET_VALUE(Napi::String::New(env, content.data(), content.size()));
}

JS_METHOD(deleteFile) {
	NAPI_ENV;
	REQ_STR_ARG(0, fileName);

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	if (!remoteStorage->FileExists(fileName.c_str())) {
		RET_BOOL(false);
	}
	RET_BOOL(remoteStorage->FileDelete(fileName.c_str()));
}

JS_METHOD(saveFilesToCloud) {
	NAPI_ENV;

	if (info.Length() < 1 || !info[0].IsArray()) {
		JS_THROW("filePaths must be an array of strings.");
		RET_UNDEFINED;
	}

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	Napi::Array filePaths = info[0].As<Napi::Array>();
	Napi::Array files = Napi::Array::New(env);
	uint32 fileCount = 0;

	for (uint32 i = 0; i < filePaths.Length(); i++) {
		Napi::Value item = filePaths.Get(i);
		if (!item.IsString()) {
			JS_THROW("filePaths must contain only strings.");
			RET_UNDEFINED;
		}

		std::string path = item.As<Napi::String>().Utf8Value();
		if (path.empty()) {
			continue;
		}

		std::string fileName = fileNameFromPath(path);
		if (fileName.empty()) {
			JS_THROW("filePaths entries must include file names.");
			RET_UNDEFINED;
		}

		std::vector<char> content;
		if (!readFileContent(env, path, &content)) {
			RET_UNDEFINED;
		}

		if (!remoteStorage->FileWrite(
		        fileName.c_str(),
		        content.empty() ? nullptr : content.data(),
		        static_cast<int32>(content.size())
		    )) {
			JS_THROW("Steam Cloud file could not be written: " + fileName);
			RET_UNDEFINED;
		}

		files.Set(fileCount, fileName);
		fileCount++;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("files", files);
	RET_VALUE(result);
}

JS_METHOD(isEnabledForUser) {
	NAPI_ENV;
	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(remoteStorage->IsCloudEnabledForAccount());
}

JS_METHOD(isEnabled) {
	NAPI_ENV;
	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(remoteStorage->IsCloudEnabledForApp());
}

JS_METHOD(setEnabled) {
	NAPI_ENV;
	REQ_BOOL_ARG(0, enabled);

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	remoteStorage->SetCloudEnabledForApp(enabled);
	RET_UNDEFINED;
}

JS_METHOD(getQuota) {
	NAPI_ENV;
	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint64 totalBytes = 0;
	uint64 availableBytes = 0;
	if (!remoteStorage->GetQuota(&totalBytes, &availableBytes)) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("totalBytes", jsStringFromUint64(env, totalBytes));
	result.Set("availableBytes", jsStringFromUint64(env, availableBytes));
	RET_VALUE(result);
}

JS_METHOD(getFileCount) {
	NAPI_ENV;
	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(remoteStorage->GetFileCount());
}

JS_METHOD(getFileNameAndSize) {
	NAPI_ENV;
	REQ_INT32_ARG(0, index);

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	int32 fileSize = 0;
	const char *fileName = remoteStorage->GetFileNameAndSize(index, &fileSize);
	if (fileName == nullptr) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("name", fileName);
	result.Set("size", fileSize);
	RET_VALUE(result);
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("saveTextToFile", Napi::Function::New(env, saveTextToFile));
	value.Set("readTextFromFile", Napi::Function::New(env, readTextFromFile));
	value.Set("deleteFile", Napi::Function::New(env, deleteFile));
	value.Set("saveFilesToCloud", Napi::Function::New(env, saveFilesToCloud));
	value.Set("isEnabledForUser", Napi::Function::New(env, isEnabledForUser));
	value.Set("isEnabled", Napi::Function::New(env, isEnabled));
	value.Set("setEnabled", Napi::Function::New(env, setEnabled));
	value.Set("getQuota", Napi::Function::New(env, getQuota));
	value.Set("getFileCount", Napi::Function::New(env, getFileCount));
	value.Set("getFileNameAndSize", Napi::Function::New(env, getFileNameAndSize));
	return value;
}
} // namespace steam_api::cloud
