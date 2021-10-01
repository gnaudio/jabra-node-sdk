import { SdkIntegration } from "./sdkintegration";
import { AddonLogSeverity, DeviceTiming, DevLogData, AudioFileFormatEnum,
  RemoteMmiActionOutput, WhiteboardPosition, PanTilt, PanTiltLimits, WhiteBalance, DateTime, VideoLimits, IPv4Status, ZoomRelative, PanTiltRelative, VideoDeviceStreamingStatus, ProxySettings } from "./core-types";
import { isNodeJs } from './util';
import { _JabraNativeAddonLog } from './logger';

// Browser friendly type-only import:
type _EventEmitter = import('events').EventEmitter;

/** @internal */
let sdkIntegration: SdkIntegration;

/** @internal */
let events: any;

// @ts-ignore
if (isNodeJs()) { 
    // These statements should be executed under nodejs only to avoid browserfy problems:
    
    let bindings = require('bindings');
    sdkIntegration = bindings('sdkintegration');

    events = require('events');
} 

import { DeviceInfo, RCCStatus, ConfigInfo, ConfigParamsCloud, DeviceCatalogueParams,
    FirmwareInfoType, SettingType, DeviceSettings, PairedListInfo, NamedAsset,
    DectInfo } from './core-types';

import { enumAPIReturnCode, enumDeviceErrorStatus, enumDeviceBtnType, enumDeviceConnectionType,
    enumSettingDataType, enumSettingCtrlType, enumSettingLoadMode, enumFirmwareEventStatus,
    enumFirmwareEventType, enumBTPairedListType, enumUploadEventStatus,
    enumDeviceFeature, enumHidState, enumWizardMode, enumSecureConnectionMode,
    enumRemoteMmiType, enumRemoteMmiInput, enumRemoteMmiPriority, enumVideoMode,
    enumNotificationUsage, enumNotificationStyle, enumSecondaryStreamContent, enumPTZPreset,
    enumColorControlPreset, enumVideoTransitionStyle, enumIntelligentZoomLatency, enumUSBState } from './jabra-enums';
import * as _jabraEnums from './jabra-enums';

import { MetaApi, ClassEntry, _getJabraApiMetaSync } from './meta';

import * as util from 'util';
import { throws } from "assert";

export namespace DeviceTypeCallbacks {
    export type btnPress = (btnType: enumDeviceBtnType, value: boolean) => void;
    export type busyLightChange = (status: boolean) => void;
    export type downloadFirmwareProgress = (type: enumFirmwareEventType, status: enumFirmwareEventStatus, dwnldStatusInPrcntg: number) => void;
    export type onBTParingListChange = (pairedListInfo: PairedListInfo) => void;
    export type onGNPBtnEvent = (btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>) => void;
    export type onDevLogEvent = (data: DevLogData) => void;
    export type onDiagLogEvent = () => void;
    export type onBatteryStatusUpdate = (levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => void;
    export type onRemoteMmiEvent = (type: enumRemoteMmiType, input: enumRemoteMmiInput) => void;
    export type onxpressConnectionStatusEvent = (status: boolean) => void;
    export type onUploadProgress = (status: enumUploadEventStatus, levelInPercent: number) => void;
    export type onDectInfoEvent = (dectInfo: DectInfo) => void;
    export type onCameraStatusEvent = (status: boolean) => void;
}

export type DeviceTypeEvents = 'btnPress' | 'busyLightChange' | 'downloadFirmwareProgress' | 'onBTParingListChange' | 'onGNPBtnEvent' | 'onDevLogEvent' | 'onDiagLogEvent' | 'onBatteryStatusUpdate' | 'onRemoteMmiEvent'| 'onxpressConnectionStatusEvent' | 'onUploadProgress' | 'onDectInfoEvent' | 'onCameraStatusEvent';
export const DeviceEventsList : DeviceTypeEvents[] = ['btnPress', 'busyLightChange', 'downloadFirmwareProgress', 'onBTParingListChange', 'onGNPBtnEvent', 'onDevLogEvent', 'onDiagLogEvent', 'onBatteryStatusUpdate', 'onRemoteMmiEvent', 'onxpressConnectionStatusEvent', 'onUploadProgress', 'onDectInfoEvent', 'onCameraStatusEvent'];

/** 
 * Represents a concrete Jabra device and the operations that can be done on it.   
 */
export class DeviceType implements DeviceInfo, DeviceTiming, MetaApi {
    /** 
    * @internal 
    * @hidden
    */
    readonly _eventEmitter: _EventEmitter;

    /** 
     * @internal 
     * @hidden
     **/
    constructor(deviceInfo: DeviceInfo | DeviceType, attached_time_ms: number) {
        if (!isNodeJs()) {
            throw new Error("This JabraType constructor() function needs to run under NodeJs and not in a browser");
        }
        
        this._eventEmitter = new events.EventEmitter();

        this.deviceID = deviceInfo.deviceID;
        this.deviceName = deviceInfo.deviceName;
        this.productID = deviceInfo.productID;
        this.ESN = deviceInfo.ESN;
        this.vendorID = deviceInfo.vendorID;
        this.variant = deviceInfo.variant;
        this.connectionType = deviceInfo.connectionType;
        this.errorStatus = deviceInfo.errorStatus;
        this.isDongleDevice = deviceInfo.isDongleDevice;
        this.isInFirmwareUpdateMode = deviceInfo.isInFirmwareUpdateMode;
        this.attached_time_ms = attached_time_ms;
        this.detached_time_ms = undefined;
    }

    readonly ESN: string;
    readonly connectionType: enumDeviceConnectionType;
    readonly deviceID: number;
    readonly deviceName: string;
    readonly errorStatus: enumDeviceErrorStatus;
    readonly isDongleDevice: boolean;
    readonly isInFirmwareUpdateMode: boolean;
    readonly productID: number;
    readonly vendorID: number;
    readonly variant: string;

    /**
     * The time since EPOC that the device was attached.
     */
    readonly attached_time_ms: number;

    /**
     * The time since EPOC that the device was subsequently detached (if no longer attached only).
     * 
     */
    readonly detached_time_ms?: number;

    //CallControl
    /**
     * Checks for OffHook command support by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if supported, otherwise `false`.
     */
    isOffHookSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isOffHookSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsOffHookSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isOffHookSupportedAsync.name, "returned with", result);
            return result;
        });
    }
    /**
     * Checks for Mute command support by the device (Async).
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if supported, otherwise `false`.
     */
    isMuteSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isMuteSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsMuteSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isMuteSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks for Ringer command support by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if supported, otherwise `false`.
     */
    isRingerSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isRingerSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsRingerSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isRingerSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks for Hold support by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if supported, otherwise `false`.
     */
    isHoldSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isHoldSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsHoldSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isHoldSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks for Online mode support by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if supported, otherwise `false`.
     */
    isOnlineSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isOnlineSupportedAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.IsOnlineSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isOnlineSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks if busylight is supported by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if supported, otherwise `false`.
     */
    isBusyLightSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isBusyLightSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsBusyLightSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isBusyLightSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Set device's offhook state to true (Async).
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    offhookAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.offhookAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetOffHook)(this.deviceID, true).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.offhookAsync.name, "returned");
        });
    }

     /**
     * Set device's offhook state to false.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    onhookAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.onhookAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetOffHook)(this.deviceID, false).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.onhookAsync.name, "returned");
        });
    }

    /**
     * Set device's mute state to true i.e., device gets muted.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    muteAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.muteAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetMute)(this.deviceID, true).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.muteAsync.name, "returned");
        });
    }

     /**
     * Set device's mute state to false i.e., device gets unmuted.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    unmuteAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.unmuteAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetMute)(this.deviceID, false).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.unmuteAsync.name, "returned");
        });
    }

    /**
     * Set device's ringer state to true.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    ringAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.ringAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetRinger)(this.deviceID, true).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.ringAsync.name, "returned");
        });
    }

    /**
     * Set device's ringer state to false.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    unringAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.unringAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetRinger)(this.deviceID, false).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.unringAsync.name, "returned");
        });
    }

    /**
     * Set device's hold state to true.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    holdAsync(): Promise<void> {
       _JabraNativeAddonLog(AddonLogSeverity.verbose, this.holdAsync.name, "called with", this.deviceID); 
       return util.promisify(sdkIntegration.SetHold)(this.deviceID, true).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.holdAsync.name, "returned");
       });
    }

    /**
     * Set device's hold state to false.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    resumeAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resumeAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetHold)(this.deviceID, false).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resumeAsync.name, "returned");
        });
    }

    /**
     * It opens radio link between base/dongle and device.
     * @param {boolean} online - Boolean value to set Online On/Off
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
    
     */
    setOnlineAsync(online: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setOnlineAsync.name, "called with", this.deviceID, online); 
        return util.promisify(sdkIntegration.SetOnline)(this.deviceID, online).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setOnlineAsync.name, "returned");
        });
    }

    /**
     * Set busylight status (Async).
     * @param {boolean} status - Boolean value to set busylight on / off.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setBusyLightStatusAsync(status: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setBusyLightStatusAsync.name, "called with", this.deviceID, status); 
        return util.promisify(sdkIntegration.SetBusyLightStatus)(this.deviceID, status).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setBusyLightStatusAsync.name, "returned");
        });
    }

    /**
     * Checks the status of busylight.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - `true` if busylight is on, `false` if busylight is off or if it is not supported.
     */
    getBusyLightStatusAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBusyLightStatusAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetBusyLightStatus)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBusyLightStatusAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get serial number.
     * @returns {Promise<string, JabraError>} - Resolve `string` if successful otherwise Reject with `error`.
     */
    getSerialNumberAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSerialNumberAsync.name, "called with", this.deviceID); 
        return this.ESN ? Promise.resolve(this.ESN) : Promise.reject(new Error("No serial number")).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSerialNumberAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get ESN (electronic serial number).
     * @returns {Promise<string, JabraError>} - Resolve `string` if successful otherwise Reject with `error`.
     */
    getESNAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getESNAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetESN)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getESNAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get asset with name.
     * @returns {Promise<NamedAsset, JabraError>} - Resolve NamedAsset `object` if successful otherwise Reject with `error`.
     */
    getNamedAssetAsyngetNamec(assetName: string): Promise<NamedAsset> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getNamedAssetAsyngetNamec.name, "called with", this.deviceID, assetName); 
        return util.promisify(sdkIntegration.GetNamedAsset)(this.deviceID, assetName).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getNamedAssetAsyngetNamec.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get battery status, if supported by device.
     * @returns {Promise<BatteryInfo, JabraError>} - Resolve batteryInfo `object` if successful otherwise Reject with `error`.
    
     */
    getBatteryStatusAsync(): Promise<{ levelInPercent?: number, isCharging?: boolean, isBatteryLow?: boolean }> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBatteryStatusAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetBatteryStatus)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBatteryStatusAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get remote control battery status, if supported by device.
     * @returns {Promise<BatteryInfo, JabraError>} - Resolve batteryInfo `object` if successful otherwise Reject with `error`.
    
     */
     getRemoteControlBatteryStatusAsync(): Promise<{ levelInPercent?: number, isCharging?: boolean, isBatteryLow?: boolean }> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRemoteControlBatteryStatusAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetRemoteControlBatteryStatus)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRemoteControlBatteryStatusAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Gets  the device image path.
     * @returns {Promise<string, JabraError>} - Resolve imagePath `string` if successful otherwise Reject with `error`.
   
     */
    getImagePathAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getImagePathAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetDeviceImagePath)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getImagePathAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Gets  the device image thumbnail path.
     * @returns {Promise<string, JabraError>} - Resolve image Thumbnail Path `string` if successful otherwise Reject with `error`.
     */
    getImageThumbnailPathAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getImageThumbnailPathAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetDeviceImageThumbnailPath)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getImageThumbnailPathAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Preloads the resources with the content of the specified archive for a specific device after this being attached.
     * @returns {Promise<void, JabraError>} - if operation failed, reject with `JabraError`.
     */
    preloadAttachedDeviceInfoAsync(zipFileName: string): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.preloadAttachedDeviceInfoAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.PreloadAttachedDeviceInfo)(this.deviceID, zipFileName).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.preloadAttachedDeviceInfoAsync.name, "returned");
        });
    }

    /**
     * Gets the Manifest Files' version that are found locally in the Resources folder for a given device.
     * @returns {Promise<string, JabraError>} - Returns a `string` with the version number if successful, otherwise reject with `JabraError`.
     */
    getLocalManifestVersionAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getLocalManifestVersionAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetLocalManifestVersion)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getLocalManifestVersionAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Check if battery is supported.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     */
    isBatterySupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isBatterySupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsBatteryStatusSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isBatterySupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Is logging enabled on device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     */
    isDevLogEnabledAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isDevLogEnabledAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsDevLogEnabled)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isDevLogEnabledAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Enable/disable logging for a device.
     * @param {boolean} enable - whether to enable device log.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    enableDevLogAsync(enable: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableDevLogAsync.name, "called with", this.deviceID, enable); 
        return util.promisify(sdkIntegration.EnableDevLog)(this.deviceID, enable).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableDevLogAsync.name, "returned");
        });
    }

    // settings APIs
    /**
     * Gets the complete settings details (all groups and its settings) for a device.
     * @returns {Promise<Array<Setting>, JabraError>}  - Resolve setting `array` if successful otherwise Reject with `error`.
     */
    getSettingsAsync(): Promise<DeviceSettings> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSettingsAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetSettings)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSettingsAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Gets the unique setting identified by a GUID of a device.
     * @param {string} guid - the unique setting identifier.
     * @returns {Promise<Array<DeviceSettings>, JabraError>}  - Resolve setting `array` if successful otherwise Reject with `error`.
     */
    getSettingAsync(guid: string): Promise<DeviceSettings> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSettingAsync.name, "called with", this.deviceID, guid); 
        return util.promisify(sdkIntegration.GetSetting)(this.deviceID, guid).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSettingAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Sets all the settings( including all groups and its settings) for a device.
     * @param {Array<DeviceSettings>} settings - pass only changed settings in an array
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`, `reboot` etc.
     * 
     * Nb. Currently this method returns an error with code=24 if rebooting as bi-result. 
     * TODO: Change signature to return reboot information normally instead.
     */
    setSettingsAsync(settings: DeviceSettings): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSettingsAsync.name, "called with", settings); 
        return util.promisify(sdkIntegration.SetSettings)(this.deviceID, settings).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSettingsAsync.name, "returned");
        });
    }

    /**
     * Gets the minimum time in seconds to stay with a participant before being allowed to change zoom/direction.
     * @returns {Promise<enumIntelligentZoomLatency, JabraError>}  - Resolve setting `enumIntelligentZoomLatency`
     * if successful otherwise Reject with `error`.
     */
    getIntelligentZoomLatencyAsync(): Promise<enumIntelligentZoomLatency> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getIntelligentZoomLatencyAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetIntelligentZoomLatency)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getIntelligentZoomLatencyAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Controls how often the camera is allowed to change speaker focus.
     * @param {enumIntelligentZoomLatency} latency - The minimum time in seconds to stay with a participant before being
     *                                               allowed to change zoom/direction. Valid range 0-30 seconds.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`
     */
    setIntelligentZoomLatencyAsync(latency: enumIntelligentZoomLatency): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setIntelligentZoomLatencyAsync.name, "called with", latency); 
        return util.promisify(sdkIntegration.SetIntelligentZoomLatency)(this.deviceID, latency).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setIntelligentZoomLatencyAsync.name, "returned");
        });
    }

    /**
     * Restore factory settings to device.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    resetSettingsAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resetSettingsAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.FactoryReset)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resetSettingsAsync.name, "returned");
        });
    }

    /**
     * Get call lock for a particular device.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    getLockAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getLockAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetLock)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getLockAsync.name, "returned");
        });
    }

    /**
     * Release our call lock for a particular device.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    releaseLockAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.releaseLockAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.ReleaseLock)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.releaseLockAsync.name, "returned");
        });
    }

    /**
     * Check if we have call lock on device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     */
    isLockedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isLockedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsLocked)(this.deviceID).then((isLocked) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isLockedAsync.name, "returned");
            return isLocked;
        });
    }

    /**
     * Select how the video viewport is managed (runtime).
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setVideoModeAsync(mode: enumVideoMode): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoModeAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetVideoMode)(this.deviceID, mode).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoModeAsync.name, "returned");
        });
    }

    /**
     * Gets how the video viewport is managed (runtime).
     * @returns {Promise<enumVideoMode, JabraError>} - Resolve `enumVideoMode` if successful otherwise Reject with `error`.
     */
    getVideoModeAsync(): Promise<enumVideoMode> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoModeAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetVideoMode)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoModeAsync.name, "returned");
            return result;
        });
    }

    /**
     * Enables or disables Picture-In-Picture on a supported video device
     * @param {enable} - The enable state
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setPictureInPictureAsync(enable: boolean) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPictureInPictureAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetPictureInPicture)(this.deviceID, enable).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPictureInPictureAsync.name, "returned");
        });
    }

    /**
     * Gets whether Picture-In-Picture is active on a supported video device
     * @returns {Promise<boolean, JabraError>} - Resolves to `boolean` result on success,
     *    rejects with `JabraError` on error.
     */
    getPictureInPictureAsync() : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPictureInPictureAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetPictureInPicture)(this.deviceID).then((enable) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPictureInPictureAsync.name, "returned");
          return enable;
        });
    }

    /**
     * Resets runtime pan, tilt and zoom settings to factory defaults on a video device.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    resetPanTiltZoom() : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resetPanTiltZoom.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.ResetPanTiltZoom)(this.deviceID).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resetPanTiltZoom.name, "returned");
        });
    }

    /**
     * Resets runtime image quality settings to factory defaults on a video device.
     * IQ is Brightness+Contrast+Sharpness+Saturation+White Balance.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    resetImageQualityControls() : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resetImageQualityControls.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.ResetImageQualityControls)(this.deviceID).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.resetImageQualityControls.name, "returned");
        });
    }

    /**
     * Gets whether the camera is actively streaming.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     */
    getIsCameraStreamingAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getIsCameraStreamingAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsCameraStreaming)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getIsCameraStreamingAsync.name, "returned");
            return result;
        });
    }

    /**
     * Gets whether a video is actively streaming video and/or audio.
     * @returns {Promise<VideoDeviceStreamingStatus, JabraError>} - Resolve `VideoDeviceStreamingStatus` if successful otherwise Reject with `error`.
     */
     isVideoDeviceStreamingAsync(): Promise<VideoDeviceStreamingStatus> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isVideoDeviceStreamingAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsVideoDeviceStreaming)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isVideoDeviceStreamingAsync.name, "returned");
            return result;
        });
    }

    /**
     * Checks if supports factory reset.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if device supports factory reset, `false` if device does not support factory reset.
     */
    isFactoryResetSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isFactoryResetSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsFactoryResetSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isFactoryResetSupportedAsync.name, "returned with", result);
            return result;
        });
    }
       
    /**
     * Returns error description for the error code
     * @returns {Prmoise<Array<string>, JabraError>} - Resolve failedSetting `array` if successful otherwise Reject with `error`.
     * - return FailedSettings if one or more settings are failed while writing to device.
     * - return empty array if all settings are written successfully.
     * - **Note**: This API should be called if setSettingsAsync API does not return void.
     */
    getFailedSettingNamesAsync(): Promise<Array<string>>{
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getFailedSettingNamesAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetFailedSettingNames)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getFailedSettingNamesAsync.name, "returned with", result);
            return result;
        });
    }

    // firmware APIs

    /**
     * Get firmware version of the device.
     * @returns {Promise<string, JabraError>} - Resolve version `string` if successful otherwise Reject with `error`.
     */
    getFirmwareVersionAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getFirmwareVersionAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetFirmwareVersion)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getFirmwareVersionAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Gets details of the latest firmware present in cloud.
     * @param {string} [authorization] - Authorization Id.
     * @returns {Promise<FirmwareInfo, JabraError>} - Resolve firminfo `object` if successful otherwise Reject with `error`.
     */
    getLatestFirmwareInformationAsync(authorization?: string): Promise<FirmwareInfoType> {
        const _authorization =  authorization || "";
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getLatestFirmwareInformationAsync.name, "called with", this.deviceID, _authorization); 
        return util.promisify(sdkIntegration.GetLatestFirmwareInformation)(this.deviceID, _authorization).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getLatestFirmwareInformationAsync.name, "returned with", result);
            return result;
        });
    }
    /**
     * Check if Firmware update available for device.
     * @param {string} [authorization] - authorizationId
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - `true` - if a new firmware update is available (existing firmware not up to date), `false` otherwise (existing firmware up to date).
     */
    checkForFirmwareUpdateAsync(authorization?: string): Promise<boolean> {
        const _authorization =  authorization || "";
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.checkForFirmwareUpdateAsync.name, "called with", this.deviceID, _authorization);
        return util.promisify(sdkIntegration.CheckForFirmwareUpdate)(this.deviceID, _authorization).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.checkForFirmwareUpdateAsync.name, "returned with", result);
            return result;
        });
    }
    /**
     * Downloads the specified firmware version file.
     * @param {string} version - Version for which file download needs to be initiated.
     * @param {string} [authorization] - Authorization Id.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.   
     */
    downloadFirmwareAsync(version: string, authorization?: string): Promise<void> {
        const _authorization =  authorization || "";
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.downloadFirmwareAsync.name, "called with", this.deviceID, _authorization);
        return util.promisify(sdkIntegration.DownloadFirmware)(this.deviceID, version, _authorization).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.downloadFirmwareAsync.name, "returned");
        });
    }

    /**
     * Get the file path of the downloaded file.
     * @param {string} version - Version for which the path is required.
     * @returns {Promise<string, JabraError>} - Resolve firmware file path `string` if successful otherwise Reject with `error`.
     * - **Note**: Call `downloadFirmwareAsync` first to ensure that data is current
     */
    getFirmwareFilePathAsync(version: string): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getFirmwareFilePathAsync.name, "called with", this.deviceID, version); 
        return util.promisify(sdkIntegration.GetFirmwareFilePath)(this.deviceID, version).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getFirmwareFilePathAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Cancels the firmware download (Async).
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    cancelFirmwareDownloadAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.cancelFirmwareDownloadAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.CancelFirmwareDownload)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.cancelFirmwareDownloadAsync.name, "returned");
        });
    }
    /**
     * Upgrades / Updates the firmware for the target device with specified version.
     * @param {string} firmwareFilePath - firmware file path.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.  
     */
    updateFirmwareAsync(firmwareFilePath: string): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.updateFirmwareAsync.name, "called with", this.deviceID, firmwareFilePath); 
        return util.promisify(sdkIntegration.UpdateFirmware)(this.deviceID, firmwareFilePath).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.updateFirmwareAsync.name, "returned");
        });
    }

    // bluetooth APIs
    /**
     * Set the bluetooth device in pairing mode.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setBTPairingAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setBTPairingAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SetBTPairing)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setBTPairingAsync.name, "returned");
        });
    }
    /**
     * Stop search for available Bluetooth devices.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    stopBTPairingAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.stopBTPairingAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.StopBTPairing)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.stopBTPairingAsync.name, "returned");
        });
    }
    
    /**
     * Search for available Bluetooth devices which are switched on, within range and ready to connect.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    searchNewDevicesAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.searchNewDevicesAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.SearchNewDevices)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.searchNewDevicesAsync.name, "returned");
        });
    }
    /**
     * Connect/Reconnect Bluetooth device to the Jabra Bluetooth adapter. Ensure the Bluetooth device is switched on and within range.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    connectBTDeviceAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectBTDeviceAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.ConnectBTDevice)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectBTDeviceAsync.name, "returned");
        });
    }
    /**
     * Connect a new device.
     * @param {string} deviceName - name of device to be connected.
     * @param {string} deviceBTAddr -  BTAddress of device to be connected.
     * @param {boolean} isConnected - current status of device to be connected.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
    */
    connectNewDeviceAsync(deviceName: string, deviceBTAddr: string, isConnected: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectNewDeviceAsync.name, "called with", this.deviceID, deviceName, deviceBTAddr, isConnected);
        return util.promisify(sdkIntegration.ConnectNewDevice)(this.deviceID, deviceName, deviceBTAddr, isConnected).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectNewDeviceAsync.name, "returned");
        });
    }
    /**
     * Connect a device which was already paired.
     * @param {string} deviceName - name of device to be connected.
     * @param {string} deviceBTAddr -  BTAddress of device to be connected.
     * @param {boolean} isConnected - current status of device to be connected.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     * - **Note**       : After device connection, getPairingListAsync api has to be called to get updated connection status.
     */
    connectPairedDeviceAsync(deviceName: string, deviceBTAddr: string, isConnected: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectPairedDeviceAsync.name, "called with", this.deviceID, deviceName, deviceBTAddr, isConnected);
        return util.promisify(sdkIntegration.ConnectPairedDevice)(this.deviceID, deviceName, deviceBTAddr, isConnected).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectPairedDeviceAsync.name, "returned");
        });
    }

    /**
     * Disconnect  Bluetooth device from  Bluetooth adapter.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    disconnectBTDeviceAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.disconnectBTDeviceAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.DisconnectBTDevice)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.disconnectBTDeviceAsync.name, "returned");
        });
    }

    /**
     * Disconnect a paired device.
     * @param {string} deviceName - name of device to be disconnected.
     * @param {string} deviceBTAddr -  BTAddress of device to be disconnected.
     * @param {boolean} isConnected - current status of device to be disconnected.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     * - **Note**       : After device disconnection, getPairingListAsync api has to be called to get updated connection status.
     */
    disconnectPairedDeviceAsync(deviceName: string, deviceBTAddr: string, isConnected: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.disconnectPairedDeviceAsync.name, "called with", this.deviceID, deviceName, deviceBTAddr, isConnected); 
        return util.promisify(sdkIntegration.DisconnectPairedDevice)(this.deviceID, deviceName, deviceBTAddr, isConnected).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.disconnectPairedDeviceAsync.name, "returned");
        });
    }

    /**
     * When Bluetooth adapter is plugged into the PC it will attempt to connect with the last connected Bluetooth device. If it cannot connect, it will automatically search for new Bluetooth devices to connect to.
     * @param {boolean} value - enable or disable for auto pairing.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setAutoPairingAsync(value: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setAutoPairingAsync.name, "called with", this.deviceID, value); 
        return util.promisify(sdkIntegration.SetAutoPairing)(this.deviceID, value).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setAutoPairingAsync.name, "returned");
        });
    }

    /**
     * Get Auto pairing mode enable or disable.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - `true` if auto pairing mode is enabled, `false` otherwise.
     */
    getAutoPairingAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getAutoPairingAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetAutoPairing)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getAutoPairingAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks if pairing list is supported by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if pairing list is supported, false if device does not support pairing list.
     */
    isPairingListSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isPairingListSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsPairingListSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isPairingListSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Gets the list of devices which are paired previously.
     * @returns { Promise<Array<PairedDevice>, JabraError>} - Resolve pairList `array` if successful otherwise Reject with `error`.
     */
    getPairingListAsync(): Promise<Array<{ deviceName: string, deviceBTAddr: string, isConnected: boolean }>> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPairingListAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetPairingList)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPairingListAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Clear list of paired BT devices from BT adapter.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    clearPairingListAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.clearPairingListAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.ClearPairingList)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.clearPairingListAsync.name, "returned");
        });
    }

    /**
     * Get name of connected BT device with BT Adapter(Async).
     * @returns {Promise<string, JabraError>} - Resolve deviceName `string` if successful otherwise Reject with `error`.
     */
    getConnectedBTDeviceNameAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getConnectedBTDeviceNameAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetConnectedBTDeviceName)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getConnectedBTDeviceNameAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Gets the list of new devices which are available to pair & connect.
     * @returns { Promise<Array<PairedDevice>, JabraError>} - Resolve pairList `array` if successful otherwise Reject with `error`.
     * - **Note**: `isConnected`, flag in Pairing List Object, will always be false as device does not give connection status for the found device.
     */
    getSearchDeviceListAsync(): Promise<Array<{ deviceName: string, deviceBTAddr: string, isConnected: boolean }>> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSearchDeviceListAsync.name, "called with", this.deviceID); 
	    return util.promisify(sdkIntegration.GetSearchDeviceList)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSearchDeviceListAsync.name, "returned with", result);
            return result;
        });
    }

    //RMMI APIs
    /**
     * Gets the supported remote MMI for a device.
     * @returns {Promise<Array<ButtonEvent>, JabraError>} - Resolve btnEvent `array` if successful otherwise Reject with `error`.
     */
    getSupportedButtonEventsAsync(): Promise<Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSupportedButtonEventsAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetSupportedButtonEvents)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSupportedButtonEventsAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Configures the remote MMI events for a device.
     * @param {Array<{buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{key: number, value: string}>}>} btnEvents
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    getButtonFocusAsync(btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getButtonFocusAsync.name, "called with", this.deviceID, btnEvents);
        return util.promisify(sdkIntegration.GetButtonFocus)(this.deviceID, btnEvents).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getButtonFocusAsync.name, "returned");
        });
    }

    /**
     * Releases the remote MMI events configured in the device.
     * @param {Array<{buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{key: number, value: string}>}>} btnEvents
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    releaseButtonFocusAsync(btnEvents: Array<{ buttonTypeKey: number, buttonTypeValue: string, buttonEventType: Array<{ key: number, value: string }> }>): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.releaseButtonFocusAsync.name, "called with", this.deviceID, btnEvents);
        return util.promisify(sdkIntegration.ReleaseButtonFocus)(this.deviceID, btnEvents).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.releaseButtonFocusAsync.name, "returned");
        });
    }        

    /**
     * Checks if Upload Ringtone to the device is supported by the device.
     * @returns {Promise<boolean>, Error} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if Upload Ringtone to the device is supported, `false` if device does not support the ringtone upload to the device.
     */
    isUploadRingtoneSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isUploadRingtoneSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsUploadRingtoneSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isUploadRingtoneSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Upload ringtone to device  (Async).
     * @param {string} filePath filepath of image file to be uploaded.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    uploadRingtoneAsync(filePath: string): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.uploadRingtoneAsync.name, "called with", this.deviceID, filePath);
        return util.promisify(sdkIntegration.UploadRingtone)(this.deviceID, filePath).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.uploadRingtoneAsync.name, "returned");
        });
    }

    /**
     * Get details of audio file for uploading to device.
     * @returns {Promise<{ audioFileType: AudioFileFormatEnum, numChannels: number, bitsPerSample: number, sampleRate: number, maxFileSize: number }, JabraError>} - Resolve Audio File Detail `object` if successful otherwise Reject with `error`.
     */
    getAudioFileParametersForUploadAsync(): Promise<{ audioFileType: AudioFileFormatEnum, numChannels: number, bitsPerSample: number, sampleRate: number, maxFileSize: number }> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getAudioFileParametersForUploadAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetAudioFileParametersForUpload)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getAudioFileParametersForUploadAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Upload ringtone to device in .wav format.
     * @param {string} filePath filepath of image file to be uploaded.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    uploadWavRingtoneAsync(filePath: string): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.uploadWavRingtoneAsync.name, "called with", this.deviceID, filePath); 
        return util.promisify(sdkIntegration.UploadWavRingtone)(this.deviceID, filePath).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.uploadWavRingtoneAsync.name, "returned");
        });
    }

    /**
     * Sets the defined date and time on device (Async).
     * @param {DateTime} timedate Date/time setting to be set on device
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setDateTimeAsync(timedate: DateTime): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setDateTimeAsync.name, "called with", this.deviceID, timedate); 
       return util.promisify(sdkIntegration.SetDatetime)(this.deviceID, timedate).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setDateTimeAsync.name, "returned with");
    });
    }
    
    /**
     * Gets the current date and time setting from device (Async).
     * @returns {Promise<DateTime, JabraError>} - Resolve `DateTime` if successful otherwise Reject with `error`.
     */
    getDateTimeAsync(): Promise<DateTime> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getDateTimeAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetDatetime)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getDateTimeAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks if date and time can be configured to device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`. 
     * - Returns `true` if configuring time for device is supported, `false` if device does not support date and time configuration.
     */
    isSetDateTimeSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isSetDateTimeSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsSetDateTimeSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isSetDateTimeSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Checks if image upload is supported by the device.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`. 
     * - Returns `true` if device supports image upload otherwise `false`.
     */
    isUploadImageSupportedAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isUploadImageSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsUploadImageSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isUploadImageSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Upload image to device.
     * @param {string} filePath filepath of image file to be uploaded.
     * @return {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`. 
     */
    uploadImageAsync(filePath: string): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.uploadImageAsync.name, "called with", this.deviceID, filePath); 
        return util.promisify(sdkIntegration.UploadImage)(this.deviceID, filePath).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.uploadImageAsync.name, "returned");
        });
    }

    /**
     * Checks if setting protection is enabled.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if setting protection is enabled otherwise `false`.
     */
    isSettingProtectionEnabledAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isSettingProtectionEnabledAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsSettingProtectionEnabled)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isSettingProtectionEnabledAsync.name, "returned with", result);
            return result;
        });
    }

    /**  
     * Get the panic list.
     * @returns {Promise<Array<string>, JabraError>} - Resolve paniclist 'array' if successful otherwise Reject with `error`.
     * - panic code will be hex string
     */
    getPanicsAsync(): Promise<Array<string>> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPanicsAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetPanics)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPanicsAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Check if a feature is supported by a device.
     * @param {number} deviceFeature the feature to check, should be `enumDeviceFeature`
     * @returns {Promise<boolean, JabraError>} - Resolve isfeatureSupports `boolean` if successful otherwise Reject with `error`.
     */
    isFeatureSupportedAsync(deviceFeature: number): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isFeatureSupportedAsync.name, "called with", this.deviceID, deviceFeature);
        return util.promisify(sdkIntegration.IsFeatureSupported)(this.deviceID,deviceFeature).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isFeatureSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Check if GN or Std HID state is supported by a device.
     *  @returns {Promise<boolean, JabraError>} - Resolve isGnHidStdHidSupported `boolean` if successful otherwise Reject with `error`.
     */
    isGnHidStdHidSupportedAsync(): Promise<boolean>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isGnHidStdHidSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsGnHidStdHidSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isGnHidStdHidSupportedAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get array of features supported by a device
     * @returns { Promise<Array<enumDeviceFeature>, JabraError>} 
     * - Resolve array of supported features, should be enumDeviceFeature `array` if successful otherwise Reject with `error`.
     */
    getSupportedFeaturesAsync(): Promise<Array<enumDeviceFeature>> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSupportedFeaturesAsync.name, "called with", this.deviceID); 
         return util.promisify(sdkIntegration.GetSupportedFeatures)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSupportedFeaturesAsync.name, "returned with", result);
            return result;
        });
    }
     
    /**
     * Sets the HID working state to either standard HID (usb.org HID specification) or GN HID.
     * @param {number} hidState - state HID working state (`enumHidState`)
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setHidWorkingStateAsync(hidState: enumHidState): Promise<void>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setHidWorkingStateAsync.name, "called with", this.deviceID, hidState);
        return util.promisify(sdkIntegration.SetHidWorkingState)(this.deviceID, hidState).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setHidWorkingStateAsync.name, "returned");
        });
    }

    /**
     * Gets the HID working state.
     * @returns {Promise<number, JabraError>} - Resolve hidState (`enumHidState`) if successful otherwise Reject with `error`.
     */
    getHidWorkingStateAsync() : Promise<enumHidState>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getHidWorkingStateAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetHidWorkingState)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getHidWorkingStateAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Sets the wizard mode (whether a full setup wizard, a limited setup
     * wizard or none will run on next power-on). Use isfeatureSupportedAsync
     * to query feature support enumDeviceFeature.FullWizardMode or
     * enumDeviceFeature.LimitedWizardMode.
     * @param {number} wizardMode Wizard mode to be set (one of WizardModes).
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setWizardModeAsync(wizardMode: enumWizardMode) : Promise<void>  {
       _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWizardModeAsync.name, "called with", this.deviceID, wizardMode); 
       return util.promisify(sdkIntegration.SetWizardMode)(this.deviceID,wizardMode).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWizardModeAsync.name, "returned");
       });
    }

    /**
    * Reads the current wizard mode (whether a full setup wizard, a limited
    * setup wizard or none will run on next power-on). 
    * Use isFeatureSupportedAsync to query feature support
    * enumDeviceFeature.FullWizardMode or enumDeviceFeature.LimitedWizardMode.
    * @returns {Promise<number, JabraError>} 
    * - Resolve enumWizardMode Current wizard mode (one of WizardModes) `number` if successful otherwise Reject with `error`.
    */ 
    getWizardModeAsync() : Promise<enumWizardMode>  {
       _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWizardModeAsync.name, "called with", this.deviceID); 
       return util.promisify(sdkIntegration.GetWizardMode)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWizardModeAsync.name, "returned with", result);
        return result;
       });
    }

    /**
    * Reads the secure connection status (whether it is in legacy mode, secure mode or restricted) 
    * @returns {Promise<number, JabraError>} 
    * - Resolve enumSecureCommectionMode secure connection mode (one of SecureConnectionModes) `number` if successful otherwise Reject with `error`.
    */ 
    getSecureConnectionModeAsync() : Promise<enumSecureConnectionMode>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSecureConnectionModeAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetSecureConnectionMode)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSecureConnectionModeAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Reboot device.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    rebootDeviceAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.rebootDeviceAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.RebootDevice)(this.deviceID).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.rebootDeviceAsync.name, "returned");
        });
    }

     /**
     * Clear a device from paired device list.
     * @param {string} deviceName - name of device to be connected.
     * @param {string} deviceBTAddr -  BTAddress of device to be connected.
     * @param {boolean} isConnected - current status of device to be connected.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    clearPairedDeviceAsync(deviceName: string, deviceBTAddr: string, isConnected: boolean): Promise<void>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.clearPairedDeviceAsync.name, "called with", this.deviceID, deviceName, deviceBTAddr, isConnected);
        return util.promisify(sdkIntegration.ClearPairedDevice)(this.deviceID, deviceName, deviceBTAddr, isConnected).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.clearPairedDeviceAsync.name, "returned");
        });
    }

    /**
     * Downloads the latest FW updater relevant for this device
     * @param {string} [authorization] - Authorization Id.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    downloadFirmwareUpdaterAsync(authorization?: string): Promise<void>  {
        const _authorization = authorization || "";
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.downloadFirmwareUpdaterAsync.name, "called with", this.deviceID, _authorization);
        return util.promisify(sdkIntegration.DownloadFirmwareUpdater)(this.deviceID, _authorization).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.downloadFirmwareUpdaterAsync.name, "returned");
        });
    }
    
    /**
     * Sets a static timestamp in the device. Can be used for later referencing using Jabra_GetTime.
     * @param {Number} timeStamp - Timestamp to be set. Unix epoch.
     * @return {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setTimestampAsync(timeStamp: number) : Promise<void>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setTimestampAsync.name, "called with", this.deviceID, timeStamp);
        return util.promisify(sdkIntegration.SetTimestamp)(this.deviceID, timeStamp).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setTimestampAsync.name, "returned");
        });
    }
    
    /**
     * Gets the static timestamp in the device.
     * @returns {Promise<number, JabraError>} - Resolve Date in milliseconds `number` if successful otherwise Reject with `error`.
     */
    getTimestampAsync() : Promise<number>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getTimestampAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.GetTimestamp)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getTimestampAsync.name, "returned with", result);
            return result;
        });
    }

    /**
    * Play Ringtone in Device.
    * @param {number} level volume Level to Play.
    * @param {number} type ringtone Type to Play.
    * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
    */
    playRingtoneAsync(level: number, type: number): Promise<void>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.playRingtoneAsync.name, "called with", this.deviceID, level, type);
        return util.promisify(sdkIntegration.PlayRingTone)(this.deviceID,level,type).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.playRingtoneAsync.name, "returned");
        });
    }

    /**
    * Checks if equalizer is supported by the device.
    * @returns {Promise<boolean, JabraError>} - Resolve True if equalizer is supported, false if device does not support
    * equalizer otherwise Reject with `error`.
    */
   isEqualizerSupportedAsync(): Promise<boolean>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isEqualizerSupportedAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsEqualizerSupported)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isEqualizerSupportedAsync.name, "returned with", result);
            return result;
        });
   }
    
   /**
   * Checks if equalizer is enabled.
   * @returns {Promise<boolean, JabraError>} - Resolve True if equalizer is enabled, false if equalizer is disabled or not
   * supported by the device otherwise Reject with `error`.
   */
   isEqualizerEnabledAsync(): Promise<boolean>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isEqualizerEnabledAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsEqualizerEnabled)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isEqualizerEnabledAsync.name, "returned with", result);
            return result;
        });
   }

   /**
   * Enable/disable equalizer.
   * @param {boolean} enable Enable or disable equalizer.
   * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
   */
   enableEqualizerAsync(enable: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableEqualizerAsync.name, "called with", this.deviceID, enable); 
        return util.promisify(sdkIntegration.EnableEqualizer)(this.deviceID, enable).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableEqualizerAsync.name, "returned");
        });
   }

   /**
   * Get equalizer parameters.
   * @param {number} maxBands Max no of bands to return (default is 5)
   * @returns {Promise<Array<EqualizerBand>, JabraError>} - Resolve equalizerBand `object` if successful otherwise Reject with `error`.
   */
   getEqualizerParametersAsync(maxNBands?: number): Promise<Array<{ max_gain: number, centerFrequency: number, currentGain: number }>> {
        const _maxNBands = maxNBands || 5;
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getEqualizerParametersAsync.name, "called with", this.deviceID, _maxNBands);
        return util.promisify(sdkIntegration.GetEqualizerParameters)(this.deviceID, _maxNBands);
   }

   /**
    * Set equalizer parameters
    * @param {Array<number>} bands Caller-owned array containing the band gains to set in dB
    * (must be within range of +/- max_gain).
    * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
   */
   setEqualizerParametersAsync(bands: Array<number>): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setEqualizerParametersAsync.name, "called with", this.deviceID, bands);
        return util.promisify(sdkIntegration.SetEqualizerParameters)(this.deviceID, bands).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setEqualizerParametersAsync.name, "returned");
        });
   }

    /**
     * Checks if firmware lock is enabled. If the firmware lock is enabled
     * it is not possible to upgrade nor downgrade the firmware. In this situation
     * the firmware can only be changed to the same version e.g. if you want to
     * change the language.
     *  @returns {Promise<boolean, JabraError>} - Resolve isFirmwareLockEnabled `boolean` if successful otherwise Reject with `error`.
     */
   isFirmwareLockEnabledAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isFirmwareLockEnabledAsync.name, "called with", this.deviceID); 
        return util.promisify(sdkIntegration.IsFirmwareLockEnabled)(this.deviceID).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isFirmwareLockEnabledAsync.name, "returned with", result);
            return result;
        });
   }

     /**
     * Enable/disable the firmware lock. if the firmware is locked the device should not be upgraded.
     * @param {boolean} enable - whether to lock the firmware in the device.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    enableFirmwareLockAsync(enable: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableFirmwareLockAsync.name, "called with", this.deviceID, enable); 
        return util.promisify(sdkIntegration.EnableFirmwareLock)(this.deviceID, enable).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableFirmwareLockAsync.name, "returned");
        });
    }

    /**
     * Gets the focus of the remote MMI specified. Once a remote MMI has
     * focus, the normal functionality of the MMI (button/LED) is suppressed until
     * #releaseRemoteMmiFocusAsync is called.
     * If only the LED output MMI functionality is required, action can be
     * specified as MMI_ACTION_NONE.
     * @param {enumRemoteMmiType} type Type of remote MMI to get focus of.
     * @param {enumRemoteMmiInput} input Action to get focus of, acts as a filter/mask for the
     * actions on the RemoteMmiCallback callback
     * @param {enumRemoteMmiPriority} priority Priority of focus.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
    */
    getRemoteMmiFocusAsync(type: enumRemoteMmiType, input: enumRemoteMmiInput, priority: enumRemoteMmiPriority) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRemoteMmiFocusAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetRemoteMmiFocus)(this.deviceID, type, input, priority).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRemoteMmiFocusAsync.name, "returned");
            return result;
        });
    }

    /**
     * Releases the focus of the remote MMI specified. Note that focus on
     * all actions are removed.
     * @param {enumRemoteMmiType} type Type of remote MMI to release focus of.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
    */    
    releaseRemoteMmiFocusAsync(type: enumRemoteMmiType) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.releaseRemoteMmiFocusAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.ReleaseRemoteMmiFocus)(this.deviceID, type).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.releaseRemoteMmiFocusAsync.name, "returned");
            return result;
        });
    }    

    /**
     * Gets the status of the remote MMI focus.
     * @param {enumRemoteMmiType} type Type of remote MMI to get focus status of.
     * @returns {Promise<boolean, JabraError>} returns true if in focus, false if not. 
    */    
    isRemoteMmiInFocusaAsync(type: enumRemoteMmiType) : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isRemoteMmiInFocusaAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.IsRemoteMmiInFocus)(this.deviceID, type).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isRemoteMmiInFocusaAsync.name, "returned");
            return result;
        });
    }  
    
    /**
     * Sets an output action on the remote MMI. Note that
     * getRemoteMmiFocusAsync must be called once for the enumRemoteMmiType in
     * question prior to setting the output action, else JabraError is
     * returned.
     * @param {enumRemoteMmiType} type type Type of remote MMI to set action of.
     * @param {RemoteMmiActionOutput} outputAction Output LED action to set.
     * @returns {Promise<boolean, JabraError>} returns true if in focus, false if not. 
    */    
    setRemoteMmiActionAsync(type: enumRemoteMmiType, actionOutput: RemoteMmiActionOutput ) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setRemoteMmiActionAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetRemoteMmiAction)(this.deviceID, type, actionOutput).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setRemoteMmiActionAsync.name, "returned");
            return result;
        });
    }
    
    /**
     * Checks whether remote management is enabled
     * @returns {Promise<boolean, JabraError>} - Resolves to true if remote
     *   management is enabled, false if it is not. Rejects to JabraError in
     *   case of errors.
     */
    isRemoteManagementEnabledAsync() : Promise<boolean> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isRemoteManagementEnabledAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.IsRemoteManagementEnabled)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isRemoteManagementEnabledAsync.name, "returned");
        return result;
      });
    }

    /**
     * Enables/disables remote management
     * @param {boolean} enable - True to enable remote management, false to
     *   disable it
     * @param {number} timeout - Time in ms to wait for a valid network connection
     * Set to 0 to disable network connection check.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *   rejects with `JabraError` if an error occurs.
     */
    enableRemoteManagementAsync(enable: boolean, timeout: number) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableRemoteManagementAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.EnableRemoteManagement)(this.deviceID, enable, timeout).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.enableRemoteManagementAsync.name, "returned");
      });
    }
    
    /**
     * Sets the Jabra Xpress URL
     * @param {string} url - The new Jabra Xpress URL
     * @param {number} timeout - Maximum allowed time in ms for URL validation.
     * Set to 0 to disable URL validation.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *   rejects with `JabraError` if an error occurs.
     */
    setXpressUrlAsync(url: string, timeout: number) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setXpressUrlAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.SetXpressUrl)(this.deviceID, url, timeout).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setXpressUrlAsync.name, "returned");
      });
    }

    /**
     * Returns the Jabra Xpress URL
     * @returns {Promise<string, JabraError>} - Resolves to the Xpress url on success,
     *   rejects with `JabraError` if an error occurs.
     */
    getXpressUrlAsync() : Promise<string> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getXpressUrlAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.GetXpressUrl)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getXpressUrlAsync.name, "returned");
        return result;
      });
    }
    
    /**
     * Configures networked device for stand-alone remote management
     * @param {string} url - URL for the Xpress backend server
     * If empty/undefined, management will be disabled
     * @param {ProxySettings} proxySettings - Proxy configuration
     * Leave 'url' empty/undefined to disable proxy
     * @param {number} timeout - Time limit for entire operation in milliseconds
     * Suggested value is 10000 ms or higher.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *   rejects with `JabraError` if an error occurs.
     */
    configureXpressManagementAsync(url: string, proxySettings: ProxySettings, timeout: number) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.configureXpressManagementAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.ConfigureXpressManagement)(this.deviceID, url, proxySettings, timeout).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.configureXpressManagementAsync.name, "returned");
      });
    }

    /**
     * Sets the password for provisioning
     * @param {string} password - The password
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *   rejects with `JabraError` if an error occurs.
     */
    setPasswordProvisioningAsync(password: string) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPasswordProvisioningAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.SetPasswordProvisioning)(this.deviceID, password).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPasswordProvisioningAsync.name, "returned");
      });
    }
  
    /**
     * Get the password for provisioning
     * @returns {Promise<string, JabraError>} - Resolves to password string on success,
     *   rejects with `JabraError` if an error occurs.
     */
    getPasswordProvisioningAsync() : Promise<string> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPasswordProvisioningAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.GetPasswordProvisioning)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPasswordProvisioningAsync.name, "returned with", result);
        return result;
      });
    }

    /**
     * Gets the pre-generated diagnostic log file on supported devices
     * @param {string} filename - Destination filename on local file system
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *   rejects with `JabraError` if an error occurs.
     * @see triggerDiagnosticLogGenerationAsync
     */
    getDiagnosticLogFileAsync(filename: string) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getDiagnosticLogFileAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetDiagnosticLogFile)(this.deviceID, filename).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getDiagnosticLogFileAsync.name, "returned");
      });
    }

    /**
     * Triggers the generation of the diagnostic log file
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *   rejects with `JabraError` if an error occurs.
     */
    triggerDiagnosticLogGenerationAsync() : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.triggerDiagnosticLogGenerationAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.TriggerDiagnosticLogGeneration)(this.deviceID).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.triggerDiagnosticLogGenerationAsync.name, "returned");
      });
    }

    /**
     * Returns whether the device is certified for Skype For Business
     * @returns {Promise<boolean, JabraError>} - Resolves to a boolean telling if the device is certified
     */
     isCertifiedForSkypeForBusinessAsync() : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isCertifiedForSkypeForBusinessAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.IsCertifiedForSkypeForBusiness)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isCertifiedForSkypeForBusinessAsync.name, "returned");
          return result;
        });
      }

      /**
     * Returns the position of the provided whiteboard corners as coordinates.
     * @param {number} whiteboardId - The whiteboard id number.
     * @returns {Promise<number, JabraError>} - Resolves to the whiteboard
     *    corners position on success, rejects with `JabraError` if an error
     *    occurs.
     */
    getWhiteboardPositionAsync(whiteboardId: number) : Promise<WhiteboardPosition> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteboardPositionAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.GetWhiteboardPosition)(this.deviceID, whiteboardId).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteboardPositionAsync.name, "returned");
        return result;
      });
    }

    /**
     * Sets the position of the provided whiteboard corners.
     * @param {number} whiteboardId - The whiteboard id number.
     * @param {WhiteboardPosition} whiteboardPosition - The whiteboard corners.
     * @returns {Promise<number, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setWhiteboardPositionAsync(whiteboardId: number, whiteboardPosition: WhiteboardPosition) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWhiteboardPositionAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.SetWhiteboardPosition)(this.deviceID, whiteboardId, whiteboardPosition).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWhiteboardPositionAsync.name, "returned");
      });
    }

    /**
     * Returns the current zoom value from the device's camera.
     * @returns {Promise<number, JabraError>} - Resolves to the current zoom
     *    value on success, else rejects with `JabraError`
     */
    getZoomAsync() : Promise<number> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getZoomAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.GetZoom)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getZoomAsync.name, "returned");
        return result;
      });
    }

    /**
     * Controls the device's camera zoom functionality.
     * @param {number} - The new value for the device camera's zoom.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setZoomAsync(zoom: number) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setZoomAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.SetZoom)(this.deviceID, zoom).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setZoomAsync.name, "returned");
      });
    }

    /**
     * Returns the zoom limit values from the device's camera.
     * @returns {Promise<VideoLimits, JabraError>} - Resolves to the current
     *    zoom limit values on success, else rejects with `JabraError`
     */
    getZoomLimitsAsync() : Promise<VideoLimits> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getZoomLimitsAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.GetZoomLimits)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getZoomLimitsAsync.name, "returned");
        return result;
      });
    }

    /**
     * Starts or stops the relative zoom action of the camera lens.
     * Note that this method should be used in pairs:
     * Initially to start the action (button down), next to stop the action (button up).
     * @param {ZoomRelative} - The action for the device camera's zoom.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setZoomRelativeActionAsync(action: ZoomRelative) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setZoomRelativeActionAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetZoomRelativeAction)(this.deviceID, action).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setZoomRelativeActionAsync.name, "returned");
        });
    }

    /** Returns the current pan-tilt values from the device's camera.
     * @returns {Promise<PanTilt, JabraError>} - Resolves to the current
     *    pan-tilt parameters on success, else rejects with `JabraError`
     */
    getPanTiltAsync() : Promise<PanTilt> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPanTiltAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.GetPanTilt)(this.deviceID).then((result) => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPanTiltAsync.name, "returned");
        return result;
      });
    }
    
    /**
     * Controls the device's camera pan-tilt functionality.
     * @param {PanTilt} - The new values for the device camera's pan-tilt.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setPanTiltAsync(panTilt: PanTilt) : Promise<void> {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPanTiltAsync.name, "called with", this.deviceID);
      return util.promisify(sdkIntegration.SetPanTilt)(this.deviceID, panTilt).then(() => {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPanTiltAsync.name, "returned");
      });
    }

    /** Returns the current pan-tilt limits values from the device's camera.
     * @returns {Promise<PanTiltLimits, JabraError>} - Resolves to the
     *    pan-tilt limits on success, else rejects with `JabraError`
     */
    getPanTiltLimitsAsync() : Promise<PanTiltLimits> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPanTiltLimitsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetPanTiltLimits)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getPanTiltLimitsAsync.name, "returned");
          return result;
        });
    }

    /**
     * Starts or stops the relative pan and/or tilt action of the camera lens.
     * Note that this method should be used in pairs:
     * Initially to start the action (button down), next to stop the action (button up).
     * @param {PanTiltRelative} - The action for the device camera's zoom.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setPanTiltRelativeActionAsync(action: PanTiltRelative) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPanTiltRelativeActionAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetPanTiltRelativeAction)(this.deviceID, action).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setPanTiltRelativeActionAsync.name, "returned");
        });
    }

    /**
     * Enables or disables the active HDR setting
     * @param {boolean} - Enable setting
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setVideoHDRAsync(enable: boolean) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoHDRAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetVideoHDR)(this.deviceID, enable).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoHDRAsync.name, "returned");
        });
    }

    /**
     * Gets the active HDR enable setting on a video device
     * @returns {Promise<boolean, JabraError>} - Resolves to `boolean` on success,
     *    rejects with `JabraError` on error.
     */
    getVideoHDRAsync() : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getVideoHDRAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetVideoHDR)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getVideoHDRAsync.name, "returned");
          return result;
        });
    }

    /**
     * Enables or disables the default HDR setting
     * @param {boolean} - Enable setting
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setVideoHDRDefaultAsync(enable: boolean) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoHDRDefaultAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetVideoHDRDefault)(this.deviceID, enable).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoHDRDefaultAsync.name, "returned");
        });
    }

    /**
     * Gets the default HDR enable setting on a video device
     * @returns {Promise<boolean, JabraError>} - Resolves to `boolean` on success,
     *    rejects with `JabraError` on error.
     */
    getVideoHDRDefaultAsync() : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getVideoHDRDefaultAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetVideoHDRDefault)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getVideoHDRDefaultAsync.name, "returned");
          return result;
        });
    }

    /**
     * Sets the contrast level on a camera device
     * @param {number} - The contrast level in the range [0;191]
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setContrastLevelAsync(Level: number) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setContrastLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetContrastLevel)(this.deviceID, Level).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setContrastLevelAsync.name, "returned");
        });
    }

    /**
     * Gets the current contrast level on a camera device
     * @returns {Promise<number, JabraError>} - Resolves to `number` level on success,
     *    rejects with `JabraError` on error.
     */
    getContrastLevelAsync() : Promise<number> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getContrastLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetContrastLevel)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getContrastLevelAsync.name, "returned");
          return result;
        });
    }

    /**
     * Sets the brightness level on a camera device
     * @param {number} - The brightness level in the range [0;255]
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setBrightnessLevelAsync(Level: number) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setBrightnessLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetBrightnessLevel)(this.deviceID, Level).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setBrightnessLevelAsync.name, "returned");
        });
    }

    /**
     * Gets the current brightness level on a camera device
     * @returns {Promise<number, JabraError>} - Resolves to `number` level on success,
     *    rejects with `JabraError` on error.
     */
    getBrightnessLevelAsync() : Promise<number> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBrightnessLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetBrightnessLevel)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBrightnessLevelAsync.name, "returned");
          return result;
        });
    }

    /**
     * Sets the sharpness level on a camera device
     * @param {number} - The sharpness level in the range [0;255]
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setSharpnessLevelAsync(Level: number) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSharpnessLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetSharpnessLevel)(this.deviceID, Level).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSharpnessLevelAsync.name, "returned");
        });
    }

    /**
     * Gets the current sharpness level on a camera device
     * @returns {Promise<number, JabraError>} - Resolves to `number` level on success,
     *    rejects with `JabraError` on error.
     */
    getSharpnessLevelAsync() : Promise<number> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSharpnessLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetSharpnessLevel)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSharpnessLevelAsync.name, "returned");
          return result;
        });
    }

    /**
     * Sets the saturation level on a camera device
     * @param {number} - The saturation level in the range [128;176]
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setSaturationLevelAsync(Level: number) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSaturationLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetSaturationLevel)(this.deviceID, Level).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSaturationLevelAsync.name, "returned");
        });
    }

    /**
     * Gets the current saturation level on a camera device
     * @returns {Promise<number, JabraError>} - Resolves to `number` level on success,
     *    rejects with `JabraError` on error.
     */
    getSaturationLevelAsync() : Promise<number> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSaturationLevelAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetSaturationLevel)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSaturationLevelAsync.name, "returned");
          return result;
        });
    }
    
    /**
     * Sets the white balance on a camera device
     * @param {WhiteBalance} - The white balance in the range [0;6500] [K] and whether auto adjust is enabled
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setWhiteBalanceAsync(setting: WhiteBalance) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWhiteBalanceAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetWhiteBalance)(this.deviceID, setting).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWhiteBalanceAsync.name, "returned");
        });
    }

    /**
     * Gets the current white balance on a camera device
     * @returns {Promise<WhiteBalance, JabraError>} - Resolves to `WhiteBalance` result on success,
     *    rejects with `JabraError` on error.
     */
    getWhiteBalanceAsync() : Promise<WhiteBalance> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteBalanceAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetWhiteBalance)(this.deviceID).then((result) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteBalanceAsync.name, "returned");
          return result;
        });
    }

    /**
     * Gets the contrast limits on a camera device
     * @returns {Promise<VideoLimits, JabraError>} - Resolves to `VideoLimits` limits on success,
     *    rejects with `JabraError` on error.
     */
    getContrastLimitsAsync() : Promise<VideoLimits> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getContrastLimitsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetContrastLimits)(this.deviceID).then((limits) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getContrastLimitsAsync.name, "returned");
          return limits;
        });
    }

    /**
     * Gets the sharpness limits on a camera device
     * @returns {Promise<VideoLimits, JabraError>} - Resolves to `VideoLimits` limits on success,
     *    rejects with `JabraError` on error.
     */
    getSharpnessLimitsAsync() : Promise<VideoLimits> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSharpnessLimitsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetSharpnessLimits)(this.deviceID).then((limits) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSharpnessLimitsAsync.name, "returned");
          return limits;
        });
    }

    /**
     * Gets the brightness limits on a camera device
     * @returns {Promise<VideoLimits, JabraError>} - Resolves to `VideoLimits` limits on success,
     *    rejects with `JabraError` on error.
     */
    getBrightnessLimitsAsync() : Promise<VideoLimits> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBrightnessLimitsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetBrightnessLimits)(this.deviceID).then((limits) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getBrightnessLimitsAsync.name, "returned");
          return limits;
        });
    }

    /**
     * Gets the saturation limits on a camera device
     * @returns {Promise<VideoLimits, JabraError>} - Resolves to `VideoLimits` limits on success,
     *    rejects with `JabraError` on error.
     */
    getSaturationLimitsAsync() : Promise<VideoLimits> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSaturationLimitsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetSaturationLimits)(this.deviceID).then((limits) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSaturationLimitsAsync.name, "returned");
          return limits;
        });
    }

    /**
     * Gets the white balance limits on a camera device
     * @returns {Promise<VideoLimits, JabraError>} - Resolves to `VideoLimits` limits on success,
     *    rejects with `JabraError` on error.
     */
    getWhiteBalanceLimitsAsync() : Promise<VideoLimits> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteBalanceLimitsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetWhiteBalanceLimits)(this.deviceID).then((limits) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteBalanceLimitsAsync.name, "returned");
          return limits;
        });
    }

    /**
     * Restores the video settings to room defaults like when a new call is started
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    restoreVideoRoomDefaultsAsync() : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.restoreVideoRoomDefaultsAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.RestoreVideoRoomDefaults)(this.deviceID).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.restoreVideoRoomDefaultsAsync.name, "returned");
        });
    }

    /**
     * Sets the room capacity setting on a video device
     * @param {number} - Room capacity in no of people
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setRoomCapacityAsync(capacity: number) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setRoomCapacityAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetRoomCapacity)(this.deviceID, capacity).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setRoomCapacityAsync.name, "returned");
        });
    }

    /**
     * Gets the room capacity setting on a video device
     * @returns {Promise<number, JabraError>} - Resolves to `number` room capacity on success,
     *    rejects with `JabraError` on error.
     */
    getRoomCapacityAsync() : Promise<number> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRoomCapacityAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetRoomCapacity)(this.deviceID).then((capacity) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRoomCapacityAsync.name, "returned");
          return capacity;
        });
    }

    /**
     * Sets whether capacity notifications are enabled on a video device
     * @param {boolean} - Notification enable state
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setRoomCapacityNotificationEnabledAsync(enable: boolean) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setRoomCapacityNotificationEnabledAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetRoomCapacityNotificationEnabled)(this.deviceID, enable).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setRoomCapacityNotificationEnabledAsync.name, "returned");
        });
    }

    /**
     * Gets whether capacity notifications are enabled on a video device
     * @returns {Promise<boolean, JabraError>} - Resolves to `boolean` enable state on success,
     *    rejects with `JabraError` on error.
     */
    getRoomCapacityNotificationEnabledAsync() : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRoomCapacityNotificationEnabledAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetRoomCapacityNotificationEnabled)(this.deviceID).then((enable) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getRoomCapacityNotificationEnabledAsync.name, "returned");
          return enable;
        });
    }

    /**
     * Sets the style of notifications on a video device
     * @param {enumNotificationStyle} - Notification style
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setNotificationStyleAsync(style: enumNotificationStyle) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setNotificationStyleAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetNotificationStyle)(this.deviceID, style).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setNotificationStyleAsync.name, "returned");
        });
    }

    /**
     * Gets the style of notifications on a video device
     * @returns {Promise<enumNotificationStyle, JabraError>} - Resolves to `enumNotificationStyle` on success,
     *    rejects with `JabraError` on error.
     */
    getNotificationStyleAsync() : Promise<enumNotificationStyle> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getNotificationStyleAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetNotificationStyle)(this.deviceID).then((style) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getNotificationStyleAsync.name, "returned");
          return style;
        });
    }

    /**
     * Sets when notifications are active on a video device
     * @param {enumNotificationUsage} - Notification style
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setNotificationUsageAsync(style: enumNotificationUsage) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setNotificationUsageAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetNotificationUsage)(this.deviceID, style).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setNotificationUsageAsync.name, "returned");
        });
    }

    /**
     * Gets when notifications are active on a video device
     * @returns {Promise<enumNotificationUsage, JabraError>} - Resolves to `enumNotificationUsage` on success,
     *    rejects with `JabraError` on error.
     */
    getNotificationUsageAsync() : Promise<enumNotificationUsage> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getNotificationUsageAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetNotificationUsage)(this.deviceID).then((usage) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getNotificationUsageAsync.name, "returned");
          return usage;
        });
    }

    /**
     * Stores the current color controls into a preset slot.
     * @param {enumColorControlPreset} - The preset slot to be used.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    storeColorControlPresetAsync(PresetSlot: enumColorControlPreset) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.storeColorControlPresetAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.StoreColorControlPreset)(this.deviceID, PresetSlot).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.storeColorControlPresetAsync.name, "returned");
        });
    }

    /**
     * Applies color controls from a given preset slot.
     * @param {enumColorControlPreset} - The preset slot to be used.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    applyColorControlPresetAsync(PresetSlot: enumColorControlPreset) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.applyColorControlPresetAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.ApplyColorControlPreset)(this.deviceID, PresetSlot).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.applyColorControlPresetAsync.name, "returned");
        });
    }

    /**
     * Stores the current Pan/Tilt/Zoom settings into a preset slot.
     * @param {enumPTZPreset} - The preset slot to be used.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    storePTZPresetAsync(PresetSlot: enumPTZPreset) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.storePTZPresetAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.StorePTZPreset)(this.deviceID, PresetSlot).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.storePTZPresetAsync.name, "returned");
        });
    }

    /**
     * Applies Pan/Tilt/Zoom settings from a given preset slot.
     * @param {enumPTZPreset} - The preset slot to be used.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    applyPTZPresetAsync(PresetSlot: enumPTZPreset) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.applyPTZPresetAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.ApplyPTZPreset)(this.deviceID, PresetSlot).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.applyPTZPresetAsync.name, "returned");
        });
    }

    /**
     * Controls what is shown on the secondary video stream.
     * @param {enumSecondaryStreamContent} - Choice of stream
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setSecondVideoStreamAsync(stream: enumSecondaryStreamContent) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSecondVideoStreamAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetSecondVideoStream)(this.deviceID, stream).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSecondVideoStreamAsync.name, "returned");
        });
    }

    /**
     * Returns which stream is shown in the secondary video stream.
     * @returns {Promise<enumSecondaryStreamContent, JabraError>} - Resolves to `enumSecondaryStreamContent` on success,
     *    rejects with `JabraError` on error.
     */
    getSecondVideoStreamAsync() : Promise<enumSecondaryStreamContent> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSecondVideoStreamAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetSecondVideoStream)(this.deviceID).then((stream) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSecondVideoStreamAsync.name, "returned");
          return stream;
        });
    }

    /**
     * Set whether the main stream shows the whiteboard.
     * @param {boolean} - Whiteboard on main stream enable status
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setWhiteboardOnMainStreamAsync(whiteboardEnable: boolean) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWhiteboardOnMainStreamAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetWhiteboardOnMainStream)(this.deviceID, whiteboardEnable).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setWhiteboardOnMainStreamAsync.name, "returned");
        });
    }

    /**
     * Get whether the main stream shows the whiteboard.
     * @returns {Promise<boolean, JabraError>} - Resolves to `bool` on success,
     *    rejects with `JabraError` on error.
     */
    getWhiteboardOnMainStreamAsync() : Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteboardOnMainStreamAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetWhiteboardOnMainStream)(this.deviceID).then((whiteboardEnable) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWhiteboardOnMainStreamAsync.name, "returned");
          return whiteboardEnable;
        });
    }

    /**
     * Sets the active video transition style with Intelligent Zoom.
     * @returns {Promise<void, JabraError>} - Resolves to `void` on success,
     *    rejects with `JabraError` on error.
     */
    setVideoTransitionStyleAsync(style: enumVideoTransitionStyle) : Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoTransitionStyleAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.SetVideoTransitionStyle)(this.deviceID, style).then(() => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setVideoTransitionStyleAsync.name, "returned");
        });
    }

    /**
     * Returns the active video transition style with Intelligent Zoom.
     * @returns {Promise<enumVideoTransitionStyle, JabraError>} - Resolves to `enumVideoTransitionStyle` on success,
     *    rejects with `JabraError` on error.
     */
    getVideoTransitionStyleAsync() : Promise<enumVideoTransitionStyle> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getVideoTransitionStyleAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetVideoTransitionStyle)(this.deviceID).then((style) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getVideoTransitionStyleAsync.name, "returned");
          return style;
        });
    }

    /**
     * Returns the status of the Ethernet connection.
     * @returns {Promise<IPv4Status, JabraError>} - Resolves to `IPv4Status` on success,
     *    rejects with `JabraError` on error.
     */
    getEthernetIPv4StatusAsync() : Promise<IPv4Status> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getEthernetIPv4StatusAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetEthernetIPv4Status)(this.deviceID).then((ethernetStatus) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getEthernetIPv4StatusAsync.name, "returned");
          return ethernetStatus;
        });
    }

    /**
     * Returns the status of the WLAN connection.
     * @returns {Promise<IPv4Status, JabraError>} - Resolves to `IPv4Status` on success,
     *    rejects with `JabraError` on error.
     */
    getWLANIPv4StatusAsync() : Promise<IPv4Status> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWLANIPv4StatusAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetWLANIPv4Status)(this.deviceID).then((ethernetStatus) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getWLANIPv4StatusAsync.name, "returned");
          return ethernetStatus;
        });
    }

    /**
     * Returns the state and speed of the device's USB connection.
     * @returns {Promise<enumUSBState, JabraError>} - Resolves to `enumUSBState` on success,
     *    rejects with `JabraError` on error.
     */
     getUSBStateAsync() : Promise<enumUSBState> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getUSBStateAsync.name, "called with", this.deviceID);
        return util.promisify(sdkIntegration.GetUSBState)(this.deviceID).then((usbState) => {
          _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getUSBStateAsync.name, "returned");
          return usbState;
        });
    }

    /**
   * Get meta information about methods, properties etc. that can be used 
   * for reflective usage of this class.
   */
   getMeta() : ClassEntry {
      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getMeta.name, "called with", this.deviceID);

      const deviceClassName = this.constructor.name;
      const apiMeta = _getJabraApiMetaSync();
      let deviceTypeMeta = apiMeta.find((c) => c.name === deviceClassName);
      if (!deviceTypeMeta)
         throw new Error("Could not find meta data for " + deviceClassName);

      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getMeta.name, "returned with", apiMeta);   

      return deviceTypeMeta;
   }

   /**
   * Add event handler for btnPress device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'btnPress', listener: DeviceTypeCallbacks.btnPress): this;

   /**
   * Add event handler for busyLightChange device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */   
   on(event: 'busyLightChange', listener: DeviceTypeCallbacks.busyLightChange): this;
      
   /**
   * Add event handler for downloadFirmwareProgress device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'downloadFirmwareProgress', listener: DeviceTypeCallbacks.downloadFirmwareProgress): this;
      
   /**
   * Add event handler for onBTParingListChange device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onBTParingListChange', listener: DeviceTypeCallbacks.onBTParingListChange): this;
      
   /**
   * Add event handler for onGNPBtnEvent device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onGNPBtnEvent', listener: DeviceTypeCallbacks.onGNPBtnEvent): this;
      
   /**
   * Add event handler for onDevLogEvent device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onDevLogEvent', listener: DeviceTypeCallbacks.onDevLogEvent): this;
         
   /**
   * Add event handler for onDiagLogEvent device events.
   *
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onDiagLogEvent', listener: DeviceTypeCallbacks.onDiagLogEvent): this;
  
   /**
   * Add event handler for onBatteryStatusUpdate device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onBatteryStatusUpdate', listener: DeviceTypeCallbacks.onBatteryStatusUpdate): this;
  
   /**
    * Add event handler for remoteMmi events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
   on(event: 'onRemoteMmiEvent', listener: DeviceTypeCallbacks.onRemoteMmiEvent): this;
   
   /**
    *  Add event handler for when the Xpress connection status is changed
    */
   on(event: 'onxpressConnectionStatusEvent', listener: DeviceTypeCallbacks.onxpressConnectionStatusEvent): this;
   
   /**
   * Add event handler for onUploadProgress device events.
   * 
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onUploadProgress', listener: DeviceTypeCallbacks.onUploadProgress): this;

   /**
   * Add event handler for onDectInfo device events.
   *
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onDectInfoEvent', listener: DeviceTypeCallbacks.onDectInfoEvent): this;
   
   /**
   * Add event handler for onCameraStatusEvent device events.
   *
   * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
   */
   on(event: 'onCameraStatusEvent', listener: DeviceTypeCallbacks.onCameraStatusEvent): this;

    /**
     * Add event handler for one of the different device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
   on(event: DeviceTypeEvents,
      listener: DeviceTypeCallbacks.btnPress | DeviceTypeCallbacks.busyLightChange | DeviceTypeCallbacks.downloadFirmwareProgress | DeviceTypeCallbacks.onBTParingListChange |
                DeviceTypeCallbacks.onGNPBtnEvent | DeviceTypeCallbacks.onDevLogEvent | DeviceTypeCallbacks.onDiagLogEvent | DeviceTypeCallbacks.onBatteryStatusUpdate | DeviceTypeCallbacks.onRemoteMmiEvent |
                DeviceTypeCallbacks.onxpressConnectionStatusEvent | DeviceTypeCallbacks.onUploadProgress | DeviceTypeCallbacks.onDectInfoEvent | DeviceTypeCallbacks.onCameraStatusEvent): this {

      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.on.name, "called with", this.deviceID, event, "<listener>"); 

      this._eventEmitter.on(event, listener);

      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.on.name, "returned"); 

      return this;
   }

    /**
    * Remove event handler for previosly setup btnPress device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'btnPress', listener: DeviceTypeCallbacks.btnPress): this;
   
    /**
    * Remove event handler for previosly setup busyLightChange device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'busyLightChange', listener: DeviceTypeCallbacks.busyLightChange): this;
   
    /**
    * Remove event handler for previosly setup downloadFirmwareProgress device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'downloadFirmwareProgress', listener: DeviceTypeCallbacks.downloadFirmwareProgress): this;
   
    /**
    * Remove event handler for previosly setup onBTParingListChange device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onBTParingListChange', listener: DeviceTypeCallbacks.onBTParingListChange): this;
   
    /**
    * Remove event handler for previosly setup onGNPBtnEvent device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onGNPBtnEvent', listener: DeviceTypeCallbacks.onGNPBtnEvent): this;
   
    /**
    * Remove event handler for previosly setup onDevLogEvent device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onDevLogEvent', listener: DeviceTypeCallbacks.onDevLogEvent): this;
   
    /**
    * Remove event handler for previosly setup onDiagLogEvent device events.
    *
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onDiagLogEvent', listener: DeviceTypeCallbacks.onDiagLogEvent): this;
   
    /**
    * Remove event handler for previosly setup onBatteryStatusUpdate device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onBatteryStatusUpdate', listener: DeviceTypeCallbacks.onBatteryStatusUpdate): this;
   
    /**
    * Remove event handler for previosly setup onRemoteMmiEvent device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onRemoteMmiEvent', listener: DeviceTypeCallbacks.onRemoteMmiEvent): this;

    /**
    * Remove event handler for onxpressConnectionStatusEvent.
    */        
    off(event: 'onxpressConnectionStatusEvent', listener: DeviceTypeCallbacks.onxpressConnectionStatusEvent): this;
    
    /**
    * Remove event handler for previosly setup onUploadProgress device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onUploadProgress', listener: DeviceTypeCallbacks.onUploadProgress): this;

    /**
    * Remove event handler for previosly setup onDectInfo device events.
    *
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onDectInfoEvent', listener: DeviceTypeCallbacks.onDectInfoEvent): this;
  
    /**
    * Remove event handler for previosly setup onCameraStatusEvent device events.
    *
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: 'onCameraStatusEvent', listener: DeviceTypeCallbacks.onCameraStatusEvent): this;

    /**
    * Remove previosly setup event handler for device events.
    * 
    * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
    */
    off(event: DeviceTypeEvents,
        listener: DeviceTypeCallbacks.btnPress | DeviceTypeCallbacks.busyLightChange | DeviceTypeCallbacks.downloadFirmwareProgress | DeviceTypeCallbacks.onBTParingListChange |
        DeviceTypeCallbacks.onGNPBtnEvent | DeviceTypeCallbacks.onDevLogEvent | DeviceTypeCallbacks.onDiagLogEvent | DeviceTypeCallbacks.onBatteryStatusUpdate | DeviceTypeCallbacks.onRemoteMmiEvent |
        DeviceTypeCallbacks.onxpressConnectionStatusEvent | DeviceTypeCallbacks.onUploadProgress | DeviceTypeCallbacks.onDectInfoEvent | DeviceTypeCallbacks.onCameraStatusEvent): this {

      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.off.name, "called with", this.deviceID, event, "<listener>"); 

      this._eventEmitter.off(event, listener);

      _JabraNativeAddonLog(AddonLogSeverity.verbose, this.off.name, "returned"); 

      return this;
   }
}
