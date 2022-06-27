#pragma once

#include "stdafx.h"

Napi::Value napi_GetFirmwareVersion(const Napi::CallbackInfo& info);
Napi::Value napi_GetLatestFirmwareInformation(const Napi::CallbackInfo& info);
Napi::Value napi_DownloadFirmware(const Napi::CallbackInfo& info);
Napi::Value napi_UpdateFirmware(const Napi::CallbackInfo& info);
Napi::Value napi_DownloadFirmwareUpdater(const Napi::CallbackInfo& info);
Napi::Value napi_GetFirmwareFilePath(const Napi::CallbackInfo& info);
Napi::Value napi_IsFirmwareLockEnabled(const Napi::CallbackInfo& info);
Napi::Value napi_EnableFirmwareLock(const Napi::CallbackInfo& info);
Napi::Value napi_CancelFirmwareDownload(const Napi::CallbackInfo& info);
Napi::Value napi_CheckForFirmwareUpdate(const Napi::CallbackInfo& info);
Napi::Value napi_GetFirmwareVersionBundle(const Napi::CallbackInfo& info);
Napi::Value napi_GetRemoteControlFirmwareVersion(const Napi::CallbackInfo& info);
