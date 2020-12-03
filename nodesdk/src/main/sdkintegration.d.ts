/**
 * This declares the natively defined javascript types defined in the c++ code for the
 * N-API based sdk integration. The interface is not required/supported by N-API but 
 * supplied for maximum type safety internally for this module.
 */

import { ConfigParamsCloud, GenericConfigParams, enumHidState, AudioFileFormatEnum, DeviceSettings, DeviceInfo, PairedListInfo, 
         NamedAsset, AddonLogSeverity, JabraError, RemoteMmiActionOutput, DectInfo } from './core-types';
import { enumDeviceBtnType, enumFirmwareEventType, enumFirmwareEventStatus, 
         enumUploadEventStatus, enumBTPairedListType, enumRemoteMmiType, 
         enumRemoteMmiInput, enumRemoteMmiPriority, enumRemoteMmiSequence } from './jabra-enums';

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
               batteryStatusCallback: (deviceId: number, levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => void,
               onRemoteMmiEvent: (deviceId: number, type: enumRemoteMmiType, input: enumRemoteMmiInput) => void,
               downloadFirmwareProgressCallback: (deviceId: number, type: enumFirmwareEventType, status: enumFirmwareEventStatus, dwnFirmPercentage: number) => void,
               uploadProgressCallback: (deviceId: number, status: enumUploadEventStatus, percentage: number) => void,
               registerPairingListCallback: (deviceId: number, pairedListInfo: PairedListInfo) => void,
               onGNPBtnEventCallback: (deviceId: number, btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>) => void,
               dectInfoCallback: (deviceId: number, dectInfo: DectInfo) => void,
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
    GetWizardMode(deviceId: number, callback: (error: JabraError, result: number) => void): void;   
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
    SetWizardMode(deviceId: number, wizardModes:number, callback: (error: JabraError, result: void) => void): void;
    GetAudioFileParametersForUpload(deviceId: number, callback: (error: JabraError, result: { audioFileType: AudioFileFormatEnum, numChannels: number, bitsPerSample: number, sampleRate: number, maxFileSize: number }) => void): void;
    SetDatetime(deviceId: number, dateTime: { sec: number, min: number, hour: number, mday: number, mon: number, year: number, wday: number }, callback: (error: JabraError, result: void) => void): void;
    GetEqualizerParameters(deviceId: number, maxNBands:number, callback: (error: JabraError, result: Array<{ max_gain: number, centerFrequency: number, currentGain: number }>) => void): void;
    GetSupportedFeatures(deviceId: number, callback: (error: JabraError, result: Array<enumDeviceFeature>) => void): void;

    GetButtonFocus(deviceId: number, btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>, callback: (error: JabraError, result: void) => void): void;
    ReleaseButtonFocus(deviceId: number, btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>, callback: (error: JabraError, result: void) => void): void;

    GetRemoteMmiFocus(deviceId: number, type: enumRemoteMmiType, input: enumRemoteMmiInput, priority: enumRemoteMmiPriority, callback: (error: JabraError, result: void) => void): void;    
    ReleaseRemoteMmiFocus(deviceId: number, type: enumRemoteMmiType, callback: (error: JabraError, result: void) => void): void;
    IsRemoteMmiInFocus(deviceId: number, type: enumRemoteMmiType, callback: (error: JabraError, result: boolean) => void): void;
    SetRemoteMmiAction(deviceId: number, type: enumRemoteMmiType, actionOuput: RemoteMmiActionOutput, callback: (error: JabraError, result: void) => void): void;
}
