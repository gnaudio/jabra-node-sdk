/**
 * This file contains type definitions shared between the public api 
 * and the internal sdk integration.
 */

import { enumDeviceConnectionType, enumSettingCtrlType, enumSettingDataType, enumAPIReturnCode, enumBTPairedListType, enumRemoteMmiSequence, enumAutoWhiteBalance, enumPanDirection, enumTiltDirection, enumZoomDirection, enumProxyType } from './jabra-enums';

/**
 * The type of error returned from rejected Jabra API promises.
 */
export declare type JabraError = Error & {
    /**
     * Jabra return code from native sdk.
     */
    code?: number 
};

/** 
 * Logging level for logging to the native log.
 * 
 * For internal use only!
 */
export declare const enum AddonLogSeverity
{
        none = 0,
        fatal = 1,
        error = 2,
        warning = 3,
        info = 4,
        debug = 5,
        verbose = 6
};

/** 
 * Native log configuration.
 * 
 * For internal use only!
 */
export declare interface NativeAddonLogConfig
{
    maxSeverity: AddonLogSeverity;
    maxSeverityString: AddonLogSeverity;
    configuredLogPath: string;
};

/**
 * @param blockAllNetworkAccess - if true, all network access is blocked
 * @param baseUrl_capabilities -
 *   // optional. The host and path (up to, but excluding the '?') of the source for capability files. The responding host is responsible for parsing the entire URL with parameters delivered and produce a response that is equivalent to the response from the default Jabra endpoints.
     // see https://devicecapabilities.jabra.com/swagger/ui/index (note: v4)
    // Special cases:
    //     null or "": use the default.
* @param baseUrl_fw -
*   // optional. The host and partial path of the source for FW files. The responding host is responsible for parsing the entire URL with parameters delivered and produce a response that is equivalent to the response from the Jabra endpoints.
    // example: https://firmware.jabra.com/v4
    // See https://firmware.jabra.com/swagger/#/v3 for full URL and parameters
    // Special cases:
    //     null or "": use the default.
* @param proxy - optional. specify the proxy to use. Null or "" if a proxy should not be used. Example: "https://myproxyhost:8042". Supported proxy types, see https://curl.haxx.se/libcurl/c/CURLOPT_PROXY.html
*/
export interface ConfigParamsCloud {
    blockAllNetworkAccess?: boolean,
    baseUrl_capabilities?: string,
    baseUrl_fw?: string,
    proxy?: string,
}

/** 
 * Misc non-cloud config parameters.
 * 
 * @internal 
 **/
export interface GenericConfigParams {
    nonJabraDeviceDectection: boolean,
}

export interface DeviceCatalogueParams {
    preloadZipFile: string,
    delayInSecondsBeforeStartingRefresh: number,
    refreshAtConnect: boolean,
    refreshScope: number,
    fetchDataForUnknownDevicesInTheBackground: boolean,
}

/**
 * Holds information about the network interface.
 */
export interface IPv4
{
    octet1: number;
    octet2: number;
    octet3: number;
    octet4: number;
}
export interface IPv4Status
{
    interfaceEnabled: boolean;
    DHCPEnabled: boolean;
    connectionStatus: boolean;
    IP: IPv4;
    subnetMask: IPv4;
}

/** Action controls for pan/tilt/zoom.*/
export interface PanTiltRelative
{
    panDirection: enumPanDirection;
    panSpeed: number; // Currently must be 1
    tiltDirection: enumTiltDirection;
    tiltSpeed: number; // Currently must be 1
}
export interface ZoomRelative
{
    direction: enumZoomDirection;
    speed: number; // Currently must be 1
}

/**
 * Used when getting/setting date/time on device.
 **/
export interface DateTime {
    sec: number;  // second      [0,59]
    min: number;  // minute      [0,59]
    hour: number; // hour        [0,23]
    mday: number; // day of month[1,31]
    mon: number;  // month       [0,11]
    year: number; // year        >99 (offset from year 1900)
    wday: number; // day of week [0,6] (0 equals sunday) - not settable, automatic based on DDMMYY
}

/** 
 * Device information data properties supported by DeviceType
 * and carried between js and native code. 
 * 
 * Nb. Not for general use - Intended for internal use.
 **/
export interface DeviceInfo {
    deviceID: number;
    productID: number;
    vendorID: number;
    deviceName: string;
    usbDevicePath?: string;
    parentInstanceId?: string; // Not currently exposed in js so marked as optional.
    errorStatus: number; // = errStatus in SDK.
    isDongleDevice: boolean; // = isDongle in SDK.
    dongleName?: string;// Not currently exposed in js so marked as optional.
    variant: string;
    ESN: string; // = serialNumber in SDK.
    isInFirmwareUpdateMode: boolean;
    connectionType: enumDeviceConnectionType; // = deviceconnection in SDK.
    connectionId?: number; // Not currently exposed in js so marked as optional.
    parentDeviceId?: number; // Not currently exposed in js so marked as optional.
}

/** 
 * Device attach/detach timing information.
 **/
export interface DeviceTiming {
  /**
     * The time since EPOC that the device was attached.
     */
    readonly attached_time_ms: number;
    /**
     * The time since EPOC that the device was subsequently detached (if no longer attached only).
     *
     */
    readonly detached_time_ms?: number;
}

export interface RCCStatus {
    isMuted: boolean,
    isOffHooked: boolean,
    isOnHold: boolean,
    isRinging: boolean
}

export declare const enum AudioFileFormatEnum {
    AUDIO_FILE_FORMAT_NOT_USED = 0,
    AUDIO_FILE_FORMAT_WAV_UNCOMPRESSED = 1,
    AUDIO_FILE_FORMAT_ULAW_COMPRESSED = 2,
    AUDIO_FILE_FORMAT_SBC_COMPRESSED = 3,
    AUDIO_FILE_FORMAT_G72_COMPRESSED =4,
};

export interface ConfigInfo {
    configName: string,
    configId: string
}

export interface FirmwareInfoType {
    version: string;
    fileSize: string;
    releaseDate: string;
    stage: string;
    releaseNotes: string;
}

export interface SettingType {
    guid: string,
    name: string,
    helpText: string,
    cntrlType: enumSettingCtrlType,
    currValue: number | string,
    settingDataType: enumSettingDataType,
    listSize?: number,
    groupName: string,
    groupHelpText: null | undefined | string,
    isPCsetting: boolean,
    isDeviceRestart: boolean,
    isWirelessConnect: boolean,
    isChildDeviceSetting: boolean,
    isDepedentsetting: boolean,
    dependentDefaultValue: null | undefined | number | string,
    isValidationSupport: boolean,
    validationRule: null | undefined | { minLength: number, maxLength: number, regExp: string, errorMessage: string },
    listKeyValue: Array<{ key: number, value: string, dependentcount?: number, dependents: Array<{ GUID: string, enableFlag: boolean }> }>
}

export interface DeviceSettings {
  errStatus?: enumAPIReturnCode;
  settingInfo: Array<SettingType>;
};

export interface PairedListInfo  { 
    listType: enumBTPairedListType;
    pairedDevice: Array<{ deviceName: string, deviceBTAddr: string, isConnected: boolean }>;
};

export interface NamedAsset {
    elements: Array<{url: string, mime: string}>;
    metadata: Array<{name: string, value: string}>
}

/**
 * Event data type for dev log call backs.
 */
export interface DevLogData {
    AppID: string;
    ESN: string;
    FW: string;
    LocalTimeStamp: string;

    /*
    * TODO: Consider changing this to a proper number
    */
    Pid: string;
    Variant: string;
    "Device Name": string;
    "Raw data": string;
    "Seq.No": number;
    "TX Acoustic Logging Level"?: string;
    "TX Acoustic Logging Peak"?: string;
    "RX Acoustic Logging Level"?: string;
    "RX Acoustic Logging Peak"?: string;
    "Speech_Analysis_TX"?: string;
    "Speech_Analysis_RX"?: string;
    "Boom Position Guidance OK"?: string;
    "Bad_Mic_detect Flag"?: string;
    "ID?": string;
  }

/**
 * Action output interface for Remote Mmi. 
 */  

export interface RemoteMmiActionOutput {
    red: number;
    green: number;
    blue: number;
    sequence: enumRemoteMmiSequence;
}

/** Dect information about density and error counts */

/*
    This type is intentionally not exported. It just collects the common
    properties of all the DectInfo types. It's not meant to be used by the API
    client, which should operate with the DectInfo algebraic data type itself.
*/
interface DectInfoCommon {
    kind: DectInfo.Kind;
    rawData :Uint8Array;
}

/*
    The DectInfo union type describes an algebraic data type.

    Polimorphism would work too, and would allow for shared properties to be
    grouped in the supertype. However, polymorphism doesn't trigger TypeScript
    type guards, which are immensely useful for the API client.

    For this reason, common properties have still been grouped in the
    DectInfoCommon interface, but such intreface has been kept private, as this
    is by now an implementation detail.

    More info on the TypeScript implementation of algebraic data types here:
    https://www.typescriptlang.org/docs/handbook/advanced-types.html#discriminated-unions
*/
export type DectInfo = DectInfoDensity | DectInfoErrorCount;
export namespace DectInfo {
    export type Kind = 'density' | 'errorCount';
}

export interface DectInfoDensity extends DectInfoCommon {
    kind: 'density';
    sumMeasuredRSSI: number;		/* This is the sum of RSSI measured for all slots. */
    maximumReferenceRSSI: number;   /* This is the maximum RSSI expected to be measured from 1 slot. */
    numberMeasuredSlots: number;    /* Number of slots measured in current communication mode. */
    dataAgeSeconds: number;		    /* Time since measurement was taken. */
}

export interface DectInfoErrorCount extends DectInfoCommon {
    kind: 'errorCount';
    syncErrors: number;		/* Number of errors in SYNC field.*/
    aErrors: number;		/* Number of errors in A field.*/
    xErrors: number;		/* Number of errors in X field.*/
    zErrors: number;		/* Number of errors in Z field.*/
    hubSyncErrors: number;	/* Number of errors in HUB Sync field.*/
    hubAErrors: number;		/* Number of errors in HUB A field.*/
    handoversCount: number; /* Handover count.*/
}

export interface Point2D {
    x: number;  /* X coordinate of the point */
    y: number;  /* y coordinate of the point */
}

export interface WhiteboardPosition {
    lowerLeftCorner: Point2D;   /* Lower-left corner of the whiteboard */
    lowerRightCorner: Point2D;  /* Lower-right corner of the whiteboard */
    upperRightCorner: Point2D;  /* Upper-right corner of the whiteboard */
    upperLeftCorner: Point2D;   /* Upper-left corner of the whiteboard */
}

/**
 * This structure represents a device camera's limits for parameters such
 * as sharpness and saturation.
 */
export interface VideoLimits {
    /* The minimum value the parameter level can be set to. */
    min: number;
    /* The maximum value the parameter level can be set to. */
    max: number;
    /* The minimum amount that the value needs to change for it to have any effect. */
    stepSize?: number;
}

/**
 * This structure represents a video device's streaming status.
 */
 export interface VideoDeviceStreamingStatus {
    /* Whether the device is actively streaming video. */
    videoStreamingStatus: boolean;
    /* Whether the device is actively streaming audio. */
    audioStreamingStatus: boolean;
}

/**
 * This structure represents a device camera's pan an tilt values.
 */
export interface PanTilt {
  pan: number,
  tilt: number
}

/**
 * This structure represents a device camera's pan and tilt limits
 */
export interface PanTiltLimits {
    pan: VideoLimits,
    tilt: VideoLimits
  }

/**
 * This structure represents the white balance settings
 */
export interface WhiteBalance {
    value: number,
    autoWB: enumAutoWhiteBalance
  }

/**
 * Proxy settings for Xpress management configuration (for networked devices)
 * Leaving 'url' empty will disable proxy
 */
export interface ProxySettings {
    type: enumProxyType;
    url: string;
    port: number;
    username: string;
    password: string;
}
