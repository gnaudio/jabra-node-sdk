#pragma once

#include "stdafx.h"

Napi::Value napi_GetBatteryStatus(const Napi::CallbackInfo& info);
Napi::Value napi_IsBatteryStatusSupported(const Napi::CallbackInfo& info);
Napi::Value napi_GetRemoteControlBatteryStatus(const Napi::CallbackInfo& info);
