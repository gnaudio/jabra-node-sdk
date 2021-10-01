
/**
 * This enum is used for the return values from API.
 */
export enum enumAPIReturnCode {
  Return_Ok = 0,
  Device_Unknown = 1,
  Device_Invalid = 2,
  Not_Supported = 3,
  Return_ParameterFail = 4,
  ProtectedSetting_Write = 5,
  No_Information = 6,
  NetworkRequest_Fail = 7,
  Device_WriteFail = 8,
  Device_ReadFails = 9,
  No_FactorySupported = 10,
  System_Error = 11,
  Device_BadState = 12,
  FileWrite_Fail = 13,
  File_AlreadyExists = 14,
  File_Not_Accessible = 15,
  Firmware_UpToDate = 16,
  Firmware_Available = 17,
  Return_Async = 18,
  Invalid_Authorization = 19,
  FWU_Application_Not_Available = 20,
  Device_AlreadyConnected = 21,
  Device_NotConnected = 22,
  CannotClear_DeviceConnected = 23,
  Device_Rebooted = 24,
  Upload_AlreadyInProgress = 25,
  Download_AlreadyInProgress = 26,
  Sdk_TooOldForFwUpdate = 27,
  No_OtaUpdateSupport = 28,
  Non_Jabra_Device_Detection_disabled = 29,
  Device_Lock = 30,
  Device_NotLock = 31,
  Return_Timeout = 32,
}


/**
 * When jabra device is attached, various info(i.e downloading manifest file) about this device is fetched from cloud. Any error status during this process is represented by this Enum.
 */
export enum enumDeviceErrorStatus {
  NoError = 0,
  SSLError = 1,
  CertError = 2,
  NetworkError = 3,
  DownloadError = 4,
  ParseError = 5,
  OtherError = 6,
  DeviceInfoError = 7,
  FileNotAccessible = 8,
  FileNotCompatible = 9,
  Device_NotFound = 10,
  Parameter_fail = 11,
  Authorization_failed = 12,
  FileNotAvailable = 13,
  ConfigParseError = 14,
  SetSettings_Fail = 15,
  Device_Reboot = 16,
  Device_ReadFail = 17,
  Device_NotReady = 18,
  FilePartiallyCompatible = 19,
  SdkTooOldForFwUpdateError = 20,
}

/**
 * This enum is used to represent Jabra device button.
 */
export enum enumDeviceBtnType {
  Undefined = 0,
  OffHook = 1,  
  Mute = 2, 
  Flash = 3,
  Redial = 4,
  Key0 = 5,
  Key1 = 6,
  Key2 = 7,
  Key3 = 8,
  Key4 = 9,
  Key5 = 10,
  Key6 = 11,
  Key7 = 12,
  Key8 = 13,
  Key9 = 14,
  KeyStar = 15,
  KeyPound = 16,
  KeyClear = 17,
  Online = 18,
  SpeedDial = 19,
  VoiceMail = 20,
  LineBusy = 21,
  RejectCall = 22,
  OutOfRange = 23,
  PseudoOffHook = 24,
  Button1 = 25,
  Button2 = 26,
  Button3 = 27,
  VolumeUp = 28,
  VolumeDown = 29,
  FireAlarm = 30,
  JackConnection = 31,
  QDConnection = 32,
  HeadsetConnection = 33,
}

/**
 * This enum is used to represent how device is connected to system via USB or Bluetooth.<br>
 * `USB` - device that connect through USB.<br>
 * `Bluetooth` - device that connect through dongle.
 */
export enum enumDeviceConnectionType {
  USB = 0,
  BT = 1
}

/**
 * Enum used for the current value of setting. Setting value is either number or string
 */
export enum enumSettingDataType {
  NUMBER = 0,
  STRING = 1
}

/**
 * Enum used for setting control type
 */
export enum enumSettingCtrlType {
  RADIO = 0,
  TOGGLE = 1,
  COMBOBOX = 2,
  DROPDOWN = 3,
  LABEL = 4,
  TEXTBOX = 5,
  BUTTON = 6,
  EDITBUTTON = 7,
  HORZRULER = 8,
  PWDTEXTBOX = 9,
  UNKNOWN = 10
}

/**
 * This enum represents the settings load mode for the API
 */
export enum enumSettingLoadMode  {
  expressMode = 0,
  retrieveMode = 1
}

/**
 * This enum represents the firmware update status
 */
export enum enumFirmwareEventStatus {
  Initiating = 0,
  InProgress = 1,
  Completed = 2,
  Cancelled = 3,
  File_NotAvailable = 4,
  File_NotAccessible = 5,
  File_AlreadyPresent = 6,
  Network_Error = 7,
  SSL_Error = 8,
  Download_Error = 9,
  Update_Error = 10,
  Invalid_Authentication = 11,
  File_UnderDownload = 12,
  Not_Allowed = 13,
  Sdk_TooOldForUpdate = 14
}

/**
 * This enum represents the firmware type
 */
export enum enumFirmwareEventType {
  Firmware_Download = 0,
  Firmware_Update = 1
}

/**
 * This enum represents the bluetooth paired list type
 */
export enum enumBTPairedListType {
  SearchResult = 0,
  PairedDevices = 1,
  SearchComplete = 2
}

export enum enumUploadEventStatus {
  Upload_InProgress = 0,
  Upload_Completed = 1,
  Upload_Error = 2
}

export enum audioFileFormat {
  AUDIO_FILE_FORMAT_NOT_USED = 0,
  AUDIO_FILE_FORMAT_WAV_UNCOMPRESSED = 1,
  AUDIO_FILE_FORMAT_ULAW_COMPRESSED = 2,
  AUDIO_FILE_FORMAT_SBC_COMPRESSED = 3,
  AUDIO_FILE_FORMAT_G72_COMPRESSED = 4
}

/**
 * This enum represents the features are available to Device.
 */
export enum enumDeviceFeature {
  BusyLight = 1000,
  FactoryReset = 1001,
  PairingList = 1002,
  RemoteMMI = 1003,
  MusicEqualizer = 1004,
  EarbudInterconnectionStatus = 1005,
  StepRate = 1006,
  HeartRate = 1007,
  RRInterval = 1008,
  RingtoneUpload = 1009,
  ImageUpload = 1010,
  NeedsExplicitRebootAfterOta = 1011,
  NeedsToBePutIncCradleToCompleteFwu = 1012,
  RemoteMMIv2 = 1013,
  Logging = 1014,
  PreferredSoftphoneListInDevice = 1015,
  VoiceAssistant = 1016,
  PlayRingtone = 1017,
  SetDateTime = 1018,
  FullWizardMode = 1019,
  LimitedWizardMode = 1020,
  OnHeadDetection = 1021,
  SettingsChangeNotification = 1022,
  AudioStreaming = 1023,
  CustomerSupport = 1024,
  MySound = 1025,
  UIConfigurableButtons = 1026,
  ManualBusyLight = 1027,
  Whiteboard = 1028,
  Video = 1029,
  AmbienceModes = 1030,
  SealingTest = 1031,
  AMASupport = 1032,
  AmbienceModesLoop = 1033,
  FFANC = 1034,
  GoogleBisto = 1035,
  VirtualDirector = 1036,
  PictureInPicture = 1037,
  DateTimeIsUTC = 1038,
  RemoteControl = 1039,
  UserConfigurableHDR = 1040,
}

/**
 * Enum for setting the HID working state.
 */
export enum enumHidState {
  NOT_SUPPORTED = 0,
  STD_HID = 1,
  GN_HID = 2
}

export enum enumWizardMode {
  FULL_WIZARD = 0,
  LIMITED_WIZARD = 1,
  NO_WIZARD = 2
}

export enum enumLogging {
  Local = 0,
  Cloud = 1,
  All = 2
}

export enum enumSecureConnectionMode {
  /** Normal pairing allowed */
  SC_LEGACY_MODE = 0,
  /** Device is allowed to connect a audio gateway eg. a mobile phone */
  SC_SECURE_MODE,
  /** Pairing not allowed */
  SC_RESTRICTED_MODE
}

/** Represents a color control  preset slot on the device.*/
export enum enumColorControlPreset {
  PRESET1 = 0
}

/** Represents a PTZ preset slot on the device.*/
export enum enumPTZPreset {
  PRESET1 = 0,
  PRESET2 = 1,
  PRESET3 = 2,
  DEFAULTS = 0xFF
}

/** Enum with the notification styles.*/
export enum enumNotificationStyle {
  LED = 0,                /* Use only LED(s) */
  LED_AND_TONES = 1,      /* Use LED and tones */
  LED_TONES_AND_VOICE = 2 /* Use LED, tones and voice warning */
}

/** Enum with the notification usages.*/
export enum enumNotificationUsage {
  WHEN_VIDEO_IS_ENABLED = 0, /* Only enable during video calls */
  ALWAYS = 1                 /* Always enable */
}

/** Enum with choices for automatic white balance.*/
export enum enumAutoWhiteBalance {
  USE_WB_TEMP_VALUE = 0,        /* Use manual WB setting */
  AUTO_ADJUSTMENT = 1           /* Automatic white balance */
}

/** Choices for the secondary stream.*/
export enum enumSecondaryStreamContent
{
  Disabled = 0,
  Whiteboard1 = 1,
  Whiteboard2 = 2,
  AIFullscreen = 3
}

/** Pan/tilt/zoom actions.*/
export enum enumPanDirection
{
    stop = 0,
    clockwise = 1,
    counterClockwise = 0xFF
};
export enum enumTiltDirection
{
    stop = 0,
    up = 1,
    down = 0xFF
};
export enum enumZoomDirection
{
    stop = 0,
    telephoto = 1,
    wideAngle = 0xFF
};

/** How the device should control the video.*/
export enum enumVideoMode
{
    FULL_SCREEN = 0,        /* No automatic control */
    INTELLIGENT_ZOOM = 1,
    ACTIVE_SPEAKER = 2
};

/** The video transition styles when using Intelligent Zoom. */
export enum enumVideoTransitionStyle
{
    IMMEDIATE = 0,
    SMOOTH = 1
};

/** How long to stay with a speaker when IZ is enabled.*/
export enum enumIntelligentZoomLatency
{
    FASTEST = 0,
    FASTER = 1,
    MEDIUM = 3,
    SLOWER = 5,
    SLOWEST = 6
};

/**
 * Enum for types of remote MMI 
 */
export enum enumRemoteMmiType {
  MMI_TYPE_MFB       = 0,
  MMI_TYPE_VOLUP     = 1,
  MMI_TYPE_VOLDOWN   = 2,
  MMI_TYPE_VCB       = 3,
  MMI_TYPE_APP       = 4,
  MMI_TYPE_TR_FORW   = 5,
  MMI_TYPE_TR_BACK   = 6,
  MMI_TYPE_PLAY      = 7,
  MMI_TYPE_MUTE      = 8,
  MMI_TYPE_HOOK_OFF  = 9,
  MMI_TYPE_HOOK_ON   = 10,
  MMI_TYPE_BLUETOOTH = 11,
  MMI_TYPE_JABRA     = 12,
  MMI_TYPE_BATTERY   = 13,
  MMI_TYPE_PROG      = 14,
  MMI_TYPE_LINK      = 15,
  MMI_TYPE_ANC       = 16,
  MMI_TYPE_LISTEN_IN = 17,
  MMI_TYPE_DOT3      = 18,
  MMI_TYPE_DOT4      = 19,
  MMI_TYPE_MEDIA     = 20,
  SEPERATOR_FOR_MMI_TYPE = 128, /* not to be used */
  MMI_TYPE_BUSYLIGHT = SEPERATOR_FOR_MMI_TYPE,
  MMI_TYPE_LED_BUSYLIGHT = SEPERATOR_FOR_MMI_TYPE,
  MMI_TYPE_LED_MULTIFUNCTIONAL = SEPERATOR_FOR_MMI_TYPE + 1,
  MMI_TYPE_LED_MUTE = SEPERATOR_FOR_MMI_TYPE + 2
};

/**
 * Enum for Remote MMI input, used to identify supported input actions
 */
export enum enumRemoteMmiInput {
  MMI_ACTION_NONE          = 0x00,
  MMI_ACTION_UP            = 0x01,
  MMI_ACTION_DOWN          = 0x02,
  MMI_ACTION_TAP           = 0x04,
  MMI_ACTION_DOUBLE_TAP    = 0x08,
  MMI_ACTION_PRESS         = 0x10,
  MMI_ACTION_LONG_PRESS    = 0x20,
  MMI_ACTION_X_LONG_PRESS  = 0x40
};

/**
 * Enum for remote MMI priorities
 */
export enum enumRemoteMmiPriority {
  /**
   * Used for remote MMIs that does not support priority.
   */
  MMI_PRIORITY_NONE    = 0x00,
  /**
   * Get remote MMI focus if device doesn't use it or no function is assigned
   * to the button.
   */
  MMI_PRIORITY_LOW     = 0x01,
  /**
   * Get remote MMI focus unconditionally, this can remove important
   * functionality from the device.
   */
  MMI_PRIORITY_HIGH    = 0x02
};

/**
 * Enum for remote MMI light sequence
 */
export enum enumRemoteMmiSequence {
  MMI_LED_SEQUENCE_OFF     = 0x01,
  MMI_LED_SEQUENCE_ON      = 0x02,
  MMI_LED_SEQUENCE_SLOW    = 0x04,
  MMI_LED_SEQUENCE_FAST    = 0x08
};

/**
 * Proxy types
 * See https://everything.curl.dev/libcurl/proxies for details
 */
export enum enumProxyType {
  HTTP     = 0,
  HTTPS    = 1,
  SOCKS4   = 2,
  SOCKS4A  = 3,
  SOCKS5   = 4,
  SOCKS5H  = 5,
};

/**
 * USB Connection states
 */
 export enum enumUSBState {
  Unplugged             = 0,
  Error                 = 1,
  USB_2_0_HighSpeed     = 2, // USB 2.0
  USB_3_0_SuperSpeed    = 3, // USB 3.0
  USB_3_2_Gen2          = 4  // USB 3.2 Gen2
};
