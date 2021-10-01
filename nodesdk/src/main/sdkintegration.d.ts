/**
 * This declares the natively defined javascript types defined in the c++ code for the
 * N-API based sdk integration. The interface is not required/supported by N-API but 
 * supplied for maximum type safety internally for this module.
 */

import { ConfigParamsCloud, GenericConfigParams, enumHidState, AudioFileFormatEnum, DeviceSettings, DeviceInfo, PairedListInfo,
         NamedAsset, AddonLogSeverity, JabraError, RemoteMmiActionOutput, DectInfo, WhiteboardPosition, ZoomLimits, PanTilt, DateTime, VideoLimitsStepSize, PanTiltRelative, ZoomRelative, IPv4Status } from './core-types';
import { enumDeviceBtnType, enumFirmwareEventType, enumFirmwareEventStatus,
         enumUploadEventStatus, enumBTPairedListType, enumRemoteMmiType,
         enumRemoteMmiInput, enumRemoteMmiPriority, enumRemoteMmiSequence, enumColorControlPreset, enumPTZPreset, enumAutoWhiteBalance, enumZoomDirection, enumSecondaryStreamContent, enumVideoTransitionStyle, enumWizardMode, enumUSBState} from './jabra-enums';

/** 
 * Declares all natively implemented n-api functions that call into the Jabra C SDK.
 * 
 * These functions are considered a low-level implementation detail and should NOT
 * be exposed directly to the users of the wrapper.
 * 
 * The functions all use simple callbacks because the current n-api c++ interface do
 * not handle promises well. This means that the high-level parts of this wrapper
 * should convert the callbacks into promises when required (the standard node 
 * function util.promisify can be useful for this).
 *  
 * @internal 
 **/
export declare interface SdkIntegration {
    isOnlineSupported(isOnlineSupported: any);
    /**
     * Initialize SDK + Jabra_SetAppID + pre-register all callback functions
     * for all events.
     */
    Initialize(appId: string,
               success: (error: JabraError, result: void) => void,
               firstScanDone: (event_time_ms: number) => void,
               attached: (deviceId: DeviceInfo, event_time_ms: number) => void,
               deAttached: (deviceId: number, event_time_ms: number) => void,
               buttonInDataTranslated: (deviceId: number, translatedInData: enumDeviceBtnType, buttonInData: bool) => void,
               devLogCallback: (deviceId: number, json: string) => void,
               diagnosticLogCallback: (deviceId: number) => void,
               batteryStatusCallback: (deviceId: number, levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => void,
               onRemoteMmiEvent: (deviceId: number, type: enumRemoteMmiType, input: enumRemoteMmiInput) => void,
               xpressConnectionStatusCallback: (deviceId: number, status: boolean) => void,
               downloadFirmwareProgressCallback: (deviceId: number, type: enumFirmwareEventType, status: enumFirmwareEventStatus, dwnFirmPercentage: number) => void,
               uploadProgressCallback: (deviceId: number, status: enumUploadEventStatus, percentage: number) => void,
               registerPairingListCallback: (deviceId: number, pairedListInfo: PairedListInfo) => void,
               onGNPBtnEventCallback: (deviceId: number, btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>) => void,
               dectInfoCallback: (deviceId: number, dectInfo: DectInfo) => void,
               cameraStatusCallback: (deviceId: number, status: boolean) => void,
               configParams: ConfigParamsCloud & GenericConfigParams) : void;

    /**
     * Uninitialize SDK and free resources. Must be called when 
     * finished (otherwise node process will not shutdown).
     * 
     * Nb. This method is blocking!!
     */
    UnInitialize(): boolean;
    
    /***
     * Add a message to native log file (internal utility, not directly Jabra SDK related).
     * 
     * Do not call this directly - use the optimized and more flexible js helper _JabraNativeAddonLog.
     * 
     * This function is blocking(!) and is also not optimized for speed. Depending on OS, the logging code alone takes 
     * from 8-70 ms to run excluding the n-api wrapping itself. 
     * 
     * Consequently, this function is not intended for general use, in loops or anywhere where 
     * speed is critical. It is however fine to use this log infrequently, like in startup/closedown
     * to mark bounderies, to log errors etc.
     * 
    */
    NativeAddonLog(severity: AddonLogSeverity, caller: string, msg: string | Error): void;

    /**
     * Get native log configuration (internal utility, not directly Jabra SDK related).
     * 
     * This function is blocking(!) and is also not optimized for speed. Clients should 
     * call this once and than cache the value.
     */
    GetNativeAddonLogConfig() : NativeAddonLogConfig;

    /**
     * Template for calling experimental N-API code synchronously. For development use only for
     * experiments only. Otherwise not called.
     * 
     * Do not call this function in production - it is for development experiments only.
     */
    SyncExperiment(param: any): any;
    
    // -----------------------------------------------------------------------------------------------------------------------
    // 1-1 non-blocking mappings of the non-device related SDK API using callbacks for results/completion.
    // ------------------------------------------------------------------------------------------------------------------------

    ConnectToJabraApplication(guid: string, softphoneName: string, callback: (error: JabraError, result: boolean) => void): void;
    DisconnectFromJabraApplication(callback: (error: JabraError, result: void) => void): void;

    SetSoftphoneReady(isReady: boolean, callback: (error: JabraError, result: void) => void): void;
    IsSoftphoneInFocus(callback: (error: JabraError, result: boolean) => void): void;

    GetErrorString(errStatusCode: number, callback: (error: JabraError, result: string) => void): void;

    // ------------------------------------------------------------------------------------------------------------------------
    // 1-1 non-blocking mappings of the device related SDK API using callbacks for results/completion.
    // ------------------------------------------------------------------------------------------------------------------------

    GetLatestFirmwareInformation(deviceId: number, string: authorizationId, callback: (error: JabraError, result: FirmwareInfoType) => void): void;
    GetFirmwareVersion(deviceId: number, callback: (error: JabraError, result: string) => void): void;
    IsFirmwareLockEnabled(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    EnableFirmwareLock(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    GetLock(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    ReleaseLock(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    IsLocked(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
   
    IsDevLogEnabled(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    EnableDevLog(deviceId: number, enabled: boolean, callback: (error: JabraError, result: void) => void): void;

    GetDeviceImagePath(deviceId: number, callback: (error: JabraError, result: string) => void): void;
    GetDeviceImageThumbnailPath(deviceId: number, callback: (error: JabraError, result: string) => void): void;

    IsGnHidStdHidSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    GetHidWorkingState(deviceId: number, callback: (error: JabraError, result: enumHidState) => void): void;
    SetHidWorkingState(deviceId: number, state: enumHidState, callback: (error: JabraError, result: void) => void): void;

    GetSettings(deviceId: number, callback: (error: JabraError, result: DeviceSettings) => void): void;
    GetSetting(deviceId: number, guid: string, callback: (error: JabraError, result: DeviceSettings) => void): void;
    SetSettings(deviceId: number, settings: DeviceSettings, callback: (error: JabraError, result: void) => void): void;
    
    FactoryReset(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    IsFactoryResetSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;

    IsSettingProtectionEnabled(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;

    IsUploadRingtoneSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    IsUploadImageSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;

    IsRingerSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetRinger(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;

    IsOffHookSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetOffHook(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;

    GetVersion(callback: (error: JabraError, result: string) => void) : void;

    GetBatteryStatus(deviceId: number, callback: (error: JabraError, result: BatteryStatusType) => void): void;
    IsBatteryStatusSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    GetRemoteControlBatteryStatus(deviceId: number, callback: (error: JabraError, result: BatteryStatusType) => void): void;
    
    UploadRingtone(deviceId: number, filename: string, callback: (error: JabraError, result: void) => void): void;
    UploadWavRingtone(deviceId: number, filename: string, callback: (error: JabraError, result: void) => void): void;

    UploadImage(deviceId: number, filename: string, callback: (error: JabraError, result: void) => void): void;

    GetNamedAsset(deviceId: number, filename: assetName, callback: (error: JabraError, result: NamedAsset) => void): void;

    GetPanics(deviceId: number, callback: (error: JabraError, result: string[]) => void): void;

    DownloadFirmware(deviceId: number, version: string, authorization?: string, callback: (error: JabraError, result: void) => void): void;
    UpdateFirmware(deviceId: number, firmFile: string, callback: (error: JabraError, result: void) => void): void;
    DownloadFirmwareUpdater(deviceId: number, authorization?: string, callback: (error: JabraError, result: void) => void): void;
    GetFirmwareFilePath(deviceId: number, version: string, callback: (error: JabraError, result: string) => void): void;

    SearchNewDevices(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    ConnectBTDevice(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    ConnectNewDevice(deviceId: number, deviceName: string, deviceBTAddr: string, isConnected: boolean, callback: (error: JabraError, result: void) => void): void;
    ConnectPairedDevice(deviceId: number, deviceName: string, deviceBTAddr: string, isConnected: boolean, callback: (error: JabraError, result: void) => void): void;
    GetConnectedBTDeviceName(deviceId: number, callback: (error: JabraError, result: string) => void): void;
    GetSearchDeviceList(deviceId: number, callback: (error: JabraError, result: Array<{ deviceName: string, deviceBTAddr: string, isConnected: boolean }>) => void): void;
    
    DisconnectBTDevice(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    DisconnectPairedDevice(deviceId: number, deviceName: string, deviceBTAddr: string, isConnected: boolean, callback: (error: JabraError, result: void) => void): void;
 
    GetAutoPairing(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetAutoPairing(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    IsPairingListSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    GetPairingList(deviceId: number, callback: (error: JabraError, result: Array<{ deviceName: string, deviceBTAddr: string, isConnected: boolean }>) => void): void;

    ClearPairingList(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    ClearPairedDevice(deviceId: number, deviceName: string, deviceBTAddr: string, isConnected: boolean, callback: (error: JabraError, result: void) => void): void;
    
    StopBTPairing(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    SetBTPairing(deviceId: number, callback: (error: JabraError, result: void) => void): void;
       
    GetSupportedButtonEvents(deviceId: number, callback: (error: JabraError, result: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>) => void): void;
    
    IsMuteSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetMute(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;

    IsHoldSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;

    IsBusyLightSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetHold(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    GetBusyLightStatus(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetBusyLightStatus(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    SetOnline(deviceId: number, online: boolean, callback: (error: JabraError, result: void) => void): void;

    IsSetDateTimeSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    IsFeatureSupported(deviceId: number, feature: number, callback: (error: JabraError, result: boolean) => void): void;
    GetWizardMode(deviceId: number, callback: (error: JabraError, result: enumWizardMode) => void): void;   
    GetSecureConnectionMode(deviceId: number, callback: (error: JabraError, result: number) => void): void;   
    RebootDevice(deviceId: number, callback: (error: JabraError, result: void) => void): void;   
    IsEqualizerSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    IsEqualizerEnabled(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    EnableEqualizer(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    
    IsOnlineSupported(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    
    CancelFirmwareDownload( deviceId: number, callback: (error: JabraError, result: void) => void): void;
    SetTimestamp( deviceId: number, timeStamp: number, callback: (error: JabraError, result: void) => void): void;
    SetEqualizerParameters( deviceId: number, bands:Array<number>, callback: (error: JabraError, result: void) => void): void;
    CheckForFirmwareUpdate( deviceId: number, authorization:string, callback: (error: JabraError, result: boolean) => void): void;
    PlayRingTone( deviceId: number, level:number, type:number,callback: (error: JabraError, result:void) => void): void;
    GetESN(deviceId: number, callback: (error: JabraError, result: string) => void): void;
    GetFailedSettingNames(deviceId: number, callback: (error: JabraError, result: Array<string>) => void): void;
    GetTimestamp(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetWizardMode(deviceId: number, wizardMode: enumWizardMode, callback: (error: JabraError, result: void) => void): void;
    GetAudioFileParametersForUpload(deviceId: number, callback: (error: JabraError, result: { audioFileType: AudioFileFormatEnum, numChannels: number, bitsPerSample: number, sampleRate: number, maxFileSize: number }) => void): void;
    SetDatetime(deviceId: number, dateTime: DateTime, callback: (error: JabraError, result: void) => void): void;
    GetDatetime(deviceId: number, callback: (error: JabraError, result: DateTime) => void): void;
    GetEqualizerParameters(deviceId: number, maxNBands:number, callback: (error: JabraError, result: Array<{ max_gain: number, centerFrequency: number, currentGain: number }>) => void): void;
    GetSupportedFeatures(deviceId: number, callback: (error: JabraError, result: Array<enumDeviceFeature>) => void): void;

    GetButtonFocus(deviceId: number, btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>, callback: (error: JabraError, result: void) => void): void;
    ReleaseButtonFocus(deviceId: number, btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>, callback: (error: JabraError, result: void) => void): void;

    GetRemoteMmiFocus(deviceId: number, type: enumRemoteMmiType, input: enumRemoteMmiInput, priority: enumRemoteMmiPriority, callback: (error: JabraError, result: void) => void): void;    
    ReleaseRemoteMmiFocus(deviceId: number, type: enumRemoteMmiType, callback: (error: JabraError, result: void) => void): void;
    IsRemoteMmiInFocus(deviceId: number, type: enumRemoteMmiType, callback: (error: JabraError, result: boolean) => void): void;
    SetRemoteMmiAction(deviceId: number, type: enumRemoteMmiType, actionOuput: RemoteMmiActionOutput, callback: (error: JabraError, result: void) => void): void;

    IsCertifiedForSkypeForBusiness(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    IsRemoteManagementEnabled(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    EnableRemoteManagement(deviceId: number, enable: boolean, timeout: number, callback: (error: JabraError, result: void) => void): void;
    SetXpressUrl(deviceId: number, url: string, timeout: number, callback: (error: JabraError, result: void) => void): void;
    GetXpressUrl(deviceId: number, callback: (error: JabraError, result: string) => void): void;
    SetPasswordProvisioning(deviceId: number, password: string, callback: (error: JabraError, result: void) => void): void;
    GetPasswordProvisioning(deviceId: number, callback: (error: JabraError, result: string) => void): void;
    ConfigureXpressManagement(deviceId: number, url: string, proxy: ProxySettings, timeout: number, callback: (error: JabraError, result: void) => void): void;
    
    GetDiagnosticLogFile(deviceId: number, filename: string, callback: (error: JabraError, result: void) => void): void;
    TriggerDiagnosticLogGeneration(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    
    PreloadDeviceInfo(zipFileName: string, callback: (error: JabraError, result: void) => void): void;
    PreloadAttachedDeviceInfo(deviceId: number, zipFileName: string, callback: (error: JabraError, result: void) => void): void;
    GetLocalManifestVersion(deviceId: number, callback: (error: JabraError, result: string) => void): void;

    GetWhiteboardPosition(deviceId: number, whiteboardId: number, callback: (error: JabraError, result: WhiteboardPosition) => void): void;
    SetWhiteboardPosition(deviceId: number, whiteboardId: number, whiteboardPosition: WhiteboardPosition, callback: (error: JabraError, result: void) => void): void;

    GetZoom(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetZoom(deviceId: number, zoom: number, callback: (error: JabraError, result: void) => void): void;
    GetZoomLimits(deviceId: number, callback: (error: JabraError, result: VideoLimits2) => void): void;
    SetZoomRelativeAction(deviceId: number, action: ZoomRelative, callback: (error: JabraError, result: void) => void): void;
    
    GetPanTilt(deviceId: number, callback: (error: JabraError, result: PanTilt) => void): void;
    SetPanTilt(deviceId: number, panTilt: PanTilt, callback: (error: JabraError, result: void) => void): void;
    GetPanTiltLimits(deviceId: number, callback: (error: JabraError, result: PanTiltLimits) => void): void;
    SetPanTiltRelativeAction(deviceId: number, action: PanTiltRelative, callback: (error: JabraError, result: void) => void): void;
    
    GetIntelligentZoomLatency(deviceId: number, callback: (error: JabraError, result: enumIntelligentZoomLatency) => void): void;
    SetIntelligentZoomLatency(deviceId: number, latency: enumIntelligentZoomLatency, callback: (error: JabraError, result: void) => void): void;
    RestoreVideoRoomDefaults(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    IsCameraStreaming(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    IsVideoDeviceStreaming(deviceId: number, callback: (error: JabraError, result: VideoDeviceStreamingStatus) => void): void;
    GetPictureInPicture(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetPictureInPicture(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    SetVideoMode(deviceId: number, mode: enumVideoMode, callback: (error: JabraError, result: void) => void): void;
    GetVideoMode(deviceId: number, callback: (error: JabraError, result: enumVideoMode) => void): void;
    SetWhiteboardOnMainStream(deviceId: number, mode: bool, callback: (error: JabraError, result: void) => void): void;
    GetWhiteboardOnMainStream(deviceId: number, callback: (error: JabraError, result: bool) => void): void;
    SetVideoTransitionStyle(deviceId: number, mode: enumVideoTransitionStyle, callback: (error: JabraError, result: void) => void): void;
    GetVideoTransitionStyle(deviceId: number, callback: (error: JabraError, result: enumVideoTransitionStyle) => void): void;
    GetVideoHDRDefault(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetVideoHDRDefault(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    GetVideoHDR(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetVideoHDR(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    GetContrastLevel(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetContrastLevel(deviceId: number, level: number, callback: (error: JabraError, result: void) => void): void;
    GetSharpnessLevel(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetSharpnessLevel(deviceId: number, level: number, callback: (error: JabraError, result: void) => void): void;
    GetBrightnessLevel(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetBrightnessLevel(deviceId: number, level: number, callback: (error: JabraError, result: void) => void): void;
    GetSaturationLevel(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetSaturationLevel(deviceId: number, level: number, callback: (error: JabraError, result: void) => void): void;
    GetWhiteBalance(deviceId: number, callback: (error: JabraError, result: WhiteBalance) => void): void;
    SetWhiteBalance(deviceId: number, setting: WhiteBalance, callback: (error: JabraError, result: void) => void): void;
    
    GetContrastLimits(deviceId: number, callback: (error: JabraError, result: VideoLimits) => void): void;
    GetSharpnessLimits(deviceId: number, callback: (error: JabraError, result: VideoLimits) => void): void;
    GetBrightnessLimits(deviceId: number, callback: (error: JabraError, result: VideoLimits) => void): void;
    GetSaturationLimits(deviceId: number, callback: (error: JabraError, result: VideoLimits) => void): void;
    GetWhiteBalanceLimits(deviceId: number, callback: (error: JabraError, result: VideoLimits) => void): void;

    SetRoomCapacity(deviceId: number, capacity: number, callback: (error: JabraError, result: void) => void): void;
    GetRoomCapacity(deviceId: number, callback: (error: JabraError, result: number) => void): void;
    SetRoomCapacityNotificationEnabled(deviceId: number, enable: boolean, callback: (error: JabraError, result: void) => void): void;
    GetRoomCapacityNotificationEnabled(deviceId: number, callback: (error: JabraError, result: boolean) => void): void;
    SetNotificationStyle(deviceId: number, style: enumNotificationStyle, callback: (error: JabraError, result: void) => void): void;
    GetNotificationStyle(deviceId: number, callback: (error: JabraError, result: enumNotificationStyle) => void): void;
    SetNotificationUsage(deviceId: number, usage: enumNotificationUsage, callback: (error: JabraError, result: void) => void): void;
    GetNotificationUsage(deviceId: number, callback: (error: JabraError, result: enumNotificationUsage) => void): void;
    
    StoreColorControlPreset(deviceId: number, type: enumColorControlPreset, callback: (error: JabraError, result: void) => void): void;
    ApplyColorControlPreset(deviceId: number, type: enumColorControlPreset, callback: (error: JabraError, result: void) => void): void;
    StorePTZPreset(deviceId: number, type: enumPTZPreset, callback: (error: JabraError, result: void) => void): void;
    ApplyPTZPreset(deviceId: number, type: enumPTZPreset, callback: (error: JabraError, result: void) => void): void;
    ResetPanTiltZoom(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    ResetImageQualityControls(deviceId: number, callback: (error: JabraError, result: void) => void): void;
    
    SetSecondVideoStream(deviceId: number, stream: enumSecondaryStreamContent, callback: (error: JabraError, result: void) => void): void;
    GetSecondVideoStream(deviceId: number, callback: (error: JabraError, result: enumSecondaryStreamContent) => void): void;
    GetEthernetIPv4Status(deviceId: number, callback: (error: JabraError, result: IPv4Status) => void): void;
    GetWLANIPv4Status(deviceId: number, callback: (error: JabraError, result: IPv4Status) => void): void;
    GetUSBState(deviceId: number, callback: (error: JabraError, result: enumUSBState) => void): void;
  }
