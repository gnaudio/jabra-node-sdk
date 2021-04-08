import { SdkIntegration } from "./sdkintegration";
import { AddonLogSeverity, DevLogData } from "./core-types";
import { isNodeJs, nameof } from './util';
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

import { DeviceInfo, RCCStatus, ConfigInfo, ConfigParamsCloud, GenericConfigParams, DeviceCatalogueParams,
         FirmwareInfoType, SettingType, DeviceSettings } from './core-types';

import { enumAPIReturnCode, enumDeviceErrorStatus, enumDeviceBtnType, enumDeviceConnectionType,
         enumSettingDataType, enumSettingCtrlType, enumSettingLoadMode, enumFirmwareEventStatus,
         enumFirmwareEventType, enumBTPairedListType, enumUploadEventStatus,
         enumDeviceFeature, enumHidState, enumWizardMode } from './jabra-enums';

import { MetaApi, ClassEntry, _getJabraApiMetaSync } from './meta';

import * as util from 'util';

import { DeviceType } from './device';

// Singletons containing our top-level object and parameters.

/** @internal */
let jabraApp: Promise<JabraType> | null = null;

/** @internal */
let jabraAppOptions: (ConfigParamsCloud & GenericConfigParams) | null = null;

/** @internal */
let jabraAppID: string | null = null;

/**
 * This function should be called to create/get the main JabraType (application) instance.
 * @param appID The user should first register the app on [Jabra developer site](https://developer.jabra.com/) to get application id.
 * @param configCloudParams Optional configuration parameters for the sdk.
 * @param nonJabraDeviceDectection If true non Jabra and Jabra devices will be detected, false by default.
 */
export function createJabraApplication(appID: string, configCloudParams: ConfigParamsCloud = {}, nonJabraDeviceDectection: boolean = false): Promise<JabraType> {
    if (!isNodeJs()) {
        return Promise.reject(new Error("This createJabraApplication() function needs to run under NodeJs and not in a browser"));
    }

    let options = configCloudParams ? JSON.parse(JSON.stringify(configCloudParams)) : {};
    options!.nonJabraDeviceDectection = nonJabraDeviceDectection;

    if (!jabraApp) {
        _JabraNativeAddonLog(AddonLogSeverity.info, "createJabraApplication", "Init - Creating new jabraApp");

        jabraAppID = appID;
        jabraAppOptions = options;
        jabraApp = new Promise<JabraType>((resolve, reject) => {
            return new JabraType(appID, options, resolve, reject);
        });
    } else { // Reuse existing promise:
        // Nb. This simple comparison unfortunately requires options argument order to be same.
        if ((JSON.stringify(options) !== JSON.stringify(jabraAppOptions)) || (appID !== jabraAppID)) {
            return Promise.reject(new Error("Repeated calls to createJabraApplication() function must have the same arguments"));
        }

        _JabraNativeAddonLog(AddonLogSeverity.info, "createJabraApplication", "Init - Reuseing existing jabraApp");
    }

    return jabraApp;
}

export namespace JabraTypeCallbacks {
    export type attach = (device: DeviceType) => void;
    export type detach = (device: DeviceType) => void;
    export type firstScanDone = () => void;
}

export type JabraTypeEvents = 'attach' | 'detach' | 'firstScanDone';

export const JabraEventsList: JabraTypeEvents[] = ['attach', 'detach', 'firstScanDone'];

/** 
 * Main API class return by createJabraApplication.   
 */
export class JabraType implements MetaApi {
    /** @internal */
    private readonly eventEmitter: _EventEmitter;

    /** @internal */
    private readonly deviceTypes: Map<number, DeviceType>;

    /** @internal */
    private readonly firstScanForDevicesDonePromise: Promise<void>;

    /**
     * The application Id used to instantiate the Api.
     * 
     * Nb. Not applicable for client side proxies.
     */
    public readonly appID?: string;

    /** 
     * @internal 
     * @hidden
     **/
    constructor(appID: string, configParams: ConfigParamsCloud & GenericConfigParams, resolve: (value: JabraType) => void, reject: (reason: Error) => void) {
        if (!isNodeJs()) {
            throw new Error("This JabraType constructor() function needs to run under NodeJs and not in a browser");
        }

        _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor", "called");

        this.appID = appID;

        this.eventEmitter = new events.EventEmitter();
    
        this.deviceTypes = new Map<number, DeviceType>();

        this.firstScanForDevicesDonePromise = new Promise<void>(( firstScanForDevicesDoneResolve, firstScanForDevicesDoneReject ) => {
            sdkIntegration.Initialize( appID, (err, success) => {
                try {
                    if (err) {
                        let errObj = new Error("Initialization error " + err);
                        _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::success callback", err);
                        reject(errObj);
                        firstScanForDevicesDoneReject(errObj);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::success", "native sdk initialized successfully");
                        resolve(this);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::success callback", err);
                }
            }, (event_time_ms) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::firstScanDone", (() =>`firstScanDone event received from native sdk with event_time_ms=${event_time_ms}`));
                    this.eventEmitter.emit('firstScanDone', undefined);
                    firstScanForDevicesDoneResolve();
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::firstScanDone callback", err);
                }
            }, (deviceData, event_time_ms) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::attach", (() =>`attach event received from native sdk with deviceData=${JSON.stringify(deviceData, null, 3)}, event_time_ms=${event_time_ms}`));
                    let deviceType = new DeviceType(deviceData, event_time_ms);
                    this.deviceTypes.set(deviceData.deviceID, deviceType);
                    this.eventEmitter.emit('attach', deviceType);
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::attach callback", err);
                }
            }, (deviceId, event_time_ms) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::detach", (() =>`detach event received from native sdk with deviceId=${deviceId}, event_time_ms=${event_time_ms}`));
                    let deviceType = this.deviceTypes.get(deviceId);
                    if (deviceType) {
                        // Assign to detached_time_ms even though it is formally a readonly because we don't want clients to change it.
                        (deviceType.detached_time_ms as DeviceType['detached_time_ms']) = event_time_ms;
                        this.deviceTypes.delete(deviceId);
                        this.eventEmitter.emit('detach', deviceType);                        
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::detach callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::detach callback", err);
                }
            }, (deviceId, translatedInData, buttonInData) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::buttonInDataTranslated", (() => `buttonInDataTranslated event received from native sdk with translatedInData=${translatedInData}, buttonInData=${buttonInData}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('btnPress', translatedInData, buttonInData);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "buttonInDataTranslated callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::buttonInDataTranslated callback", err)
                }
            }, (deviceId, jsonData) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onDevLogEvent", (() => `onDevLogEvent event received from native sdk with jsonData=${jsonData}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        const data = JSON.parse(jsonData);                        
                        device._eventEmitter.emit('onDevLogEvent', data as DevLogData);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onDevLogEvent callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onDevLogEvent callback", err)
                }
            }, (deviceId, levelInPercent, isCharging, isBatteryLow) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onBatteryStatusUpdate", (() => `onBatteryStatusUpdate event received from native sdk with levelInPercent=${levelInPercent}, isCharging=${isCharging}, isBatteryLow=${isBatteryLow}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('onBatteryStatusUpdate', levelInPercent, isCharging, isBatteryLow);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onBatteryStatusUpdate callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onBatteryStatusUpdate callback", err)
                }
            }, (deviceId, type, input) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onRemoteMmiEvent", (() => `onRemoteMmiEvent event received from native sdk with type=${type}, input=${input}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('onRemoteMmiEvent', type, input);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onRemoteMmiEvent callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onRemoteMmiEvent callback", err)
                }
            }, (deviceId, type, status, dwnFirmPercentage) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::downloadFirmwareProgress", (() => `downloadFirmwareProgress event received from native sdk with type=${type}, status=${status}, dwnFirmPercentage=${dwnFirmPercentage}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('downloadFirmwareProgress', type, status, dwnFirmPercentage);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "downloadFirmwareProgress callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::downloadFirmwareProgress callback", err)
                }                
            }, (deviceId, status, percentage) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onUploadProgress", (() => `onUploadProgress event received from native sdk with status ${status}, percentage ${percentage}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('onUploadProgress', status, percentage);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onUploadProgress callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onUploadProgress callback", err)
                }
            }, (deviceId, pairedListInfo) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onBTParingListChange", (() => `onBTParingListChange event received from native sdk with pairedListInfo ${JSON.stringify(pairedListInfo, null, 3)}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('onBTParingListChange', pairedListInfo);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onBTParingListChange callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onBTParingListChange callback", err)
                }
            }, (deviceId, buttonEvents) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onGNPBtnEvent", (() => `onGNPBtnEvent event received from native sdk with buttonEvents=${buttonEvents}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('onGNPBtnEvent', buttonEvents);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onGNPBtnEventChange callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onGNPBtnEventChange callback", err)
                }
            }, (deviceId, dectInfo) => {
                try {
                    _JabraNativeAddonLog(AddonLogSeverity.verbose, "JabraType::constructor::onDectInfoEvent", (() => `onDectInfoEvent event received from native sdk with dectInfo=${dectInfo}`));
                    let device = this.deviceTypes.get(deviceId);
                    if (device) {
                        device._eventEmitter.emit('onDectInfoEvent', dectInfo);
                    } else {
                        _JabraNativeAddonLog(AddonLogSeverity.error, "onDectInfoEvent callback", "Could not lookup device with id " + deviceId);
                    }
                } catch (err) {
                    // Log but do not propagate js errors into native caller (or node process will be aborted):
                    _JabraNativeAddonLog(AddonLogSeverity.error, "JabraType::constructor::onDectInfoEvent callback", err);
                }
            },
            configParams);  
        });
    }

    /**
     * The user must call this function when finished using the wrapper. Otherwise
     * the node process will not shutdown properly.
     * 
     * @param {boolean} shutdownServer Optional parameter only applicable to electronrendererhelper,
     * where it cause API server shutdown if set. Can safely be ignored otherwise.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    disposeAsync(shutdownServer: boolean = false): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.info, this.disposeAsync.name, "called");

        this.eventEmitter.removeAllListeners();

        let retPromise: Promise<void>;
        if (!sdkIntegration.UnInitialize()) {
            _JabraNativeAddonLog(AddonLogSeverity.error, this.disposeAsync.name, "Dispose of API failed.")
            retPromise = Promise.reject(new Error("Failed uninitializing"));
        } else {
            this.deviceTypes.clear();
            jabraApp = null;
            _JabraNativeAddonLog(AddonLogSeverity.info, this.disposeAsync.name, "Dispose of API succeded")
            retPromise = Promise.resolve();
        }

        return retPromise;
    }

    /**
     * Get list of currently attached Jabra devices.
     */
    getAttachedDevices(): DeviceType[] {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getAttachedDevices.name, "called");
        const result = Array.from(this.deviceTypes.values());
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getAttachedDevices.name, "returned", result);
        return result;
    }

    /**
     * Integrates softphone app to Jabra applications like Jabra Direct(JD) and Jabra Suite for Mac(JMS).
     * @param {string} guid Client unique ID.
     * @param {string} softphoneName Name of the application to be shown in JD or JMS.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`. 
     * - Returns `true` if softphone app integrates to Jabra application, `false` otherwise.
     */   
    connectToJabraApplicationAsync(guid: string, softphoneName: string): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectToJabraApplicationAsync.name, "called with ", guid, softphoneName);
        return util.promisify(sdkIntegration.ConnectToJabraApplication)(guid, softphoneName).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.connectToJabraApplicationAsync.name, "returned", result);
            return result;
        });
    }

    /**
     * Disconnects connected from Jabra applications.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`. 
     */
    disconnectFromJabraApplicationAsync(): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.disconnectFromJabraApplicationAsync.name, "called");        
        return util.promisify(sdkIntegration.DisconnectFromJabraApplication)().then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.disconnectFromJabraApplicationAsync.name, "returned");
        });
    }

    /**
     * Sets the softphone to Ready. Currently applicable for only Jabra Direct.
     * @param {boolean} isReady Sets the softphone readiness state.
     * @returns {Promise<void, JabraError>} - Resolve `void` if successful otherwise Reject with `error`.
     */
    setSoftphoneReadyAsync(isReady: boolean): Promise<void> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSoftphoneReadyAsync.name, "called with", isReady);
        return util.promisify(sdkIntegration.SetSoftphoneReady)(isReady).then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.setSoftphoneReadyAsync.name, "returned");
        });
    }

    /**
     * Indicates whether the softphone is in focus.
     * @returns {Promise<boolean, JabraError>} - Resolve `boolean` if successful otherwise Reject with `error`.
     * - Returns `true` if softphone is in focus, `false` otherwise.
     */
    isSoftphoneInFocusAsync(): Promise<boolean> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isSoftphoneInFocusAsync.name, "called"); 
        return util.promisify(sdkIntegration.IsSoftphoneInFocus)().then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.isSoftphoneInFocusAsync.name, "returned with", result);
            return result;
        });
    }

    /** 
     * Wait for initial device scan to be done.
     */
    scanForDevicesDoneAsync(): Promise<void>  {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.scanForDevicesDoneAsync.name, "called"); 
        return this.firstScanForDevicesDonePromise.then(() => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.scanForDevicesDoneAsync.name, "returned");
        });
    }    

    /**
     * Get the SDK version.
     * @returns {Promise<string, JabraError>} - Resolve SDK version `string` if successful otherwise Reject with `error`.
     */
    getSDKVersionAsync(): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSDKVersionAsync.name, "called");
        return util.promisify(sdkIntegration.GetVersion)().then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getSDKVersionAsync.name, "returned with", result);
            return result;
        });
    }

    /**
     * Get return error string from a previously returned SDK error status.
     * @param {number} errStatusCode Status code of the error from the Jabra Device.
     * @returns {Promise<string, JabraError>} - Resolve Error String `string` if successful otherwise Reject with `error`.
    */
    getErrorStringAsync(errStatusCode: number): Promise<string> {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getErrorStringAsync.name, "called"); 
        return util.promisify(sdkIntegration.GetErrorString)(errStatusCode).then((result) => {
            _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getErrorStringAsync.name, "returned with", result);
            return result;
        });
    }

    /** 
     * Internal function for N-API experimentation only - it may be removed/changed at 
     * any time without warning - do not call.
     * 
     * @internal 
     * @hidden
     **/
    _SyncExperiment(p?: any): any {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this._SyncExperiment.name, "called with", p);
        const result = sdkIntegration.SyncExperiment(p);
        if (result instanceof Promise) {
            return result.then((result: any) => {
                _JabraNativeAddonLog(AddonLogSeverity.verbose, this._SyncExperiment.name, "returned with", result);
                return result;
            });
        } else {
            return result;
        }
    }

    /**
     * Get meta information about methods, properties etc. that can be used 
     * for reflective usage of this class.
     */
    getMeta() : ClassEntry {
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getMeta.name, "called"); 
        const jabraClassName = this.constructor.name;
        const apiMeta = _getJabraApiMetaSync();
        let jabraTypeMeta = apiMeta.find((c) => c.name === jabraClassName);
        if (!jabraTypeMeta)
            throw new Error("Could not find meta data for " + jabraClassName);
        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.getMeta.name, "returned with", apiMeta);            
        return jabraTypeMeta;
    }

    /**
     * Add event handler for attach device events. The attach event is 
     * particulary important, since this callback is where you get a reference to a DeviceType
     * object with detailed API for the device.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    on(event: 'attach', listener: JabraTypeCallbacks.attach): this; 
    
    /**
     * Add event handler for detach device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    on(event: 'detach', listener: JabraTypeCallbacks.detach): this;

    /**
     * Add event handler for firstScanDone device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    on(event: 'firstScanDone', listener: JabraTypeCallbacks.firstScanDone): this;

    /**
     * Add event handler for attach, detach or firstScanDone device events. The attach event is 
     * particulary important, since this callback is where you get a reference to a DeviceType
     * object with detailed API for the device.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    on(event: JabraTypeEvents,
        listener: JabraTypeCallbacks.attach | JabraTypeCallbacks.detach | JabraTypeCallbacks.firstScanDone): this {

        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.on.name, "called with", event, "<listener>"); 

        this.eventEmitter.on(event, listener);

        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.on.name, "returned"); 

        return this;
    }

    /**
     * Remove previosly setup event handler for attach device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    off(event: 'attach', listener: JabraTypeCallbacks.attach): this;

    /**
     * Remove previosly setup event handler for detach device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    off(event: 'detach', listener: JabraTypeCallbacks.detach): this;

    /**
     * Remove previosly setup event handler for firstScanDone device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    off(event: 'firstScanDone', listener: JabraTypeCallbacks.firstScanDone): this;

    /**
     * Remove previosly setup event handler for attach, detach or firstScanDone device events.
     * 
     * *Please make sure your callback arguments matches the event type or you will get a misleading typescript error. See also {@link https://github.com/microsoft/TypeScript/issues/30843 30843}*
     */
    off(event: JabraTypeEvents,
        listener: JabraTypeCallbacks.attach | JabraTypeCallbacks.detach | JabraTypeCallbacks.firstScanDone): this {

        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.off.name, "called with", event, "<listener>"); 

        this.eventEmitter.off(event, listener);

        _JabraNativeAddonLog(AddonLogSeverity.verbose, this.off.name, "returned"); 

        return this;
    }
}
