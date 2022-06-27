#pragma once

#include "stdafx.h"

void toBTAddr(uint8_t *dest, const std::string& srcHex, size_t destSize);
std::string toBTAddrString(const uint8_t *src, size_t srcSize);

Napi::Value napi_ConnectBTDevice(const Napi::CallbackInfo& info);
Napi::Value napi_GetConnectedBTDeviceName(const Napi::CallbackInfo& info);
Napi::Value napi_ConnectNewDevice(const Napi::CallbackInfo& info);
Napi::Value napi_ConnectPairedDevice(const Napi::CallbackInfo& info);

Napi::Value napi_DisconnectBTDevice(const Napi::CallbackInfo& info);
Napi::Value napi_DisconnectPairedDevice(const Napi::CallbackInfo& info);

Napi::Value napi_GetAutoPairing(const Napi::CallbackInfo& info);
Napi::Value napi_SetAutoPairing(const Napi::CallbackInfo& info);
Napi::Value napi_IsPairingListSupported(const Napi::CallbackInfo& info);
Napi::Value napi_GetPairingList(const Napi::CallbackInfo& info);
Napi::Value napi_ClearPairingList(const Napi::CallbackInfo& info);

Napi::Value napi_SetBTPairing(const Napi::CallbackInfo& info);
Napi::Value napi_StopBTPairing(const Napi::CallbackInfo& info);

Napi::Value napi_SearchNewDevices(const Napi::CallbackInfo& info);
Napi::Value napi_ClearPairedDevice(const Napi::CallbackInfo& info);
Napi::Value napi_GetSearchDeviceList(const Napi::CallbackInfo& info);

Napi::Value napi_BTLinkQualityChangeEventEnabled(const Napi::CallbackInfo& info);
