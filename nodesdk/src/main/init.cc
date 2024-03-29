#include "stdafx.h"
#include "device.h"
#include "dect.h"
#include "enablers.h"
#include "settings.h"
#include "battery.h"
#include "misc.h"
#include "fwu.h"
#include "bt.h"
#include "app.h"
#include "callControl.h"
#include "deviceconstants.h"


/**
 * Here we set nodejs exports for all native code functions declared in the various .cc modules.
 *
 * Requires a naming pattern where all native code functions is prefixed with "napi_" and
 * the exported javascript function is identical except it is without the "napi_" prefix.
  */
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  // App:
  EXPORTS_SET(Initialize)
  EXPORTS_SET(UnInitialize)

  EXPORTS_SET(ConnectToJabraApplication)
  EXPORTS_SET(DisconnectFromJabraApplication)
  EXPORTS_SET(SetSoftphoneReady)
  EXPORTS_SET(IsSoftphoneInFocus)
  EXPORTS_SET(GetErrorString)

  EXPORTS_SET(SyncExperiment)

  // Framework:
  EXPORTS_SET(GetVersion)

  // Device:
  EXPORTS_SET(GetFirmwareVersion)
  EXPORTS_SET(GetFirmwareVersionBundle)
  EXPORTS_SET(GetLatestFirmwareInformation)
  EXPORTS_SET(IsCertifiedForSkypeForBusiness)
  EXPORTS_SET(GetRemoteControlFirmwareVersion)
  
  EXPORTS_SET(GetLanguagePackInformation)
  EXPORTS_SET(GetSubDeviceProperty)
  EXPORTS_SET(GetUserDefinedDeviceName)

  EXPORTS_SET(GetDeviceImagePath)
  EXPORTS_SET(GetDeviceImageThumbnailPath)
  
  EXPORTS_SET(PreloadDeviceInfo)
  EXPORTS_SET(PreloadAttachedDeviceInfo)
  EXPORTS_SET(GetLocalManifestVersion)

  EXPORTS_SET(IsGnHidStdHidSupported)
  EXPORTS_SET(GetHidWorkingState)
  EXPORTS_SET(SetHidWorkingState)

  EXPORTS_SET(GetLock);
  EXPORTS_SET(ReleaseLock);
  EXPORTS_SET(IsLocked);
  EXPORTS_SET(SetHidWorkingState)

  EXPORTS_SET(IsOnlineSupported)
 
  EXPORTS_SET(IsMuteSupported)
  EXPORTS_SET(SetMute)

  EXPORTS_SET(IsHoldSupported) 
  EXPORTS_SET(IsBusyLightSupported)  

  EXPORTS_SET(IsRingerSupported)
  EXPORTS_SET(SetRinger)

  EXPORTS_SET(IsOffHookSupported)
  EXPORTS_SET(SetOffHook)

  EXPORTS_SET(UploadWavRingtone)
  EXPORTS_SET(UploadRingtone)
  EXPORTS_SET(UploadImage)
  EXPORTS_SET(GetNamedAsset)
  EXPORTS_SET(GetWizardMode)
  EXPORTS_SET(SetWizardMode)
  EXPORTS_SET(GetSecureConnectionMode)
  EXPORTS_SET(RebootDevice)
  EXPORTS_SET(GetAudioFileParametersForUpload)
  EXPORTS_SET(GetSupportedButtonEvents)
  EXPORTS_SET(GetButtonFocus)
  EXPORTS_SET(ReleaseButtonFocus)
   
  // Suported features
  EXPORTS_SET(IsFeatureSupported)
  EXPORTS_SET(GetSupportedFeatures)

  // Misc
  EXPORTS_SET(GetPanics)

  // FWU
  EXPORTS_SET(DownloadFirmware)
  EXPORTS_SET(UpdateFirmware)
  EXPORTS_SET(DownloadFirmwareUpdater)
  EXPORTS_SET(GetFirmwareFilePath)
  EXPORTS_SET(IsFirmwareLockEnabled)
  EXPORTS_SET(EnableFirmwareLock)
  EXPORTS_SET(CancelFirmwareDownload)
  EXPORTS_SET(CheckForFirmwareUpdate)

  // Device settings:
  EXPORTS_SET(SetSettings)
  EXPORTS_SET(GetSetting)
  EXPORTS_SET(GetSettings)
  EXPORTS_SET(FactoryReset)
  EXPORTS_SET(IsFactoryResetSupported)
  EXPORTS_SET(IsSettingProtectionEnabled)
  EXPORTS_SET(IsUploadRingtoneSupported)
  EXPORTS_SET(IsUploadImageSupported)
  EXPORTS_SET(IsSetDateTimeSupported)
  EXPORTS_SET(IsEqualizerSupported)
  EXPORTS_SET(IsEqualizerEnabled)
  EXPORTS_SET(EnableEqualizer)
  EXPORTS_SET(GetFailedSettingNames)
  EXPORTS_SET(SetTimestamp)
  EXPORTS_SET(SetEqualizerParameters)
  EXPORTS_SET(PlayRingTone)
  EXPORTS_SET(GetESN)
  EXPORTS_SET(GetTimestamp)
  EXPORTS_SET(GetEqualizerParameters)
  EXPORTS_SET(SetDatetime) 
  EXPORTS_SET(GetDatetime)
  EXPORTS_SET(GetEthernetIPv4Status);
  EXPORTS_SET(GetWLANIPv4Status);
  EXPORTS_SET(GetMACAddress);
  
  // Remote MMI
  EXPORTS_SET(GetRemoteMmiFocus)
  EXPORTS_SET(ReleaseRemoteMmiFocus)
  EXPORTS_SET(IsRemoteMmiInFocus)
  EXPORTS_SET(SetRemoteMmiAction)

  // Battery
  EXPORTS_SET(GetBatteryStatus)
  EXPORTS_SET(IsBatteryStatusSupported)
  EXPORTS_SET(GetRemoteControlBatteryStatus)

  // BT
  EXPORTS_SET(SearchNewDevices)
  EXPORTS_SET(ConnectBTDevice)
  EXPORTS_SET(ConnectNewDevice)
  EXPORTS_SET(ConnectPairedDevice)
  EXPORTS_SET(GetConnectedBTDeviceName)
  EXPORTS_SET(ClearPairedDevice)
  EXPORTS_SET(GetSearchDeviceList)

  EXPORTS_SET(DisconnectBTDevice)
  EXPORTS_SET(DisconnectPairedDevice)

  EXPORTS_SET(GetAutoPairing)
  EXPORTS_SET(SetAutoPairing)
  EXPORTS_SET(IsPairingListSupported)
  EXPORTS_SET(GetPairingList)
  EXPORTS_SET(ClearPairingList)

  EXPORTS_SET(SetBTPairing)
  EXPORTS_SET(StopBTPairing)
  
  EXPORTS_SET(BTLinkQualityChangeEventEnabled)

  // DECT
  EXPORTS_SET(GetConnectedHeadsetNames)
  EXPORTS_SET(TriggerDECTPairing)
  EXPORTS_SET(TriggerDECTSecurePairing)
  EXPORTS_SET(GetDECTPairingKey)
  EXPORTS_SET(SetDECTPairingKey)
  
  // Callbacks:
  EXPORTS_SET(IsDevLogEnabled);
  EXPORTS_SET(EnableDevLog);

  // Setup logging.
  EXPORTS_SET(NativeAddonLog);
  EXPORTS_SET(GetNativeAddonLogConfig);

  // Call control
  EXPORTS_SET(SetHold);
  EXPORTS_SET(GetBusyLightStatus);
  EXPORTS_SET(SetBusyLightStatus);
  EXPORTS_SET(SetOnline);

  // Network related controls
  EXPORTS_SET(IsRemoteManagementEnabled);
  EXPORTS_SET(EnableRemoteManagement);
  EXPORTS_SET(SetXpressUrl);
  EXPORTS_SET(GetXpressUrl);
  EXPORTS_SET(ConfigureXpressManagement);
  EXPORTS_SET(GetXpressManagementNetworkStatus);
  EXPORTS_SET(GetDiagnosticLogFile);
  EXPORTS_SET(TriggerDiagnosticLogGeneration);
  EXPORTS_SET(SetPasswordProvisioning);
  EXPORTS_SET(GetPasswordProvisioning);
  EXPORTS_SET(SetNetworkAuthenticationMode);
  EXPORTS_SET(GetNetworkAuthenticationMode);
  EXPORTS_SET(SetNetworkAuthenticationIdentity);

  // Video related controls
  EXPORTS_SET(GetIntelligentZoomLatency)
  EXPORTS_SET(SetIntelligentZoomLatency)
  EXPORTS_SET(IsCameraStreaming);
  EXPORTS_SET(IsVideoDeviceStreaming);
  EXPORTS_SET(GetPictureInPicture);
  EXPORTS_SET(SetPictureInPicture);
  EXPORTS_SET(GetWhiteboardPosition);
  EXPORTS_SET(SetWhiteboardPosition);
  EXPORTS_SET(GetZoom);
  EXPORTS_SET(SetZoom);
  EXPORTS_SET(GetZoomLimits);
  EXPORTS_SET(GetSensorRegions);
  EXPORTS_SET(GetPanTilt);
  EXPORTS_SET(SetPanTilt);
  EXPORTS_SET(GetPanTiltLimits);
  EXPORTS_SET(SetPanTiltRelativeAction);
  EXPORTS_SET(SetZoomRelativeAction);
  EXPORTS_SET(StoreColorControlPreset);
  EXPORTS_SET(ApplyColorControlPreset);
  EXPORTS_SET(StorePTZPreset);
  EXPORTS_SET(ApplyPTZPreset);
  EXPORTS_SET(RestoreVideoRoomDefaults);
  EXPORTS_SET(ResetPanTiltZoom);
  EXPORTS_SET(ResetImageQualityControls);
  EXPORTS_SET(SetVideoHDR);
  EXPORTS_SET(GetVideoHDR);
  EXPORTS_SET(SetVideoHDRDefault);
  EXPORTS_SET(GetVideoHDRDefault);
  EXPORTS_SET(GetContrastLevel);
  EXPORTS_SET(SetContrastLevel);
  EXPORTS_SET(GetSharpnessLevel);
  EXPORTS_SET(SetSharpnessLevel);
  EXPORTS_SET(GetBrightnessLevel);
  EXPORTS_SET(SetBrightnessLevel);
  EXPORTS_SET(GetSaturationLevel);
  EXPORTS_SET(SetSaturationLevel);
  EXPORTS_SET(GetWhiteBalance);
  EXPORTS_SET(SetWhiteBalance);
  EXPORTS_SET(GetContrastLimits);
  EXPORTS_SET(GetSharpnessLimits);
  EXPORTS_SET(GetBrightnessLimits);
  EXPORTS_SET(GetSaturationLimits);
  EXPORTS_SET(GetWhiteBalanceLimits);
  EXPORTS_SET(SetRoomCapacity);
  EXPORTS_SET(GetRoomCapacity);
  EXPORTS_SET(SetRoomCapacityNotificationEnabled);
  EXPORTS_SET(GetRoomCapacityNotificationEnabled);
  EXPORTS_SET(SetNotificationStyle);
  EXPORTS_SET(GetNotificationStyle);
  EXPORTS_SET(SetNotificationUsage);
  EXPORTS_SET(GetNotificationUsage);
  EXPORTS_SET(SetSecondVideoStream);
  EXPORTS_SET(GetSecondVideoStream);
  EXPORTS_SET(SetVideoMode);
  EXPORTS_SET(GetVideoMode);
  EXPORTS_SET(SetVideoTransitionStyle);
  EXPORTS_SET(GetVideoTransitionStyle);
  EXPORTS_SET(GetWhiteboardOnMainStream);
  EXPORTS_SET(SetWhiteboardOnMainStream);
  EXPORTS_SET(GetUSBState);
    
  // Constants
  EXPORTS_SET(GetConstSync);
  EXPORTS_SET(GetConstStringSync);
  EXPORTS_SET(GetConstBooleanSync);
  EXPORTS_SET(GetConstIntegerSync);
  EXPORTS_SET(GetConstFieldSync);
  EXPORTS_SET(GetConstListSync);

  try {
    configureLogging();
  } catch (const std::exception &e) {
    std::cerr << "Fatal log error - configureLogging for jabra sdk node wrapper failed:" << e.what() << std::flush;
  } catch (...) {
    std::cerr << "Fatal log error - configureLogging for jabra sdk node wrapper failed:" << std::flush;
  }

  return exports;
}

NODE_API_MODULE(sdkintegration, Init)
