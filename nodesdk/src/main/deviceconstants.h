#include "stdafx.h"

Napi::Value napi_GetConstSync(const Napi::CallbackInfo& info);
Napi::Value napi_GetConstStringSync(const Napi::CallbackInfo& info);
Napi::Value napi_GetConstIntegerSync(const Napi::CallbackInfo& info);
Napi::Value napi_GetConstBooleanSync(const Napi::CallbackInfo& info);
Napi::Value napi_GetConstFieldSync(const Napi::CallbackInfo& info);
Napi::Value napi_GetConstListSync(const Napi::CallbackInfo& info);
void freeConstants(unsigned short deviceId);
void freeConstants();
