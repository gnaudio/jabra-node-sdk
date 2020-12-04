#include "stdafx.h"
#include "device.h"
#include "enablers.h"
#include "settings.h"
#include "battery.h"
#include "misc.h"
#include "fwu.h"
#include "bt.h"
#include "app.h"
#include "callControl.h"


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
  EXPORTS_SET(GetLatestFirmwareInformation)

  EXPORTS_SET(GetDeviceImagePath)
  EXPORTS_SET(GetDeviceImageThumbnailPath)

  EXPORTS_SET(IsGnHidStdHidSupported)
  EXPORTS_SET(GetHidWorkingState)
  EXPORTS_SET(SetHidWorkingState)

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

  // Remote MMI
  EXPORTS_SET(GetRemoteMmiFocus)
  EXPORTS_SET(ReleaseRemoteMmiFocus)
  EXPORTS_SET(IsRemoteMmiInFocus)
  EXPORTS_SET(SetRemoteMmiAction)

  // Battery
  EXPORTS_SET(GetBatteryStatus)
  EXPORTS_SET(IsBatteryStatusSupported)

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
