#include "stdafx.h"

Napi::Value napi_GetDeviceImageThumbnailPath(const Napi::CallbackInfo& info);
Napi::Value napi_GetDeviceImagePath(const Napi::CallbackInfo& info);

Napi::Value napi_IsGnHidStdHidSupported(const Napi::CallbackInfo& info);
Napi::Value napi_GetHidWorkingState(const Napi::CallbackInfo& info);
Napi::Value napi_SetHidWorkingState(const Napi::CallbackInfo& info);

Napi::Value napi_GetLock(const Napi::CallbackInfo& info);
Napi::Value napi_ReleaseLock(const Napi::CallbackInfo& info);
Napi::Value napi_IsLocked(const Napi::CallbackInfo& info);
  
Napi::Value napi_IsRingerSupported(const Napi::CallbackInfo& info);
Napi::Value napi_SetRinger(const Napi::CallbackInfo& info);

Napi::Value napi_IsOffHookSupported(const Napi::CallbackInfo& info);
Napi::Value napi_SetOffHook(const Napi::CallbackInfo& info);

Napi::Value napi_UploadRingtone(const Napi::CallbackInfo& info);
Napi::Value napi_UploadWavRingtone(const Napi::CallbackInfo& info);

Napi::Value napi_UploadImage(const Napi::CallbackInfo& info);
Napi::Value napi_GetNamedAsset(const Napi::CallbackInfo& info);
Napi::Value napi_GetSupportedButtonEvents(const Napi::CallbackInfo& info);

Napi::Value napi_IsOnlineSupported(const Napi::CallbackInfo& info);

Napi::Value napi_IsMuteSupported(const Napi::CallbackInfo& info);
Napi::Value napi_SetMute(const Napi::CallbackInfo& info);

Napi::Value napi_IsHoldSupported(const Napi::CallbackInfo& info);
Napi::Value napi_IsBusyLightSupported(const Napi::CallbackInfo& info);

Napi::Value napi_IsSetDateTimeSupported(const Napi::CallbackInfo& info);

Napi::Value napi_IsFeatureSupported(const Napi::CallbackInfo& info);
Napi::Value napi_GetSupportedFeatures(const Napi::CallbackInfo& info);

Napi::Value napi_GetWizardMode(const Napi::CallbackInfo& info);
Napi::Value napi_GetSecureConnectionMode(const Napi::CallbackInfo& info);
Napi::Value napi_RebootDevice(const Napi::CallbackInfo& info);
Napi::Value napi_IsEqualizerSupported(const Napi::CallbackInfo& info);
Napi::Value napi_IsEqualizerEnabled(const Napi::CallbackInfo& info);
Napi::Value napi_EnableEqualizer(const Napi::CallbackInfo& info);
Napi::Value napi_SetDatetime(const Napi::CallbackInfo& info);
Napi::Value napi_GetDatetime(const Napi::CallbackInfo& info);
Napi::Value napi_SetTimestamp(const Napi::CallbackInfo& info);
Napi::Value napi_SetEqualizerParameters(const Napi::CallbackInfo& info);
Napi::Value napi_PlayRingTone(const Napi::CallbackInfo& info);
Napi::Value napi_GetESN(const Napi::CallbackInfo& info);
Napi::Value napi_GetTimestamp(const Napi::CallbackInfo& info);
Napi::Value napi_SetWizardMode(const Napi::CallbackInfo& info);
Napi::Value napi_GetAudioFileParametersForUpload(const Napi::CallbackInfo& info);
Napi::Value napi_GetButtonFocus(const Napi::CallbackInfo& info);
Napi::Value napi_ReleaseButtonFocus(const Napi::CallbackInfo& info);

Napi::Value napi_GetEqualizerParameters(const Napi::CallbackInfo& info);

Napi::Value napi_GetRemoteMmiFocus(const Napi::CallbackInfo& info);
Napi::Value napi_ReleaseRemoteMmiFocus(const Napi::CallbackInfo& info);
Napi::Value napi_IsRemoteMmiInFocus(const Napi::CallbackInfo& info);
Napi::Value napi_SetRemoteMmiAction(const Napi::CallbackInfo& info);
