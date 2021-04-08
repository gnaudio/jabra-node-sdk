#include "stdafx.h"

Napi::Value napi_GetSetting(const Napi::CallbackInfo& info);
Napi::Value napi_GetSettingsNoFilter(const Napi::CallbackInfo& info);
Napi::Value napi_GetSettings(const Napi::CallbackInfo& info);
Napi::Value napi_SetSettings(const Napi::CallbackInfo& info);
Napi::Value napi_FactoryReset(const Napi::CallbackInfo& info);
Napi::Value napi_IsSettingProtectionEnabled(const Napi::CallbackInfo& info);
Napi::Value napi_IsUploadImageSupported(const Napi::CallbackInfo& info);
Napi::Value napi_IsUploadRingtoneSupported(const Napi::CallbackInfo& info);
Napi::Value napi_IsFactoryResetSupported(const Napi::CallbackInfo& info);
Napi::Value napi_GetFailedSettingNames(const Napi::CallbackInfo& info);