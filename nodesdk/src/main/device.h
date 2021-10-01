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

Napi::Value napi_PreloadDeviceInfo(const Napi::CallbackInfo& info);
Napi::Value napi_PreloadAttachedDeviceInfo(const Napi::CallbackInfo& info);
Napi::Value napi_GetLocalManifestVersion(const Napi::CallbackInfo& info);

Napi::Value napi_IsCertifiedForSkypeForBusiness(const Napi::CallbackInfo& info);
Napi::Value napi_IsRemoteManagementEnabled(const Napi::CallbackInfo& info);
Napi::Value napi_EnableRemoteManagement(const Napi::CallbackInfo& info);
Napi::Value napi_SetXpressUrl(const Napi::CallbackInfo& info);
Napi::Value napi_GetXpressUrl(const Napi::CallbackInfo& info);
Napi::Value napi_SetPasswordProvisioning(const Napi::CallbackInfo& info);
Napi::Value napi_GetPasswordProvisioning(const Napi::CallbackInfo& info);
Napi::Value napi_ConfigureXpressManagement(const Napi::CallbackInfo& info);

Napi::Value napi_TriggerDiagnosticLogGeneration(const Napi::CallbackInfo& info);
Napi::Value napi_GetDiagnosticLogFile(const Napi::CallbackInfo& info);

Napi::Value napi_GetWhiteboardPosition(const Napi::CallbackInfo& info);
Napi::Value napi_SetWhiteboardPosition(const Napi::CallbackInfo& info);

Napi::Value napi_GetZoom(const Napi::CallbackInfo& info);
Napi::Value napi_SetZoom(const Napi::CallbackInfo& info);
Napi::Value napi_GetZoomLimits(const Napi::CallbackInfo& info);
Napi::Value napi_SetZoomRelativeAction(const Napi::CallbackInfo& info);

Napi::Value napi_GetPanTilt(const Napi::CallbackInfo& info);
Napi::Value napi_SetPanTilt(const Napi::CallbackInfo& info);
Napi::Value napi_GetPanTiltLimits(const Napi::CallbackInfo& info);
Napi::Value napi_SetPanTiltRelativeAction(const Napi::CallbackInfo& info);

Napi::Value napi_GetIntelligentZoomLatency(const Napi::CallbackInfo& info);
Napi::Value napi_SetIntelligentZoomLatency(const Napi::CallbackInfo& info);
Napi::Value napi_RestoreVideoRoomDefaults(const Napi::CallbackInfo& info);
Napi::Value napi_ResetPanTiltZoom(const Napi::CallbackInfo& info);
Napi::Value napi_ResetImageQualityControls(const Napi::CallbackInfo& info);
Napi::Value napi_SetVideoMode(const Napi::CallbackInfo& info);
Napi::Value napi_GetVideoMode(const Napi::CallbackInfo& info);
Napi::Value napi_SetWhiteboardOnMainStream(const Napi::CallbackInfo& info);
Napi::Value napi_GetWhiteboardOnMainStream(const Napi::CallbackInfo& info);
Napi::Value napi_IsCameraStreaming(const Napi::CallbackInfo& info);
Napi::Value napi_IsVideoDeviceStreaming(const Napi::CallbackInfo& info);
Napi::Value napi_GetPictureInPicture(const Napi::CallbackInfo& info);
Napi::Value napi_SetPictureInPicture(const Napi::CallbackInfo& info);
Napi::Value napi_SetVideoTransitionStyle(const Napi::CallbackInfo& info);
Napi::Value napi_GetVideoTransitionStyle(const Napi::CallbackInfo& info);
Napi::Value napi_SetVideoHDR(const Napi::CallbackInfo& info);
Napi::Value napi_GetVideoHDR(const Napi::CallbackInfo& info);
Napi::Value napi_SetVideoHDRDefault(const Napi::CallbackInfo& info);
Napi::Value napi_GetVideoHDRDefault(const Napi::CallbackInfo& info);
Napi::Value napi_SetContrastLevel(const Napi::CallbackInfo& info);
Napi::Value napi_GetContrastLevel(const Napi::CallbackInfo& info);
Napi::Value napi_SetSharpnessLevel(const Napi::CallbackInfo& info);
Napi::Value napi_GetSharpnessLevel(const Napi::CallbackInfo& info);
Napi::Value napi_SetBrightnessLevel(const Napi::CallbackInfo& info);
Napi::Value napi_GetBrightnessLevel(const Napi::CallbackInfo& info);
Napi::Value napi_SetSaturationLevel(const Napi::CallbackInfo& info);
Napi::Value napi_GetSaturationLevel(const Napi::CallbackInfo& info);
Napi::Value napi_SetWhiteBalance(const Napi::CallbackInfo& info);
Napi::Value napi_GetWhiteBalance(const Napi::CallbackInfo& info);

Napi::Value napi_SetContrastLimits(const Napi::CallbackInfo& info);
Napi::Value napi_GetContrastLimits(const Napi::CallbackInfo& info);
Napi::Value napi_SetSharpnessLimits(const Napi::CallbackInfo& info);
Napi::Value napi_GetSharpnessLimits(const Napi::CallbackInfo& info);
Napi::Value napi_SetBrightnessLimits(const Napi::CallbackInfo& info);
Napi::Value napi_GetBrightnessLimits(const Napi::CallbackInfo& info);
Napi::Value napi_SetSaturationLimits(const Napi::CallbackInfo& info);
Napi::Value napi_GetSaturationLimits(const Napi::CallbackInfo& info);
Napi::Value napi_SetWhiteBalanceLimits(const Napi::CallbackInfo& info);
Napi::Value napi_GetWhiteBalanceLimits(const Napi::CallbackInfo& info);

Napi::Value napi_SetRoomCapacity(const Napi::CallbackInfo& info);
Napi::Value napi_GetRoomCapacity(const Napi::CallbackInfo& info);
Napi::Value napi_SetRoomCapacityNotificationEnabled(const Napi::CallbackInfo& info);
Napi::Value napi_GetRoomCapacityNotificationEnabled(const Napi::CallbackInfo& info);
Napi::Value napi_SetNotificationStyle(const Napi::CallbackInfo& info);
Napi::Value napi_GetNotificationStyle(const Napi::CallbackInfo& info);
Napi::Value napi_SetNotificationUsage(const Napi::CallbackInfo& info);
Napi::Value napi_GetNotificationUsage(const Napi::CallbackInfo& info);

Napi::Value napi_StoreColorControlPreset(const Napi::CallbackInfo& info);
Napi::Value napi_ApplyColorControlPreset(const Napi::CallbackInfo& info);
Napi::Value napi_StorePTZPreset(const Napi::CallbackInfo& info);
Napi::Value napi_ApplyPTZPreset(const Napi::CallbackInfo& info);

Napi::Value napi_SetSecondVideoStream(const Napi::CallbackInfo& info);
Napi::Value napi_GetSecondVideoStream(const Napi::CallbackInfo& info);
Napi::Value napi_GetEthernetIPv4Status(const Napi::CallbackInfo& info);
Napi::Value napi_GetWLANIPv4Status(const Napi::CallbackInfo& info);
Napi::Value napi_GetUSBState(const Napi::CallbackInfo& info);
