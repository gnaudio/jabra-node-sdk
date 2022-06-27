#pragma once

#include "stdafx.h"

Napi::Value napi_GetConnectedHeadsetNames(const Napi::CallbackInfo& info);
Napi::Value napi_TriggerDECTPairing(const Napi::CallbackInfo& info);
Napi::Value napi_TriggerDECTSecurePairing(const Napi::CallbackInfo& info);
Napi::Value napi_GetDECTPairingKey(const Napi::CallbackInfo& info);
Napi::Value napi_SetDECTPairingKey(const Napi::CallbackInfo& info);
