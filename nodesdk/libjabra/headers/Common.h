#ifndef COMMON_H
#define COMMON_H

/* --------------------------------------------------------------------
 *
 *  GGGGGG  NN    N
 *  G       N N   N
 *  G  GGG  N  N  N - Audio
 *  G    G  N   N N
 *  GGGGGG  N    NN
 *
 *  Copyright (c) 2018, GN-Audio
 * -------------------------------------------------------------------- */

/**
 * @file Common.h
 * @brief Defines the common interface for the Jabra SDK.
 * Developer information can be found in the SDK documentation.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <stdint.h>
#include <wchar.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#if defined _WIN32 || defined __CYGWIN___
#define LIBRARY_API extern "C" __declspec(dllexport)
#else
#ifdef __APPLE__
#ifdef __cplusplus
#define LIBRARY_API extern "C" __attribute__ ((visibility ("default")))
#else
#define LIBRARY_API __attribute__ ((visibility ("default")))
#endif
#include <stdbool.h>
#elif __linux__
#ifdef __cplusplus
#define LIBRARY_API extern "C" __attribute__ ((visibility ("default")))
#else
#define LIBRARY_API __attribute__ ((visibility ("default")))
#include <stdbool.h>
#endif
#endif
#endif

#if defined _WIN32 || defined __CYGWIN___
#define LIBRARY_VAR extern "C" __declspec(dllexport)
#else
#ifdef __cplusplus
#define LIBRARY_VAR extern "C" __attribute__ ((visibility ("default")))
#else
#define LIBRARY_VAR extern __attribute__ ((visibility ("default")))
#endif
#endif

typedef enum _DeviceListType {
  SearchResult,
  PairedDevices,
  SearchComplete
} Jabra_DeviceListType;

/** @brief Paired devices list structure. */
typedef struct _PairedDevice {
  /** @brief Name of device, this is end-user editable i.e. it is not
   * necessarily the name of the product. */
  char* deviceName;
  /** @brief Bluetooth MAC address. */
  uint8_t deviceBTAddr[6];
  /** Current connection status. */
  bool isConnected;
} Jabra_PairedDevice;

/** @brief List of paired devices list structure. */
typedef struct _PairingList {
  /** Number of #Jabra_PairedDevice elements in the list. */
  unsigned short count;
  /** List type identifier. */
  Jabra_DeviceListType listType;
  /** Paired device information. */
  Jabra_PairedDevice* pairedDevice;
} Jabra_PairingList;

/** @brief The secure connection modes */
typedef enum _SecureConnectionMode {
    /** Normal pairing allowed */
    SC_LEGACY_MODE = 0,
    /** Device is allowed to connect a audio gateway eg. a mobile phone */
    SC_SECURE_MODE,
    /** Pairing not allowed */
    SC_RESTRICTED_MODE
} Jabra_SecureConnectionMode;

/**
 * @brief This enum is used for the return values from API.
 * @def DEFINE_CODE macro is used to a and b.
 */
#define DEFINE_CODE(a,b) a,
typedef enum _ReturnCode {
#include "returncodes.inc"
  NUMBER_OF_JABRA_RETURNCODES
} Jabra_ReturnCode;
#undef DEFINE_CODE

#define DEFINE_CODE(a,b) a,
typedef enum _ErrorStatus {
#include "errorcodes.inc"
  NUMBER_OF_JABRA_ERRORCODES
} Jabra_ErrorStatus;
#undef DEFINE_CODE

typedef enum _DeviceConnectionType {
  USB = 0,
  BT
} DeviceConnectionType;

/** @brief Device description structure, identifies attached devices. */
typedef struct _DeviceInfo {
  /** @brief Device identifier, used for most API calls to identify the device
   * to operate on. */
  unsigned short deviceID;
  /** @brief Product identifier, used for identifying the type of product. */
  unsigned short productID;
  /** @brief Vendor identifier, used for identifying the vendor. Jabra vendor
   * ID is 0x0B0E. */
  unsigned short vendorID;
  char* deviceName;
  char* usbDevicePath;
  char* parentInstanceId;
  Jabra_ErrorStatus errStatus;
  bool isDongle;
  char* dongleName;
  char* variant;
  char* serialNumber;
  bool isInFirmwareUpdateMode;
  DeviceConnectionType deviceconnection;
  unsigned long connectionId;
  unsigned short parentDeviceId;
} Jabra_DeviceInfo;

/**
 * This structure represents each button event type info. For example: Tap
 * (00), Press(01), Double Tap(02) etc. */
typedef struct _ButtonEventType {
  /** Hex value for button event. Key can be 00 for Tap, 01 for Press, 02
   * for double tap etc. */
  unsigned short key;
  /** Description of button event. For example: value can be "Tap" or
   * "Press" or "Double tap". */
  char *value;
} ButtonEventType;

/**
 * This structure represents each Remote MMI info. For example Volume up/down
 * button is supported by Tap, MFB button is supported by Tap/Press/Double Tap.
 */
typedef struct _ButtonEventInfo {
  /** Hex value for button type. For example: Volume up(01), Volume down(02)
   * etc. */
  unsigned short buttonTypeKey;
  /** Description of button type. For example: buttonTypeValue can be
   * "Volume Up" or "Volume Down" or "MFB". */
  char *buttonTypeValue;
  /** Number of button events under a button type. Ex. If MFB is supported by
   * "Tap", "Press", "Double Tap", then buttonEventTypeSize is 3. */
  int buttonEventTypeSize;
  /** Button event information of all button event types of the device. */
  ButtonEventType *buttonEventType;
} ButtonEventInfo;

/** This structure represents Remote MMI's available for the device. */
typedef struct _ButtonEvent {
  /** Number of Remote MMI's available for the device. If device supports
   * "Volume Up", "Volume Down" and "MFB" as remote MMI, buttonEventCount
   * is 3. */
  int buttonEventCount;
  /** Remote MMI information of all button events of the device. */
  ButtonEventInfo* buttonEventInfo;
} ButtonEvent;

/** Predefined inputs enum. */
typedef enum Jabra_HidInput {
  Undefined,
  OffHook,
  Mute,
  Flash,
  Redial,
  Key0,
  Key1,
  Key2,
  Key3,
  Key4,
  Key5,
  Key6,
  Key7,
  Key8,
  Key9,
  KeyStar,
  KeyPound,
  KeyClear,
  Online,
  SpeedDial,
  VoiceMail,
  LineBusy,
  RejectCall,
  OutOfRange,
  PseudoOffHook,
  Button1,
  Button2,
  Button3,
  VolumeUp,
  VolumeDown,
  FireAlarm,
  JackConnection,
  QDConnection,
  HeadsetConnection,
} Jabra_HidInput;

/** Equalizer. */
typedef struct _EqualizerBand {
  /** The gain (or attenuation) range in dB which the device can handle. Only
   * the positive value (gain) is given, the max attenuation is the
   * corresponding negative value. Read-only. */
  float max_gain;
  /** Band center frequency in Hz. Read-only. */
  int centerFrequency;
  /** The current gain setting [dB] for the band. Must be numerically <=
   * max_gain. Read-only. */
  float currentGain;
} Jabra_EqualizerBand;

/** Logging Flags. */
typedef enum _Logging {
  Local = 0,
  Cloud,
  All
} Jabra_Logging;

/** This structure represents firmware version info of a firmware from
 * cloud. */
typedef struct _FirmwareInfo {
  /** Version of firmware. */
  char *version;
  /** Size of firmware file in KB/MB. */
  char * fileSize;
  /** Release date of firmware. */
  char *releaseDate;
  /** Firmware stage. */
  char *stage;
  /** Release notes of firmware. */
  wchar_t *releaseNotes;
} Jabra_FirmwareInfo;

typedef struct _FirmwareInfoList {
  unsigned count;
  Jabra_FirmwareInfo* items;
} Jabra_FirmwareInfoList;

/** This enum represents event type for callback. */
typedef enum  _FirmwareEventType {
  Firmware_Download = 0,
  Firmware_Update
} Jabra_FirmwareEventType;

/** @todo needs to be consolidated with other result codes. */
typedef enum _FirmwareEventStatus {
  Initiating = 0,
  InProgress,
  Completed,
  Cancelled,
  File_NotAvailable,
  File_NotAccessible,
  File_AlreadyPresent,
  Network_Error,
  SSL_Error,
  Download_Error,
  Update_Error,
  Invalid_Authentication,
  File_UnderDownload,
  Not_Allowed,
  Sdk_TooOldForUpdate
} Jabra_FirmwareEventStatus;


/* Firmware update return codes */
typedef enum _FirmwareUpdateReturnCode {
    Success = 0,
    AlreadyRunning,
    FirmwareFileDoesNotMatchDevice,
    HeadsetNotDocked,
    FirmwareUpdateFailed,
    FirmwareAlreadyUpToDate,
    DowngradeNotAllowed,
    SuccessButPowerCycleRequired,
    SuccessButEarbudsMustBePlacedInCradle
} Jabra_FirmwareUpdateReturnCode;

typedef enum _UploadEventStatus {
  Upload_InProgress = 0,
  Upload_Completed,
  Upload_Error,
} Jabra_UploadEventStatus;

typedef enum _DeviceFeature {
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
  PlayRingtone=1017,
  SetDateTime = 1018,
  FullWizardMode = 1019,
  LimitedWizardMode = 1020,
  OnHeadDetection = 1021,
  SettingsChangeNotification = 1022,
  AudioStreaming = 1023,
  CustomerSupport = 1024,
  MySound = 1025,
  UIConfigurableButtons = 1026
} DeviceFeature;

/** This enum represents actions/parameters required to update firmware in a given device. */
typedef enum _DeviceFWURequirement {
  LanguagePackRegion = 2000,                /** It requires selecting a language region pack (display) */
  TunePackRegion = 2001,                    /** It requires selecting a tune region pack */
  LanguageSelection = 2002,                 /** It requires selecting a language (if a device requires region selection, it doesn't require language selection, and viceversa) */
  HasDockableHeadset = 2003,                /** It has a dockable headset that must be docked before updating firmware */
  IsSelfPowered = 2004,                     /** It is self-powered, therefore if it needs to power cycle it must be turned off and on */
  MustBePowerCycledAfterFwUpdate = 2005,    /** It must be power cycled after updating firmware */
  MustBeDockedInCradleAfterFwUpdate = 2006, /** It must be docked in the cradle after updating firmware */
  SupportsOtaUpdate = 2007                  /** Supports OTA updates */
} DeviceFWURequirement;

/** This enum represents all the language regions that can be supported */
typedef enum _Regions {
    EMEA_AU_NZ = 1,
    NA = 2,
    NA_Japan = 3,
    UK_APAC = 4,
    Korean = 5,
    EA_Oceania = 6,
    Global = 7
}Regions;

/** This structure represents a single language with an integer id and a wide character string. */
typedef struct _Language {
    unsigned int id;
    char* languageName;      /** Language name in UTF-8 encoding */
}Language;

/** This structure represents a list of languages */
typedef struct _LanguageList {
    int count;              /** Amount of languages stored in this list */
    Language *languages;
}LanguageList;

/** @brief This structure represents the product registration info. */
typedef struct _ProductRegistration {
  const char *firstName;
  const char *lastName;
  const char *email;
  const char *country;
  const char *appVersion;
  const char *osName;
  const char *osVersion;
  const char *locale;
  bool marketingConsent;
} ProductRegInfo;

typedef enum _AVRCPCommand {
  AVRCPPlay = 0,
  AVRCPPause,
  AVRCPStop,
  AVRCPToggle,
  AVRCPPrevious,
  AVRCPNext
} AVRCPCommand;

typedef enum _SystemComponentID {
  PRIMARY_HEADSET,
  SECONDARY_HEADSET,
  CRADLE,
  OTHER
} SystemComponentID;

typedef struct _MapEntry_Int_String {
  int key;
  char* value;
} MapEntry_Int_String;

typedef struct _Map_Int_String {
  int length;
  MapEntry_Int_String* entries;
} Map_Int_String;

/** Structure to use when setting the date and time of a device using
 * #Jabra_SetDateTime. */
typedef struct _timedate_t {
  /** Seconds, range is [0-59]. */
  int sec;
  /** Minutes, range is [0-59]. */
  int min;
  /** Hours, range is [0-23]. */
  int hour;
  /** Day of month, range is [1-31]. */
  int mday;
  /** Month, range is [0-11]. */
  int mon;
  /** Year, range is [0-n], where 0 is 1900CE. */
  int year;
  /** Day of week, range is [0-6] where 0 is Sunday. */
  int wday;
} timedate_t;

/** Bitmasks for use with #Jabra_SetSubscribedDeviceEvents and
 * #Jabra_GetSupportedDeviceEvents. */
LIBRARY_VAR const uint32_t DEVICE_EVENT_AUDIO_READY;

typedef enum _AUDIO_FILE_FORMAT {
  AUDIO_FILE_FORMAT_NOT_USED = 0,
  AUDIO_FILE_FORMAT_WAV_UNCOMPRESSED,
  AUDIO_FILE_FORMAT_ULAW_COMPRESSED,
  AUDIO_FILE_FORMAT_SBC_COMPRESSED,
  AUDIO_FILE_FORMAT_G72_COMPRESSED,
} AUDIO_FILE_FORMAT;

/** Structure represents the parameters for uploading audio file to device. */
typedef struct _audioFileParams {
  /** Audio file format allowed. */
  AUDIO_FILE_FORMAT audioFileType;
  /** Number of channels present. */
  unsigned int numChannels;
  /** Bits per sample. */
  unsigned int bitsPerSample;
  /** Sample rate of the audio. */
  unsigned int sampleRate;
  /** Maximum file size allowed. */
  unsigned int maxFileSize;
} Jabra_AudioFileParams;

/**
 * Types of remote MMIs, use #Jabra_GetRemoteMmiTypes to determine the types
 * supported for the device in question.
 * @note RemoteMMIv2 only.
 */
typedef enum _RemoteMmiType {
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
} RemoteMmiType;

/**
 * Remote MMI sequences, used to identify supported output LEDs (as a bitmask)
 * and for setting the output LEDs (single bit).
 * @note RemoteMMIv2 only.
 */
typedef enum _RemoteMmiSequence {
  MMI_LED_SEQUENCE_OFF     = 0x01,
  MMI_LED_SEQUENCE_ON      = 0x02,
  MMI_LED_SEQUENCE_SLOW    = 0x04,
  MMI_LED_SEQUENCE_FAST    = 0x08
} RemoteMmiSequence;

/**
 * Remote MMI priorities.
 * @note RemoteMMIv2 only.
 */
typedef enum _RemoteMmiPriority {
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
} RemoteMmiPriority;

/**
 * Remote MMI action to use in #Jabra_SetRemoteMmiAction for setting the MMI
 * output LED(s). Only single bit value of RemoteMmiSequence can be specified
 * as parameter RemoteMmiActionOutput to #Jabra_SetRemoteMmiAction.
 * @note RemoteMMIv2 only.
 */
typedef struct _RemoteMmiActionOutput {
  /** Red LED output component to set. */
  uint8_t red;
  /** Green LED output component to set. */
  uint8_t green;
  /** Blue LED output component to set. */
  uint8_t blue;
  /** Output sequence to set. */
  RemoteMmiSequence sequence;
} RemoteMmiActionOutput;

/**
 * Supported remote MMI output LED colours.
 * @note RemoteMMIv2 only.
 */
typedef struct _RemoteMmiOutput {
  /** Red LED supported. */
  bool red;
  /** Green LED supported. */
  bool green;
  /** Blue LED supported. */
  bool blue;
} RemoteMmiOutput;

/**
 * Remote MMI input actions.
 * Remote MMI input, used to identify supported input actions (as a bitmask)
 * and for reporting input events via the RemoteMmiCallback callback
 * (as single bit).
 * @note RemoteMMIv2 only.
 */
typedef enum _RemoteMmiInput {
  MMI_ACTION_NONE          = 0x00,
  MMI_ACTION_UP            = 0x01,
  MMI_ACTION_DOWN          = 0x02,
  MMI_ACTION_TAP           = 0x04,
  MMI_ACTION_DOUBLE_TAP    = 0x08,
  MMI_ACTION_PRESS         = 0x10,
  MMI_ACTION_LONG_PRESS    = 0x20,
  MMI_ACTION_X_LONG_PRESS  = 0x40
} RemoteMmiInput;

/**
 * Remote MMI definitions for the supported MMIs.
 * @note RemoteMMIv2 only.
 */
typedef struct _RemoteMmiDefinition {
  /** Supported type. */
  RemoteMmiType type;
  /** Mask of supported priorities. */
  RemoteMmiPriority priorityMask;
  /** Mask of supported output LED sequences. */
  RemoteMmiSequence sequenceMask;
  /** Mask of supported inputs. */
  RemoteMmiInput inputMask;
  /** Supported output LED colours. */
  RemoteMmiOutput output;
} RemoteMmiDefinition;

typedef struct _PanicListDevType {
  uint8_t panicCode[25];
} Jabra_PanicListDevType;

typedef struct _PanicListType {
  unsigned int entriesNo;
  /** Array with dynamic length 1..x. */
  Jabra_PanicListDevType* panicList;
} Jabra_PanicListType;

/**
 * WizardModes to be used in #Jabra_SetWizardMode and #Jabra_GetWizardMode.
 */
typedef enum _WizardModes {
  FULL_WIZARD     = 0x00,
  LIMITED_WIZARD  = 0x01,
  NO_WIZARD       = 0x02
} WizardModes;

/** @brief Definition of behavior of the internal DeviceCatalogue. */
typedef struct {
  /** Full path of zip file to preload (same as #Jabra_PreloadDeviceInfo, which
   * will be deprecated, may be null. */
  const char* preloadZipFile;
  /** When refreshing data for existing devices, wait this time before going
   * online to reduce the risk of cache locks and reduce the CPU load at
   * startup. Default: 30s. */
  unsigned delayInSecondsBeforeStartingRefresh;
  /** When a device is connected, update device data in the background (using
   * delayInSecondsBeforeStartingRefresh). Default: true. */
  bool refreshAtConnect;
  /** At SDK startup, update data for all previously connected devices in the
   * background (using  delayInSecondsBeforeStartingRefresh). Default: true. */
  bool refreshAtStartup;
  /** When refreshing, what should be in scope:  0: nothing (= block
   * refreshes), 1:all previously connected devices. Default: 1. */
  int refreshScope;
  /** If true: when an unknown device connects, data is updated in the
   * background (ignoring the delay in delayInSecondsBeforeStartingRefresh),
   * and an update notification is delivered to the onDeviceDataUpdated
   * callback. If false: device data is fetched synchronously (as before).
   * Default: false. */
  bool fetchDataForUnknownDevicesInTheBackground;
  /** If not null: called when data for a connected device is (partially or
   * fully) updated. */
  void (*onDeviceDataUpdated)(unsigned short deviceID);
} DeviceCatalogue_params;

/** For use with ConfigParams.cloudConfig_params */
typedef struct _ConfigParams_cloud {
    /** if true, all network access is blocked. */
    bool blockAllNetworkAccess;

    /**
     * @brief optional. The host and path (up to, but excluding the '?') of the
     * source for capability files. The responding host is responsible for
     * parsing the entire URL with parameters delivered and produce a response
     * that is equivalent to the response from the default Jabra endpoints.
     * see https://devicecapabilities.jabra.com/swagger/ui/index (note: v4)
     * Special cases: null or "": use the default.
     */
    const char* baseUrl_capabilities;

    /**
     * @brief optional. The host and partial path of the source for FW files.
     * The responding host is responsible for parsing the entire URL with
     * parameters delivered and produce a response that is equivalent to the
     * response from the Jabra endpoints.
     * example: https://firmware.jabra.com/v4
     * See https://firmware.jabra.com/swagger/#/v3 for full URL and parameters
     * Special cases: null or "": use the default.
     */
    const char* baseUrl_fw;

    /**
     * @brief optional. Specify the proxy to use. NULL or "" if a proxy should
     * not be used. Example: "https://myproxyhost:8042". Supported proxy types,
     * https://curl.haxx.se/libcurl/c/CURLOPT_PROXY.html
     */
    const char* proxy;
} ConfigParams_cloud;

/** Parameters for configuring the SDK at initialization. */
typedef struct _Config_params {
  /** Optional config for the device catalogue. May be null. */
  DeviceCatalogue_params* deviceCatalogue_params;
  /** Optional configuration of cloud access. May be null. */
  ConfigParams_cloud* cloudConfig_params;
  /** For internal Jabra use. */
  void* reserved2;
} Config_params;


/** The connection status of the audio jack connector on the device (not supported by all devices) */
typedef struct _JackStatus{
    bool inserted;
} JackStatus;

/** The link connection status component */
typedef enum _LinkStatusComponent {
    RIGHT_EARBUD = 0,
    LEFT_EARBUD  = 1
} LinkStatusComponent;

/** The connection status of the link e.g. left earbud connected or not (not supported by all devices) */
typedef struct _LinkConnectStatus{
    bool open;
    LinkStatusComponent component;
} LinkConnectStatus;

/** The status of the on-head detection of the device (not supported by all devices) */
typedef struct _HeadDetectionStatus{
    bool leftOn; /** true: left earcup is on head (false: off) */
    bool rightOn; /** true: right earcup is on head (false: off) */
} HeadDetectionStatus;

/** Listener for JackStatus events */
typedef void (*JackConnectorStatusListener)(unsigned short deviceID, const JackStatus status);

/** Listener for HeadDetectionStatus events */
typedef void (*HeadDetectionStatusListener)(unsigned short deviceID, const HeadDetectionStatus status);

/** Listener for link connection status events */
typedef void (*LinkConnectionStatusListener)(unsigned short deviceID, const LinkConnectStatus status);

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief Get the SDK version.
 * @param[in,out] version Pointer to buffer used to hold the SDK version. The
 * buffer must be allocated by the caller prior to calling this function.
 * @param[in,out] count Character count, at time of call count must the size
 * of the version buffer, at return count hols the count of the actual
 * characters written to the version buffer.
 * @return Return_Ok if get version is successful.
 * @return Return_ParameterFail if version is NULL or too small to contain
 * result.
 * @note This function can be called without #Jabra_Initialize being called.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetVersion(char* const version, int count);

/**
 * @brief Sets the Application ID. The application ID must be set before
 * #Jabra_Initialize is called. The application ID can requested via the
 * Jabra Developer Zone https://developer.jabra.com
 * @param[in] inAppID ID for the application.
 */
LIBRARY_API void Jabra_SetAppID(const char* inAppID);

/**
 * @brief Library initialization - only intended to be called once.
 * @param[in] FirstScanForDevicesDoneFunc Callback method, called when USB scan
 * is done. Can be NULL if not used.
 * @param[in] DeviceAttachedFunc Callback method, called when a device is
 * attached. Can be NULL if not used. Callee must call #Jabra_FreeDeviceInfo to
 * free memory.
 * @param[in] DeviceRemovedFunc Callback method, called when a device is
 * removed. Can be NULL if not used.
 * @param[in] ButtonInDataRawHidFunc Callback method, called on new input data.
 * HID Events will (default) NOT be triggered for standard HID to avoid conflicts
 * Raw HID. Low-level. Can be NULL if not used. 
 * @param[in] ButtonInDataTranslatedFunc Callback method, called on new input
 * data. High-level. Can be NULL if not used.
 * @param[in] nonJabraDeviceDectection true non Jabra and Jabra devices will be detected,
 * false only Jabra devices will be detected. Non Jabra device detection is not supported on Linux
 * @param[in] configParams Optional configuration of various SDK library
 * behavior. Can be NULL if not used.
 * @return True if library initialization is successful.
 * @return False if library initialization is not successful. One reason could
 * be that the library is already initialized or that #Jabra_SetAppID has not
 * been called prior to calling this function.
 * @note AppID must be set using #Jabra_SetAppID before the library
 * initialization is called. If not the initialization fails.
 */
LIBRARY_API bool Jabra_InitializeV2(void(*FirstScanForDevicesDoneFunc)(void),
  void(*DeviceAttachedFunc)(Jabra_DeviceInfo deviceInfo),
  void(*DeviceRemovedFunc)(unsigned short deviceID),
  void(*ButtonInDataRawHidFunc)(unsigned short deviceID, unsigned short usagePage, unsigned short usage, bool buttonInData),
  void(*ButtonInDataTranslatedFunc)(unsigned short deviceID, Jabra_HidInput translatedInData, bool buttonInData),
  bool nonJabraDeviceDectection,
  Config_params* configParams
  );

/**
 * @deprecated This API is going to be deprecated, consider using Jabra_InitializeV2 instead
 * @brief Library initialization - only intended to be called once.
 * @param[in] FirstScanForDevicesDoneFunc Callback method, called when USB scan
 * is done. Can be NULL if not used.
 * @param[in] DeviceAttachedFunc Callback method, called when a device is
 * attached. Can be NULL if not used. Callee must call #Jabra_FreeDeviceInfo to
 * free memory.
 * @param[in] DeviceRemovedFunc Callback method, called when a device is
 * removed. Can be NULL if not used.
 * @param[in] ButtonInDataRawHidFunc Callback method, called on new input data.
 * HID Events will (default) NOT be triggered for standard HID to avoid conflicts
 * Raw HID. Low-level. Can be NULL if not used.
 * @param[in] ButtonInDataTranslatedFunc Callback method, called on new input
 * data. High-level. Can be NULL if not used.
 * @param[in] instance Optional instance number. Can be 0 if not used.
 * @param[in] configParams Optional configuration of various SDK library
 * behavior. Can be NULL if not used.
 * @return True if library initialization is successful.
 * @return False if library initialization is not successful. One reason could
 * be that the library is already initialized or that #Jabra_SetAppID has not
 * been called prior to calling this function.
 * @note AppID must be set using #Jabra_SetAppID before the library
 * initialization is called. If not the initialization fails.
 */
LIBRARY_API bool Jabra_Initialize(void(*FirstScanForDevicesDoneFunc)(void),
                                  void(*DeviceAttachedFunc)(Jabra_DeviceInfo deviceInfo),
                                  void(*DeviceRemovedFunc)(unsigned short deviceID),
                                  void(*ButtonInDataRawHidFunc)(unsigned short deviceID, unsigned short usagePage, unsigned short usage, bool buttonInData),
                                  void(*ButtonInDataTranslatedFunc)(unsigned short deviceID, Jabra_HidInput translatedInData, bool buttonInData),
                                  unsigned int instance,
                                  Config_params* configParams
);


/**
 * @brief Library uninitialize.
 * @return True if library uninitialization is successful, false if library
 * initialization is not successful (for example if called when not
 * initialized).
 */
LIBRARY_API bool Jabra_Uninitialize(void);

/**
 * @brief Enable Hid events from non Jabra devices.
 * @param[in] hidEvents true HID events are send to app, false HID events are discarded
 * @return Return_Ok if successful
 * @return Non_Jabra_Device_Detection_disabled if non-jabra device detection is disabled
 * @return System_Error  if device manager instance is not available.
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetHidEventsFromNonJabraDevices(bool hidEvents);

/**
 * @brief Is Hid events from non Jabra devices enabled.
 * @return true HID events are enabled, false HID events are not enabled
 */
LIBRARY_API bool Jabra_IsHidEventsFromNonJabraDevicesEnabled(void);

/**
 * @brief Enable Hid events from Jabra devices.
 * @param[in] hidEvents true HID events are send to app, false HID events are discarded
 */
LIBRARY_API void Jabra_SetStdHidEventsFromJabraDevices(bool hidEvents);

/**
 * @brief Is Hid events from Jabra devices enabled.
 * @return true HID events are enabled, false HID events are not enabled
 */
LIBRARY_API bool Jabra_IsStdHidEventsFromJabraDevicesEnabled(void);

/**
 * @brief Check if device scan is done.
 * @return True if USB device scan is done, false if USB device scan is not
 * done.
 * @note Library initialization must be performed before calling this function.
 */
LIBRARY_API bool Jabra_IsFirstScanForDevicesDone(void);

/**
 * @brief Check if a device is attached.
 * @param[in] deviceID Id for specific device.
 * @return True if device is attached, false if device is not attached.
 */
LIBRARY_API bool Jabra_IsDeviceAttached(unsigned short deviceID);

/**
 * @brief Get information of all attached devices.
 * @param[in] count Pointer to an integer that has the number of
 * #Jabra_DeviceInfo structures allocated before calling this function.
 * On return this pointer has the value of how many devices that was added.
 * @param[in] deviceInfoList Pointer to an array of #Jabra_DeviceInfo to be
 * populated.
 * @note call #Jabra_FreeDeviceInfo on each object in the list when done do
 * avoid a memory leak.
 * @see Jabra_FreeDeviceInfo
 */
LIBRARY_API void Jabra_GetAttachedJabraDevices(int* count, Jabra_DeviceInfo* deviceInfoList);

/**
 * Frees the #Jabra_DeviceInfo structure members.
 * @param[in] info #Jabra_DeviceInfo structure to be freed.
 * @see Jabra_GetAttachedJabraDevices
 */
LIBRARY_API void Jabra_FreeDeviceInfo(Jabra_DeviceInfo info);

/**
* @deprecated This API has been deprecated, consider using Jabra_GetESN instead
 * @brief Get device serial number.
 * @param[in] deviceID ID for a specific device.
 * @param[in] serialNumber Pointer to location where the serial number is
 * written. Must be allocated by the caller.
 * @param[in] count Maximum number of characters to copy to serialNumber.
 * @return Return_Ok if get serial number is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if serialNumber or count is incorrect.
 * @return No_Information if there is no information in the cloud regarding
 * the serial number.
 * @return NetworkRequest_Fail if the server is down or if there is any error
 * during parsing.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetSerialNumber(unsigned short deviceID, char* const serialNumber, int count);

/**
 * @brief Get device ESN (electronic serial number).
 * @param[in] deviceID ID for a specific device.
 * @param[in] esn Pointer to buffer location where the ESN is written. The
 * buffer must be allocated by the caller.
 * @param[in] count Maximum number of characters to copy to serialNumber.
 * @return Return_Ok if get serial number is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if esn or count is incorrect.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetESN( unsigned short deviceID, char* const esn, int count);

/**
 * @brief Get device SKU (stock keeping unit).
 * @param[in] deviceID ID for a specific device.
 * @param[in] sku Pointer to buffer location where the SKU is written. The
 * buffer must be allocated by the caller.
 * @param[in] count Maximum number of characters to copy to sku.
 * @return Return_Ok if get serial number is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if sku or count is incorrect.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetSku(unsigned short deviceID, char* const sku, unsigned int count);

/**
 * @brief Get the hardware and config version of the device
 * @param[in] deviceID ID for a specific device.
 * @param[in] HwVersion Pointer to buffer where hardware version is saved.
 * @param[in] configVersion Pointer to buffer where config version is saved.
 * @return Device_Unknown if the deviceID specified is not known.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetHwAndConfigVersion(unsigned short deviceID, unsigned short *HwVersion, unsigned short *configVersion);

/**
 * @brief Get ESN for all device components. Some devices may be a system of
 * multiple devices, e.g., a TW headset (left earbud, right earbud, cradle).
 * @param[in] deviceID ID for a specific device.
 * @return Pointer to a map with the requested info (null on failure). Caller
 * is responsible for calling #Jabra_FreeMap to release the allocated memory
 * after use.
 * @see Jabra_FreeMap
 */
LIBRARY_API Map_Int_String* Jabra_GetMultiESN( unsigned short deviceID);

/**
 * @brief Release memory allocated by functions returning a Map_Int_String*
 * @param[in] map map to release.
 * @see Jabra_GetMultiESN
 */
LIBRARY_API void Jabra_FreeMap(Map_Int_String* map);

/**
 * @brief Get firmware version of the device.
 * @param[in] deviceID ID for a specific device.
 * @param[in] firmwareVersion Pointer to a location where the firmware version
 * is written. Must be allocated by the caller.
 * @param[in] count Maximum number of characters to copy to firmwareVersion.
 * @param[out] firmwareVersion Firmware version of device.
 * @return Return_Ok if get serial number is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetFirmwareVersion(unsigned short deviceID, char* const firmwareVersion, int count);

/**
 * @brief Get the language code for the current language of the device.
 * @param[in] deviceID ID for a specific device.
 * @param[out] languageCode The code of the current language.
 * @return Return_Ok if is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetCurrentLanguageCode(unsigned short deviceID, unsigned short* languageCode);

/**
 * @brief Gets the device image path.
 * @param[in] deviceID ID for a specific device.
 * @return The path of the device image.
 * @note As memory is allocated through SDK, needs to be freed by calling
 * #Jabra_FreeString.
 */
LIBRARY_API char* Jabra_GetDeviceImagePath(unsigned short deviceID);

/**
 * @brief Gets the device image thumbnail path.
 * @param[in] deviceID ID for a specific device.
 * @return The path of the device image thumbnail.
 * @note As memory is allocated through SDK, need to be freed by calling
 * #Jabra_FreeString.
 */
LIBRARY_API char* Jabra_GetDeviceImageThumbnailPath(unsigned short deviceID);


typedef enum _BatteryComponent {
    UNKNOWN, /*!< Unable to determine the component. Try updating the SDK. */
    MAIN, /*!< Generally applies to headsets with headband that only contains one battery. */
    COMBINED, /*!< For headsets that contains multiple batteries but is not capable of sending each individual state. */
    RIGHT, /*!< The battery in the right unit */ 
    LEFT, /*!< The battery in the left unit */ 
    CRADLE_BATTERY /*!< The battery in the cradle */ 
} BatteryComponent;

typedef struct _BatteryStatusUnitType {
    /*Level in percent*/
    uint8_t levelInPercent;
    /*The component for which the level corresponds to. @see BatteryComponent*/
    BatteryComponent component;
} Jabra_BatteryStatusUnit;

typedef struct _BatteryStatusType {
    /*Level in percent*/
    uint8_t levelInPercent;
    /*Indicates if the battery is charging or not*/
    bool charging;
    /*Indicates if the battery is low. The logic depends on the device and differs*/
    bool batteryLow;
    /*The component for which the level corresponds to. @see BatteryComponent*/
    BatteryComponent component;
    /*Count of extra units*/
    size_t extraUnitsCount;
    /*Contains additional information about other units in the headset - @see Jabra_BatteryStatusUnit.*/
    Jabra_BatteryStatusUnit* extraUnits;
} Jabra_BatteryStatus;

/**
 * @brief Get battery status, if supported by device.
 * @param[in] deviceID ID for a specific device.
 * @param[out] batteryStatus Struct containing battery status.
 * @return Return_Ok if get battery information is returned.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if device does not have battery information.
 * @note Since dongle does not have battery, SDK returns Not_Supported when
 * battery status is requested for dongle device.
 * @note As memory is allocated through SDK, need to be freed by calling
 * #Jabra_FreeBatteryStatus.
 * @see Jabra_RegisterBatteryStatusUpdateCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetBatteryStatusV2(unsigned short deviceID, Jabra_BatteryStatus** batteryStatus);

/**
 * @brief Copy the content of a Jabra_BatteryStatus struct. See @Jabra_BatteryStatus.
 * @param[in] from pointer containing the source.
 * @param[out] dest pointer to copy to. Must be instantiated by the caller.
*/
LIBRARY_API void Jabra_CopyJabraBatteryStatus(const Jabra_BatteryStatus* from, Jabra_BatteryStatus* to);

/**
 * Frees the #Jabra_BatteryStatus
 * @param[in] batteryStatus #Jabra_BatteryStatus structure to be freed.
 */
LIBRARY_API void Jabra_FreeBatteryStatus(Jabra_BatteryStatus* batteryStatus);

/**
 * Type definition of function pointer to use for
 * #Jabra_RegisterBatteryStatusCallback. See @Jabra_BatteryStatus
 */
typedef void(*BatteryStatusUpdateCallbackV2)(unsigned short deviceID, Jabra_BatteryStatus* batteryStatus);

/**
 * @deprecated This API has been deprecated, use #Jabra_GetBatteryStatusV2
 * @brief Get battery status, if supported by device.
 * @param[in] deviceID ID for a specific device.
 * @param[out] levelInPercent Battery level in percent (0 - 100).
 * @param[out] charging Indicates if battery is being charged.
 * @param[out] batteryLow Indicates if battery level is low.
 * @return Return_Ok if get battery information is returned.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if device does not have battery information.
 * @note Since dongle does not have battery, SDK returns Not_Supported when
 * battery status is requested for dongle device.
 * @see Jabra_RegisterBatteryStatusUpdateCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetBatteryStatus(unsigned short deviceID, int *levelInPercent, bool *charging, bool *batteryLow);

/**
 * Type definition of function pointer to use for
 * #Jabra_RegisterBatteryStatusCallback.
 */
typedef void(*BatteryStatusUpdateCallback)(unsigned short deviceID, int levelInPercent, bool charging, bool batteryLow);

/**
 * @brief Register for battery status update callback.
 * @param[in] callback Callback method called when the battery status changes.
 * @see Jabra_GetBatteryStatus
 * @note As memory is allocated through SDK, need to be freed by calling #Jabra_FreeBatteryStatus.
 */
LIBRARY_API void Jabra_RegisterBatteryStatusUpdateCallbackV2(BatteryStatusUpdateCallbackV2 const callback);

/**
 * @deprecated This API has been deprecated, use #Jabra_RegisterBatteryStatusUpdateCallbackV2
 * @brief Register for battery status update callback.
 * @param[in] callback Callback method called when the battery status changes.
 * @see Jabra_GetBatteryStatus
 */
LIBRARY_API void Jabra_RegisterBatteryStatusUpdateCallback(BatteryStatusUpdateCallback const callback);

/**
 * @deprecated This API has been deprecated.
 * @brief Get the warranty end date of the device.
 * @param[in] deviceID ID for a device.
 * @return Warranty end date of the device if warranty for the device is not
 * yet expired. If device is NOT in warranty, it returns a nullptr.
 * @note As memory is allocated through SDK for returned warranty end date,
 * needs to be freed by calling #Jabra_FreeString.
 */
LIBRARY_API char* Jabra_GetWarrantyEndDate(unsigned short deviceID);

/**
 * @brief Integrates softphone app to Jabra applications like Jabra Direct(JD)
 * and Jabra Suite for Mac(JMS).
 * @param[in] guid Client unique ID.
 * @param[in] softphoneName Name of the application to be shown in JD or JMS.
 * @return True if softphone app integrates to Jabra application, false if it fails or already connected
 * otherwise.
 * @see Jabra_DisconnectFromJabraApplication
 */
LIBRARY_API bool Jabra_ConnectToJabraApplication(const char* guid, const char* softphoneName);

/**
 * @brief Disconnects connected from Jabra applications.
 * @see Jabra_ConnectToJabraApplication
 */
LIBRARY_API void Jabra_DisconnectFromJabraApplication(void);

/**
 * @brief Sets the softphone to Ready. Currently applicable for only Jabra
 * Direct.
 * Will be available in later versions of JMS.
 * @param[in] isReady Sets the softphone readiness state.
 * @see Jabra_IsSoftphoneInFocus
 */
LIBRARY_API void Jabra_SetSoftphoneReady(bool isReady);

/**
 * @brief Indicates whether the softphone is in focus.
 * @return True if softphone is in focus, false otherwise.
 * @see Jabra_SetSoftphoneReady
 */
LIBRARY_API bool Jabra_IsSoftphoneInFocus(void);

/**
 * @brief Set the Bluetooth device in pairing mode.
 * @param[in] deviceID ID for a BT adapter.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_SearchNewDevices
 * @see Jabra_GetSearchDeviceList
 * @see Jabra_StopBTPairing
 * @see Jabra_SetAutoPairing
 * @see Jabra_GetAutoPairing
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetBTPairing(unsigned short deviceID);

/**
 * @brief Search for available Bluetooth devices which are switched on, within
 * range and ready to connect.
 * @param[in] deviceID ID for the BT adapter.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_SetBTPairing
 * @see Jabra_GetSearchDeviceList
 * @see Jabra_StopBTPairing
 * @see Jabra_SetAutoPairing
 * @see Jabra_GetAutoPairing
 */
LIBRARY_API Jabra_ReturnCode Jabra_SearchNewDevices(unsigned short deviceID);

/**
 * @brief Gets the list of new devices which are available to pair & connect.
 * @param[in] deviceID ID for specific device.
 * @return Returns pointer to the structure #Jabra_PairingList contains
 * available devices ready to pair.
 * @note As memory is allocated through SDK, it must be freed by calling
 * #Jabra_FreePairingList.
 * @see Jabra_SetBTPairing
 * @see Jabra_SearchNewDevices
 * @see Jabra_StopBTPairing
 * @see Jabra_SetAutoPairing
 * @see Jabra_GetAutoPairing
 */
LIBRARY_API Jabra_PairingList* Jabra_GetSearchDeviceList(unsigned short deviceID);

/**
 * @brief Stop search for available Bluetooth devices.
 * @param[in] deviceID ID for a BT adapter.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_SetBTPairing
 * @see Jabra_SearchNewDevices
 * @see Jabra_GetSearchDeviceList
 * @see Jabra_SetAutoPairing
 * @see Jabra_GetAutoPairing
 */
LIBRARY_API Jabra_ReturnCode Jabra_StopBTPairing(unsigned short deviceID);

/**
 * @brief When Bluetooth adapter is plugged into the PC it will attempt to
 * connect with the last connected Bluetooth device. If it cannot connect,
 * it will automatically search for new Bluetooth devices to connect to.
 * @param[in] deviceID ID for a BT adapter.
 * @param[in] value Enable or disable for auto pairing.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_SetBTPairing
 * @see Jabra_SearchNewDevices
 * @see Jabra_GetSearchDeviceList
 * @see Jabra_StopBTPairing
 * @see Jabra_GetAutoPairing
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetAutoPairing(unsigned short deviceID, bool value);

/**
 * @brief Get Auto pairing mode enable or disable.
 * @param[in] deviceID ID for a BT adapter.
 * @return True if auto pairing mode is enabled, false otherwise.
 * @see Jabra_SetBTPairing
 * @see Jabra_SearchNewDevices
 * @see Jabra_GetSearchDeviceList
 * @see Jabra_StopBTPairing
 * @see Jabra_SetAutoPairing
 */
LIBRARY_API bool Jabra_GetAutoPairing(unsigned short deviceID);

/**
 * @brief Connect/Reconnect Bluetooth device to the Jabra Bluetooth adapter.
 * Ensure the Bluetooth device is switched on and within range.
 * @param[in] deviceID ID for a BT adapter.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_ConnectNewDevice
 * @see Jabra_DisconnectBTDevice
 * @see Jabra_ConnectPairedDevice
 * @see Jabra_DisConnectPairedDevice
 * @see Jabra_GetConnectedBTDeviceName
 */
LIBRARY_API Jabra_ReturnCode Jabra_ConnectBTDevice(unsigned short deviceID);

/**
 * @brief Connect a new device.
 * @param[in] deviceID ID for specific device.
 * @param[in] device pointer to structure #Jabra_PairedDevice.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_AlreadyConnected if the device is already connected.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_ConnectBTDevice
 * @see Jabra_DisconnectBTDevice
 * @see Jabra_ConnectPairedDevice
 * @see Jabra_DisConnectPairedDevice
 * @see Jabra_GetConnectedBTDeviceName
 */
LIBRARY_API Jabra_ReturnCode Jabra_ConnectNewDevice(unsigned short deviceID, Jabra_PairedDevice* device);

/**
 * @brief Disconnect Bluetooth device from Bluetooth adapter.
 * @param[in] deviceID ID for a BT adapter.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_ConnectBTDevice
 * @see Jabra_ConnectNewDevice
 * @see Jabra_ConnectPairedDevice
 * @see Jabra_DisConnectPairedDevice
 * @see Jabra_GetConnectedBTDeviceName
 */
LIBRARY_API Jabra_ReturnCode Jabra_DisconnectBTDevice(unsigned short deviceID);

/**
 * @brief Connect a device which was already paired.
 * @param[in] deviceID ID for specific device.
 * @param[in] device Pointer to structure #Jabra_PairingList.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_AlreadyConnected if a device is already connected.
 * @return Device_WriteFail if it fails to write the value to the device.
 * @note After device connection, #Jabra_GetPairingList api has to be called
 * to get updated connection status. In order to connect to a device from the
 * list of paired devices, make sure that there is no paired device currently
 * connected. Any paired device currently connected has to be disconnected by
 * calling #Jabra_DisconnectPairedDevice before using
 * #Jabra_ConnectPairedDevice.
 * @see Jabra_ConnectBTDevice
 * @see Jabra_ConnectNewDevice
 * @see Jabra_DisconnectBTDevice
 * @see Jabra_DisConnectPairedDevice
 * @see Jabra_GetConnectedBTDeviceName
*/
LIBRARY_API Jabra_ReturnCode Jabra_ConnectPairedDevice(unsigned short deviceID, Jabra_PairedDevice* device);

/**
 * @brief Disconnect a paired device.
 * @param[in] deviceID ID for specific device.
 * @param[in] device pointer to structure #Jabra_PairingList.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_NotConnected if the device is not connected.
 * @return Device_WriteFail if it fails to write the value to the device.
 * @note After device disconnection, #Jabra_GetPairingList api has to be called
 * to get updated connection status.
 * @see Jabra_ConnectBTDevice
 * @see Jabra_ConnectNewDevice
 * @see Jabra_DisconnectBTDevice
 * @see Jabra_ConnectPairedDevice
 * @see Jabra_GetConnectedBTDeviceName
 */
LIBRARY_API Jabra_ReturnCode Jabra_DisConnectPairedDevice(unsigned short deviceID, Jabra_PairedDevice* device);

/**
 * @brief Get name of connected BT device with BT Adapter.
 * @param[in] deviceID ID for a BT adapter.
 * @return Name of connected BT device if successful, otherwise return NULL.
 * @note As memory is allocated through SDK, it must be freed by calling
 * #Jabra_FreeString.
 * @see Jabra_ConnectBTDevice
 * @see Jabra_ConnectNewDevice
 * @see Jabra_DisconnectBTDevice
 * @see Jabra_ConnectPairedDevice
 * @see Jabra_DisConnectPairedDevice
 */
LIBRARY_API char* Jabra_GetConnectedBTDeviceName(unsigned short deviceID);

/**
 * @brief Checks if pairing list is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if pairing list is supported, false if device does not support
 * pairing list.
 * @see Jabra_GetPairingList
 * @see Jabra_ClearPairingList
 * @see Jabra_FreePairingList
 * @see Jabra_ClearPairedDevice
 * @see Jabra_RegisterPairingListCallback
 */
LIBRARY_API bool Jabra_IsPairingListSupported(unsigned short deviceID);

/**
 * @brief Gets the secure connection mode. The interface is only valid to use for a dongle
 * @param[in] deviceID ID for a device.
 * @param[out] Secure connection mode.
 * @return Return_Ok if success.
 * @return Not_Supported if not supported by the device (all non dongle devices will return this)
 * @return Return_ParameterFail if scMode is a null pointer
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_ReadFails if not able to read the mode
 * @see Jabra_GetPairingList
 * @see Jabra_ClearPairingList
 * @see Jabra_FreePairingList
 * @see Jabra_ClearPairedDevice
 * @see Jabra_RegisterPairingListCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetSecureConnectionMode(unsigned short deviceID, Jabra_SecureConnectionMode *scMode);

/**
 * @brief Gets the list of devices which are paired previously.
 * @param[in] deviceID ID for specific device.
 * @return Returns pointer to the structure #Jabra_PairingList contains all
 * paired device details.
 * @note As memory is allocated through SDK, it must be freed by calling
 * #Jabra_FreePairingList.
 * @see Jabra_IsPairingListSupported
 * @see Jabra_ClearPairingList
 * @see Jabra_FreePairingList
 * @see Jabra_ClearPairedDevice
 * @see Jabra_RegisterPairingListCallback
 */
LIBRARY_API Jabra_PairingList* Jabra_GetPairingList(unsigned short deviceID);

/**
 * @brief Clear list of paired BT devices from BT adaptor.
 * @param[in] deviceID ID for a BT adapter.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_WriteFail if it fails to write to the device.
 * @see Jabra_IsPairingListSupported
 * @see Jabra_GetPairingList
 * @see Jabra_FreePairingList
 * @see Jabra_ClearPairedDevice
 * @see Jabra_RegisterPairingListCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_ClearPairingList(unsigned short deviceID);

/**
 * @brief Frees the memory allocated for the list of paired devices.
 * @param[in] deviceList #Jabra_PairingList structure pointer, which needs to
 * be freed.
 * @see Jabra_IsPairingListSupported
 * @see Jabra_GetPairingList
 * @see Jabra_ClearPairingList
 * @see Jabra_ClearPairedDevice
 * @see Jabra_RegisterPairingListCallback
 */
LIBRARY_API void Jabra_FreePairingList(Jabra_PairingList* deviceList);

/**
 * @brief Clear devices from the paired device list.
 * @param[in] deviceID ID for specific device.
 * @param[in] device Pointer to structure #Jabra_PairingList.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return CannotClear_DeviceConnected if the device has an active connection.
 * @return Device_WriteFail if it fails to write the value to the device.
 * @see Jabra_IsPairingListSupported
 * @see Jabra_GetPairingList
 * @see Jabra_ClearPairingList
 * @see Jabra_FreePairingList
 * @see Jabra_RegisterPairingListCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_ClearPairedDevice(unsigned short deviceID, Jabra_PairedDevice* device);

/**
 * @brief Register callback for Pairing list.
 * @param[in] PairingList Callback method, will be called when paired devices
 * changed event received from device.
 * @see Jabra_IsPairingListSupported
 * @see Jabra_GetPairingList
 * @see Jabra_ClearPairingList
 * @see Jabra_FreePairingList
 * @see Jabra_ClearPairedDevice
 */
LIBRARY_API void Jabra_RegisterPairingListCallback(void(*PairingList)(unsigned short deviceID, Jabra_PairingList *lst));

/**
 * @brief Get error string from the error status.
 * @param[in] errStatus Status of the error from the Jabra Device.
 * @return Corresponding error text.
 */
LIBRARY_API const char* Jabra_GetErrorString(Jabra_ErrorStatus errStatus);

/**
 * @brief Get descriptive string from the return code.
 * @param[in] code Return code to get description of.
 * @return Corresponding text.
 */
LIBRARY_API const char* Jabra_GetReturnCodeString(Jabra_ReturnCode code);

/**
 * @brief Checks if busylight is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if busylight is supported, false if device does not support
 * busylight.
 * @see Jabra_GetBusylightStatus
 * @see Jabra_SetBusylightStatus
 * @see Jabra_RegisterBusylightEvent
 */
LIBRARY_API bool Jabra_IsBusylightSupported(unsigned short deviceID);

/**
 * @brief Checks the status of busylight.
 * @param[in] deviceID ID for a device.
 * @return True if busylight is on, false if busylight is off or if it is not
 * supported.
 * @see Jabra_IsBusylightSupported
 * @see Jabra_SetBusylightStatus
 * @see Jabra_RegisterBusylightEvent
 */
LIBRARY_API bool Jabra_GetBusylightStatus(unsigned short deviceID);

/**
 * @brief Enable/Disable the busylight status.
 * @param[in] deviceID ID for a device.
 * @param[in] value enable or disable busylight.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if the functionality is not supported.
 * @see Jabra_IsBusylightSupported
 * @see Jabra_GetBusylightStatus
 * @see Jabra_RegisterBusylightEvent
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetBusylightStatus(unsigned short deviceID, bool value);

/**
 * @brief Registration for busylight event.
 * @param[in] BusylightFunc Callback method, called when busylight event is
 * received from device. Can be NULL if not used.
 * @see Jabra_IsBusylightSupported
 * @see Jabra_GetBusylightStatus
 * @see Jabra_SetBusylightStatus
 */
LIBRARY_API void Jabra_RegisterBusylightEvent(void(*BusylightFunc)(unsigned short deviceID, bool busylightValue));

/**
 * @brief Is left earbud status supported.
 * @param[in] deviceID ID for a device.
 * @return True left earbud status is supported, false if device does not
 * support left earbud status.
 * @see Jabra_GetLeftEarbudStatus
 * @see Jabra_RegisterLeftEarbudStatus
 * @see Jabra_RegisterHearThroughSettingChangeHandler
 */
LIBRARY_API bool Jabra_IsLeftEarbudStatusSupported(unsigned short deviceID);

/**
 * @brief Get left earbud connection status.
 * @param[in] deviceID ID for a device.
 * @return True if left earbud is connected, false if left earbud is not
 * connected.
 * @see Jabra_IsLeftEarbudStatusSupported
 * @see Jabra_RegisterLeftEarbudStatus
 * @see Jabra_RegisterHearThroughSettingChangeHandler
 */
LIBRARY_API bool Jabra_GetLeftEarbudStatus(unsigned short deviceID);

/**
 * @brief Registration for left earbud connection status event. Can only be
 * called when a device is attached.
 * @param[in] deviceID ID for a device.
 * @param[in] LeftEarbudFunc Callback method, called when left earbud status
 * event is received from device. Can be NULL if not used.
 * @return Return_Ok if success otherwise error code.
 * @see Jabra_IsLeftEarbudStatusSupported
 * @see Jabra_GetLeftEarbudStatus
 * @see Jabra_RegisterHearThroughSettingChangeHandler
 */
LIBRARY_API Jabra_ReturnCode Jabra_RegisterLeftEarbudStatus(unsigned short deviceID, void(*LeftEarbudFunc)(unsigned short deviceID, bool connected));

/**
 * @brief Registration for HearThrough setting change event.
 * @param[in] HearThroughSettingChangeFunc Callback method, called when
 * HearThrough setting is changed on device. Can be NULL if not used.
 * @see Jabra_IsLeftEarbudStatusSupported
 * @see Jabra_GetLeftEarbudStatus
 * @see Jabra_RegisterLeftEarbudStatus
 */
LIBRARY_API void Jabra_RegisterHearThroughSettingChangeHandler(void(*HearThroughSettingChangeFunc)(unsigned short deviceID, bool enabled));

/**
 * @brief Checks if equalizer is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if equalizer is supported, false if device does not support
 * equalizer.
 * @see Jabra_IsEqualizerEnabled
 * @see Jabra_EnableEqualizer
 * @see Jabra_GetEqualizerParameters
 * @see Jabra_SetEqualizerParameters
 */
LIBRARY_API bool Jabra_IsEqualizerSupported(unsigned short deviceID);

/**
 * @brief Checks if equalizer is enabled.
 * @param[in] deviceID ID for a device.
 * @return True if equalizer is enabled, false if equalizer is disabled or not
 * supported.
 * @see Jabra_IsEqualizerSupported
 * @see Jabra_EnableEqualizer
 * @see Jabra_GetEqualizerParameters
 * @see Jabra_SetEqualizerParameters
*/
LIBRARY_API bool Jabra_IsEqualizerEnabled(unsigned short deviceID);

/**
 * @brief Enable/disable equalizer.
 * @param[in] deviceID ID for a device.
 * @param[in] value Enable or disable equalizer.
 * @return Return_Ok if success otherwise error code.
 * @see Jabra_IsEqualizerSupported
 * @see Jabra_IsEqualizerEnabled
 * @see Jabra_GetEqualizerParameters
 * @see Jabra_SetEqualizerParameters
 */
LIBRARY_API Jabra_ReturnCode Jabra_EnableEqualizer(unsigned short deviceID, bool value);

/**
 * @brief Get equalizer parameters.
 * @param[in] deviceID ID for a device.
 * @param[in/out] bands Caller allocated / owned array for the parameters. Allocate at least the number of bands you expect the device to have (could vary, but 5 is a good bet). On input: empty, on successful return: holds the equalizer parameters.
 * @param[in/out] nbands in: The size of the bands array, out (if successful): the actual number of bands. bands[0..*nbands-1] are then valid.
 * @return Return_Ok if success.
 * @return Not_Supported if equalizer is not supported.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_ReadFails
 * @return Return_ParameterFail if bands or nbands is incorrect. Typically seen if nbands is too small on input (i.e., the buffer is not large enough)
 * @see Jabra_IsEqualizerSupported
 * @see Jabra_IsEqualizerEnabled
 * @see Jabra_EnableEqualizer
 * @see Jabra_SetEqualizerParameters
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetEqualizerParameters(unsigned short deviceID, Jabra_EqualizerBand * bands, unsigned int * nbands);

/**
 * @brief Set equalizer parameters.
 * @param[in] deviceID ID for a device.
 * @param[in] bands Caller-owned array containing the band gains to set in dB
 * (must be within range of +/- #Jabra_EqualizerBand.max_gain).
 * @param[in] nbands Number of bands to set. Use the nbands value obtained from a successful call to Jabra_GetEqualizerParameters()
 * @return Return_Ok if success.
 * @return Not_Supported if equalizer is not supported.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if bands or nbands is incorrect.
 * @see Jabra_IsEqualizerSupported
 * @see Jabra_IsEqualizerEnabled
 * @see Jabra_EnableEqualizer
 * @see Jabra_GetEqualizerParameters
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetEqualizerParameters(unsigned short deviceID, float * bands, unsigned int nbands);

/**
 * @deprecated This API has been deprecated, use #Jabra_IsFeatureSupported with
 * parameter #DeviceFeature.RemoteMMI instead.
 * @brief Checks if remote MMI feature is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if remote MMI feature is supported, false if device does not
 * support remote MMI feature.
 */
LIBRARY_API bool Jabra_IsRemoteMMISupported(unsigned short deviceID);

/**
 * @brief Configures the remote MMI events for a device.
 * @param[in] deviceID ID for a specific device.
 * @param[in] buttonEvent Button events to be set in device.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if buttonEvent is incorrect.
 * @return Device_WriteFail if it fails to write the value to the device.
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetButtonFocus(unsigned short deviceID, ButtonEvent *buttonEvent);

/**
 * @brief Releases the remote MMI events configured in the device.
 * @param[in] deviceID ID for a specific device.
 * @param[in] buttonEvent Button events to be released in device.
 * @return Return_Ok if setting is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if buttonEvent is incorrect.
 * @return Device_WriteFail if it fails to write the value to the device.
 */
LIBRARY_API Jabra_ReturnCode Jabra_ReleaseButtonFocus(unsigned short deviceID, ButtonEvent *buttonEvent);

/**
 * @brief Gets the supported remote MMI for a device.
 * @param[in] deviceID ID for specific device.
 * @return Pointer to the structure ButtonEvent containing all button events
 * for that device. In case of error a NULL pointer is returned.
 */
LIBRARY_API ButtonEvent* Jabra_GetSupportedButtonEvents(unsigned short deviceID);

/**
 * @brief Free the memory allocated for the button events.
 * @param[in] eventsSupported Pointer to the structure #ButtonEvent to be
 * freed.
 */
LIBRARY_API void Jabra_FreeButtonEvents(ButtonEvent *eventsSupported);

/**
 * @brief Registration for GNP button events i.e remote MMI.
 * @param[in] ButtonGNPEventFunc Callback method to receive GNP Button events
 * /remote MMI events. Can be NULL if not used.
 */
LIBRARY_API void Jabra_RegisterForGNPButtonEvent(void(*ButtonGNPEventFunc)(unsigned short deviceID, ButtonEvent *buttonEvent));

/**
 * @brief Checks if setting protection is enabled.
 * @param[in] deviceID ID for a device.
 * @return True if setting protection is enabled otherwise false.
 */
LIBRARY_API bool Jabra_IsSettingProtectionEnabled(unsigned short deviceID);


/**
 * @brief Get the URL for contacting Jabra customer support.
 * @param[in] appName Name of the application.
 * @param[in] appVersion Version of the application.
 * @param[in] deviceBrand Name of the phone vendor (e.g., "Apple"). May be empty or null.
 * @param[in] deviceModel Device model name (e.g. "IPhone 8"). May be empty or null.
 * @return NPS URL or null pointer if it is not available. If no customer support is available this way for the device, nullptr is returned.
 * @note As memory is allocated through SDK for the Url, it needs to be freed
 * by calling #Jabra_FreeString.
 */
LIBRARY_API char* Jabra_GetCustomerSupportUrl(unsigned short deviceID, const char* appName, const char* appVersion, const char* deviceBrand, const char* deviceModel );


/**
 * @brief Get the NPS URL for the application only.
 * @param[in] appName Name of the application.
 * @param[in] appVersion Version of the application.
 * @return NPS URL or null pointer if it is not available.
 * @note As memory is allocated through SDK for NPS Url, it needs to be freed
 * by calling #Jabra_FreeString.
 */
LIBRARY_API char* Jabra_GetNpsUrlForApplication(const char* appName, const char* appVersion);

/**
 * @brief Get the NPS URL.
 * @param[in] deviceID ID for a device.
 * @param[in] appName Name of the app.
 * @param[in] appVersion Version of the app.
 * @return NPS URL or null pointer if it is not available.
 * @note As memory is allocated through SDK for NPS Url, it needs to be freed
 * by calling #Jabra_FreeString.
 */
LIBRARY_API char* Jabra_GetNpsUrl(unsigned short deviceID, const char* appName, const char* appVersion);

/**
 * @brief Register a product.
 * @param[in] deviceID ID of device from which settings needs to be updated.
 * @param[in] prodReg Product registration information.
 * @return Return_Ok if registration was successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return NetworkRequest_Fail if there is network issue.
 * @return Return_ParameterFail if prodReg is incorrect.
 */
LIBRARY_API Jabra_ReturnCode Jabra_ProductRegistration(unsigned short deviceID, const ProductRegInfo* prodReg);

/**
 * @brief Tells the device to execute a AVRCP command. This interface is only supported by iOS.
 * @param[in] deviceID ID for a specific device.
 * @param[in] command The command to execute.
 * @return Return_Ok if setting is successful.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_WriteFail if it fails to write to the device.
 */
LIBRARY_API Jabra_ReturnCode Jabra_ExecuteAVRCPCommand(unsigned short deviceID, AVRCPCommand command);

/**
 * @brief To configure logging of device events.
 * @param[in] logFlag Specifies the location of logs saved. Local logs are
 * saved only on local machine. Cloud logs are saved in cloud. All logs are
 * saved in both local and cloud.
 * @param[in] flag Enable or disable logging for the specified location.
 * @see Jabra_RegisterDevLogCallback
 * @see Jabra_EnableDevLog
 * @see Jabra_IsDevLogEnabled
 */
LIBRARY_API void Jabra_ConfigureLogging(Jabra_Logging logFlag, bool flag);

/**
 * @deprecated This API has been deprecated, use #Jabra_RegisterDevLogCallback
 * instead to register callback for Logging.
 * @param[in] LogDeviceEvent Callback method, will be called when events
 * are received from the device.
 * eventStr: Is a Json message for the event to be logged of below format.
 *            {
 *                "AppID" : "",
 *                "Device Name" : "",
 *                "ESN" : "",
 *                "EventName" : "",
 *                "FW" : "",
 *                "LocalTimeStamp" : "",
 *                "Pid" : ,
 *                "Seq.No" : ,
 *                "Value" : "",
 *                "Variant" : ""
 *            }
 * @note As memory is allocated through SDK, it must be freed by calling
 * #Jabra_FreeString.
 * @see Jabra_ConfigureLogging
 * @see Jabra_RegisterDevLogCallback
 * @see Jabra_EnableDevLog
 * @see Jabra_IsDevLogEnabled
 */
LIBRARY_API void Jabra_RegisterLoggingCallback(void(*LogDeviceEvent)(char* eventStr));

/**
 * @brief Registration for device logging events.
 * @param[in] LogDeviceEvent Callback method, will be called when log events
 * are received from the device.
 * eventStr: Is a Json message for the event to be logged of below format.
 *            {
 *                "AppID" : "",
 *                "Device Name" : "",
 *                "ESN" : "",
 *                "EventName" : "",
 *                "FW" : "",
 *                "LocalTimeStamp" : "",
 *                "Pid" : ,
 *                "Seq.No" : ,
 *                "Value" : "",
 *                "Variant" : ""
 *            }
 * @note As memory is allocated through SDK, it must be freed by calling
 * #Jabra_FreeString.
 * @see Jabra_ConfigureLogging
 * @see Jabra_EnableDevLog
 * @see Jabra_IsDevLogEnabled
 */
LIBRARY_API void Jabra_RegisterDevLogCallback(void(*LogDeviceEvent)(unsigned short deviceID, char* eventStr));

/**
 * @brief Enable/disable logging for a device.
 * @param[in] deviceID Device ID.
 * @param[in] enable True enable logging in device otherwise disable.
 * eventStr Is a Json message for the event to be logged of below format.
 *          {
 *              "AppID" : "",
 *              "Device Name" : "",
 *              "ESN" : "",
 *              "EventName" : "",
 *              "FW" : "",
 *              "LocalTimeStamp" : "",
 *              "Pid" : ,
 *              "Seq.No" : ,
 *              "Value" : "",
 *              "Variant" : ""
 *          }
 * @return Return_Ok successfully updated firmware or error code.
 * @see Jabra_ConfigureLogging
 * @see Jabra_RegisterDevLogCallback
 * @see Jabra_IsDevLogEnabled
 */
LIBRARY_API Jabra_ReturnCode Jabra_EnableDevLog(unsigned short deviceID, bool enable);

/**
 * @brief Is logging enabled on device.
 * @param[in] deviceID ID for a device.
 * @return True if logging is enabled otherwise false.
 * @see Jabra_ConfigureLogging
 * @see Jabra_RegisterDevLogCallback
 * @see Jabra_EnableDevLog
 */
LIBRARY_API bool Jabra_IsDevLogEnabled(unsigned short deviceID);

/**
 * @brief Checks if firmware lock is enabled. If the firmware lock is enabled
 * it is not possible to upgrade nor downgrade the firmware. In this situation
 * the firmware can only be changed to the same version e.g. if you want to
 * change the language.
 * @param[in] deviceID ID for a device.
 * @return True if firmware lock is enabled otherwise false.
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API bool Jabra_IsFirmwareLockEnabled(unsigned short deviceID);

/**
 * @brief Enable or disable firmware up-and-downgrade lock.
 * @param[in] deviceID ID for a device.
 * @param[in] enable set to true to enable or false to disable the lock.
 * @return Device_Unknown if the device is unknown.
 * @return Not_Supported if firmware lock is not supported for this device.
 * @return Return_Ok if success.
 * @see Jabra_IsFirmwareLockEnabled
 */
LIBRARY_API Jabra_ReturnCode Jabra_EnableFirmwareLock(unsigned short deviceID, bool enable);


/**
 * @brief Check if Firmware update available for device.
 * @param[in] deviceID ID for specific device.
 * @param[in] authorizationId Authorization Id.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if authorizationId is a null pointer
 * or the request to the server is invalid.
 * @return Firmware_UpToDate if device has latest firmware version.
 * @return Firmware_Available if new firmware version is available for the
 * device.
 * @return No_Information if firmware file is not available.
 * @return NetworkRequest_Fail if request to the server fails.
 * @return Invalid_Authorization if authorization is invalid.
 * @return Not_Supported if check is not supported for given device.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_ReturnCode Jabra_CheckForFirmwareUpdate(unsigned short deviceID, const char* authorizationId);

/**
 * @brief Gets details of the latest firmware present in cloud.
 * @param[in] deviceID ID for specific device.
 * @param[in] authorizationId Authorization Id.
 * @return #Jabra_FirmwareInfo structure pointer for details of the latest
 * firmware, nullptr if the device is unknown, authorizationId is a nullptr
 * or there was a request error.
 * @note As memory is allocated through SDK, memory needs to be freed by
 * calling #Jabra_FreeFirmwareInfo.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_FirmwareInfo* Jabra_GetLatestFirmwareInformation(unsigned short deviceID, const char* authorizationId);

/**
 * @brief Frees the firmware information structure members.
 * @param[in] firmwareInfo #Jabra_FirmwareInfo structure to be freed.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API void Jabra_FreeFirmwareInfo(Jabra_FirmwareInfo* firmwareInfo);

/**
 * @brief Get the file path of the downloaded file.
 * @param[in] deviceID ID for specific device.
 * @param[in] version Version for which the path is required.
 * @return firmwareFilePath firmware file path of the device, nullptr
 * if the device is unknown or version is a nullptr.
 * @note Call #Jabra_DownloadFirmware first to ensure that data is current
 * @note As memory is allocated through SDK for firmwareFilePath, it must be
 * freed by calling #Jabra_FreeString.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API char* Jabra_GetFirmwareFilePath(unsigned short deviceID, const char* version);

/**
 * @brief Gets information of all the firmware present in cloud for the
 * specific device.
 * @param[in] deviceID ID for the specific device.
 * @param[in] authorizationId Authorization ID.
 * @return A list of information about all firmware versions. If no information
 * is available, the device is unknown or authorizationId is a null pointer, nullptr
 * is returned.
 * @note The list must be freed by calling #Jabra_FreeFirmwareInfoList.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_FirmwareInfoList* Jabra_GetAllFirmwareInformation(unsigned short deviceID, const char* authorizationId);

/**
 * @brief Frees the list of firmware information structure.
 * @param[in] firmwareInfolist #Jabra_FirmwareInfoList structure to be freed.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API void Jabra_FreeFirmwareInfoList(Jabra_FirmwareInfoList* firmwareInfolist);

/**
 * @brief Downloads the specified firmware version file.
 * @param[in] deviceID ID for specific device.
 * @param[in] version Version for which file download needs to be initiated.
 * @param[in] authorizationId Authorization Id.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_Invalid if deviceID is wrong.
 * @return Return_ParameterFail if version or authorizationId is incorrect.
 * @return Return_Async if firmware download is in progress and will be
 * returned asynchronously using the callback.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_ReturnCode Jabra_DownloadFirmware(unsigned short deviceID, const char* version, const char* authorizationId);

/**
 * @brief Downloads the latest FW updater relevant for this device.
 * @param[in] deviceID ID for specific device.
 * @param[in] authorizationId Authorization Id.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_Invalid if deviceID is wrong.
 * @return Return_ParameterFail if authorizationId is incorrect.
 * @return Return_Async if firmware download is in progress and will be
 * returned asynchronously using the callback.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_ReturnCode Jabra_DownloadFirmwareUpdater(unsigned short deviceID, const char* authorizationId);

/**
 * @brief Upgrades/Updates the firmware for the target device with specified
 * version.
 * @param[in] deviceID ID for specific device.
 * @param[in] filepath Firmware file path.
 * @return Return_Ok Successfully updated firmware.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_Invalid if deviceID is wrong.
 * @return File_Not_Accessible if firmware file path is incorrect.
 * @return FWU_Application_Not_Available if firmware updater folder/application
 * is not found.
 * @return Return_ParameterFail if filepath is incorrect.
 * @return Return_Async firmware update is in progress. Use FWU progress
 * callback to determine when update is done.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_CancelFirmwareDownload
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_ReturnCode Jabra_UpdateFirmware(unsigned short deviceID, const char* filepath);

/**
 * @brief Cancels the firmware download.
 * @param[in] deviceID ID for specific device.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_Invalid if deviceID is wrong.
 * @return Return_Ok cancels firmware download.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_RegisterFirmwareProgressCallBack
 */
LIBRARY_API Jabra_ReturnCode Jabra_CancelFirmwareDownload(unsigned short deviceID);

/**
 * @brief Type definition of function pointer to use for
 * #Jabra_RegisterFirmwareProgressCallBack.
 */
typedef void(*FirmwareProgress)(unsigned short deviceID, Jabra_FirmwareEventType type, Jabra_FirmwareEventStatus status, unsigned short percentage);

/**
 * @brief Registration for firmware progress event.
 * @param[in] callback Callback method, called when firmwareProgress event is
 * received from device. Can be NULL if not used.
 * @see Jabra_IsFirmwareLockEnabled
 * @see Jabra_CheckForFirmwareUpdate
 * @see Jabra_GetLatestFirmwareInformation
 * @see Jabra_FreeFirmwareInfo
 * @see Jabra_GetFirmwareFilePath
 * @see Jabra_GetAllFirmwareInformation
 * @see Jabra_FreeFirmwareInfoList
 * @see Jabra_DownloadFirmware
 * @see Jabra_DownloadFirmwareUpdater
 * @see Jabra_UpdateFirmware
 * @see Jabra_CancelFirmwareDownload
 */
LIBRARY_API void Jabra_RegisterFirmwareProgressCallBack(FirmwareProgress const callback);

/**
 * @brief Recreates the session, input and output streams for all devices which
 * are connected to the phone and not to the application.
 */
LIBRARY_API void Jabra_Reconnect(void);

/**
 * @brief Check if a feature is supported by a device.
 * @param[in] deviceID ID for the specific device.
 * @param[in] feature The feature to check.
 * @return True if specified feature is supported.
 * @see Jabra_GetSupportedFeatures
 * @see Jabra_FreeSupportedFeatures
 */
LIBRARY_API bool Jabra_IsFeatureSupported(unsigned short deviceID, DeviceFeature feature);

/**
 * @brief Get array of features supported by a device.
 * @param[in] deviceID ID for the specific device.
 * @param[out] count Number of items in result.
 * @return Array of supported features, may be null. Shall be freed by calling
 * #Jabra_FreeSupportedFeatures.
 * @see Jabra_IsFeatureSupported
 * @see Jabra_FreeSupportedFeatures
 */
LIBRARY_API const DeviceFeature* Jabra_GetSupportedFeatures(unsigned short deviceID, unsigned int* count);

/**
 * @brief Free a list of features obtained by calling
 * #Jabra_GetSupportedFeatures.
 * @param[in] features List to delete.
 * @see Jabra_GetSupportedFeatures
 * @see Jabra_FreeSupportedFeatures
 */
LIBRARY_API void Jabra_FreeSupportedFeatures(const DeviceFeature* features);

/**
 * @brief Request that the headset does not play its 'end of call' tone when
 * the SCO closes next time. Precondition SCO is open. No checks - best effort.
 * @param[in] deviceID ID for the specific device.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_Ok Sent the cmd.
 * @return Device_WriteFail unable to send cmd to device
 */
LIBRARY_API Jabra_ReturnCode Jabra_RequestNoHangupToneNextTime(unsigned short deviceID);

/**
 * @brief Indicates whether the device is certified for Skype for Business.
 * @param[in] deviceID ID for a device.
 * @return True if device is certified for Skype for Business,
 * false if device is not certified for Skype for Business.
 */
LIBRARY_API bool Jabra_IsCertifiedForSkypeForBusiness(unsigned short deviceID);

/**
 * @brief Free a char array.
 * @param[in] arrPtr Array to delete.
 */
LIBRARY_API void Jabra_FreeCharArray(const char** arrPtr);

/**
 * @brief Checks if ringtone upload is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if upload ringtone is supported otherwise false.
 * @see Jabra_UploadRingtone
 * @see Jabra_UploadWavRingtone
 * @see Jabra_GetAudioFileParametersForUpload
 * @see Jabra_RegisterUploadProgress
 */
LIBRARY_API bool Jabra_IsUploadRingtoneSupported(unsigned short deviceID);

/**
 * @brief Upload ringtone to device. For Mac and Linux only (for Windows use
 * #Jabra_UploadWavRingtone).
 * @param[in] deviceID ID for a specific device.
 * @param[in] fileName Audio file name to be uploaded. The format supported is
 * wav 16kHz in uncompressed format.
 * @return Return_Ok.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail audio file has wrong type.
 * @return FileWrite_Fail if accessing the file fails.
 * @return Upload_AlreadyInProgress if upload is ongoing.
 * @see Jabra_IsUploadRingtoneSupported
 * @see Jabra_UploadWavRingtone
 * @see Jabra_GetAudioFileParametersForUpload
 * @see Jabra_RegisterUploadProgress
 */
LIBRARY_API Jabra_ReturnCode Jabra_UploadRingtone(unsigned short deviceID, const char* fileName);

/**
 * @brief Upload ringtone to device in wav format.
 * @param[in] deviceID ID for a specific device.
 * @param[in] fileName Audio file name to be uploaded. The format supported can
 * be obtained from #Jabra_GetAudioFileParametersForUpload.
 * @return Return_Ok if uploading is started successfully.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail audio file has wrong type.
 * @return Upload_AlreadyInProgress if upload is ongoing.
 * @see Jabra_IsUploadRingtoneSupported
 * @see Jabra_UploadRingtone
 * @see Jabra_GetAudioFileParametersForUpload
 * @see Jabra_RegisterUploadProgress
 */
LIBRARY_API Jabra_ReturnCode Jabra_UploadWavRingtone(unsigned short deviceID, const char* fileName);

/**
 * @brief Get details of audio file for uploading to device.
 * @param[in] deviceID ID for the specific device.
 * @return #Jabra_AudioFileParams structure for details of audio file.
 * @see Jabra_IsUploadRingtoneSupported
 * @see Jabra_UploadRingtone
 * @see Jabra_UploadWavRingtone
 * @see Jabra_RegisterUploadProgress
 */
LIBRARY_API Jabra_AudioFileParams Jabra_GetAudioFileParametersForUpload(unsigned short deviceID);

/**
 * @brief Type definition of function pointer to use for
 * #Jabra_RegisterUploadProgress.
 */
typedef void(*UploadProgress)(unsigned short deviceID, Jabra_UploadEventStatus status, unsigned short percentage);

/**
 * @brief Registration for ringtone upload progress event.
 * @param[in] callback Callback method, called during the upload process.
 * @see Jabra_IsUploadRingtoneSupported
 * @see Jabra_IsUploadImageSupported
 * @see Jabra_UploadRingtone
 * @see Jabra_UploadImage
 */
LIBRARY_API void Jabra_RegisterUploadProgress(UploadProgress const callback);

/**
 * @brief Checks if image upload is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if image upload image supported otherwise false.
 * @see Jabra_RegisterUploadProgress
 * @see Jabra_UploadImage
 */
LIBRARY_API bool Jabra_IsUploadImageSupported(unsigned short deviceID);

/**
 * @brief Upload image to device.
 * @param[in] deviceID ID for a specific device.
 * @param[in] fileName Name of image file to be uploaded.
 * @return Return_Ok if images is uploaded successfully.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Return_ParameterFail if file has wrong type.
 * @return FileWrite_Fail if accessing the file fails.
 * @return Upload_AlreadyInProgress if upload is ongoing.
 * @return File_Not_Accessible if unable to access the file specified.
 * @see Jabra_IsUploadImageSupported
 * @see Jabra_RegisterUploadProgress
 */
LIBRARY_API Jabra_ReturnCode Jabra_UploadImage(unsigned short deviceID, const char* fileName);

/**
 * @brief Sets the wizard mode (whether a full setup wizard, a limited setup
 * wizard or none will run on next power-on). Use #Jabra_IsFeatureSupported
 * to query feature support #DeviceFeature.FullWizardMode or
 * #DeviceFeature.LimitedWizardMode.
 * @param[in] deviceID ID for a specific device.
 * @param[in] wizardMode Wizard mode to be set (one of WizardModes).
 * @return Return_Ok if the wizard mode was set successfully.
 * @return Return_ParameterFail if the input parameter fails to comply.
 * @return Device_WriteFail if the write request was rejected.
 * @return Device_Unknown if the device is not known.
 * @see Jabra_IsFeatureSupported
 * @see Jabra_GetWizardMode
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetWizardMode(unsigned short deviceID, WizardModes wizardMode);

/**
 * @brief Reads the current wizard mode (whether a full setup wizard, a limited
 * setup wizard or none will run on next power-on). Use
 * #Jabra_IsFeatureSupported to query feature support
 * #DeviceFeature.FullWizardMode or #DeviceFeature.LimitedWizardMode.
 * @param[in] deviceID ID for a specific device.
 * @param[out] wizardMode Current wizard mode (one of WizardModes).
 * @return Return_Ok if the current wizard mode was retrieved successfully.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_Unknown id the device is not known.
 * @see Jabra_IsFeatureSupported
 * @see Jabra_SetWizardMode
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetWizardMode(unsigned short deviceID, WizardModes* wizardMode);

/**
 * @brief Checks if date and time synchronization is supported by the device.
 * @param[in] deviceID ID for a device.
 * @return True if date and time synchronization is supported otherwise false.
 * @see Jabra_SetDateTime
 */
LIBRARY_API bool Jabra_IsSetDateTimeSupported(unsigned short deviceID);

/**
 * @brief Sets (synchronizes) the date and time in the device.
 * @param[in] deviceID ID of the device to operate on.
 * @param[in] dateTime Date and time to set. If this parameter is set to NULL
 * the date and time is set to the current time of the platform.
 * @return Return_Ok if time and date is set successfully.
 * @return Device_Unknown if the deviceID specified is not known.
 * @see Jabra_IsSetDateTimeSupported
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetDateTime(unsigned short deviceID, const timedate_t* const dateTime);

/**
 * @brief Request info on supported device events.
 * @param[in] deviceID ID for a specific device.
 * @return event mask (0 if nothing supported, or on any error).
 * @see Jabra_SetSubscribedDeviceEvents
 * @deprecated
 */
LIBRARY_API uint32_t Jabra_GetSupportedDeviceEvents(unsigned short deviceID);

/**
 * @brief Set device events to subscribe to. Event callbacks are received
 * through the event listener mechanism for each platform.
 * @param[in] deviceID ID for a specific device.
 * @param[in] eventMask The bitmask representing supported device events
 * (one of DEVICE_EVENTS_* ) 0 = none
 * @return Return_Ok if successful.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_Unknown if the deviceID specified is not known.
 * @see Jabra_GetSupportedDeviceEvents
 * @deprecated
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetSubscribedDeviceEvents(unsigned short deviceID, uint32_t eventMask);

/**
 * @brief Gets a list of supported remote MMIs.
 * @param[in] deviceID ID for the specific device.
 * @param[in,out] types Pointer to allocated memory area containing count of
 * structures of #RemoteMmiDefinition. The memory area must be freed by calling
 * #Jabra_FreeRemoteMmiTypes.
 * @param[out] count Number of items passed via types.
 * @return Return_Ok list and count is valid.
 * @return Return_ParameterFail in case of an incorrect parameter.
 * @return Not_Supported the device does not support remote MMIv2.
 * @return Device_Unknown if the deviceID specified is not known.
 * @note RemoteMMIv2 only.
 * @see Jabra_FreeRemoteMmiTypes
 * @see Jabra_IsRemoteMmiInFocus
 * @see Jabra_GetRemoteMmiFocus
 * @see Jabra_ReleaseRemoteMmiFocus
 * @see Jabra_SetRemoteMmiAction
 * @see Jabra_RegisterRemoteMmiCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetRemoteMmiTypes(unsigned short deviceID, RemoteMmiDefinition** const types, int* count);

/**
 * @brief Frees the memory area allocated by #Jabra_GetRemoteMmiTypes.
 * @param[in] types Pointer to the memory area to free.
 * @note RemoteMMIv2 only.
 * @see Jabra_GetRemoteMmiTypes
 * @see Jabra_IsRemoteMmiInFocus
 * @see Jabra_GetRemoteMmiFocus
 * @see Jabra_ReleaseRemoteMmiFocus
 * @see Jabra_SetRemoteMmiAction
 * @see Jabra_RegisterRemoteMmiCallback
*/
LIBRARY_API void Jabra_FreeRemoteMmiTypes(RemoteMmiDefinition* types);

/**
 * @brief Gets the status of the remote MMI focus.
 * @param[in] deviceID ID for the specific device.
 * @param[in] type Type of remote MMI to get focus status of.
 * @param[out] isInFocus True if in focus, false if not.
 * @return Return_Ok if status has been gotten successfully.
 * @return Return_ParameterFail in case of an incorrect parameter.
 * @return Not_Supported the device does not support remote MMI.
 * @return Device_Unknown if the deviceID specified is not known.
 * @note RemoteMMIv2 only.
 * @see Jabra_GetRemoteMmiTypes
 * @see Jabra_FreeRemoteMmiTypes
 * @see Jabra_GetRemoteMmiFocus
 * @see Jabra_ReleaseRemoteMmiFocus
 * @see Jabra_SetRemoteMmiAction
 * @see Jabra_RegisterRemoteMmiCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_IsRemoteMmiInFocus(unsigned short deviceID, RemoteMmiType type, bool* isInFocus);

/**
 * @brief Gets the focus of the remote MMI specified. Once a remote MMI has
 * focus, the normal functionality of the MMI (button/LED) is suppressed until
 * #Jabra_ReleaseRemoteMmiFocus or #Jabra_ReleaseRemoteMmiFocusAll is called.
 * If only the LED output MMI functionality is required, action can be
 * specified as MMI_ACTION_NONE.
 * @param[in] deviceID ID for the specific device.
 * @param[in] type Type of remote MMI to get focus of.
 * @param[in] action Action to get focus of, acts as a filter/mask for the
 * actions on the RemoteMmiCallback callback. Note that several actions can be
 * OR'ed together e.g. (RemoteMmiInput)(MMI_ACTION_TAP | MMI_ACTION_DOWN).
 * @param[in] priority Priority of focus.
 * @return Return_Ok focus has been gotten successfully.
 * @return Not_Supported the device does not support remote MMI.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_WriteFail if it fails to write to the device.
 * @note RemoteMMIv2 only.
 * @see Jabra_GetRemoteMmiTypes
 * @see Jabra_FreeRemoteMmiTypes
 * @see Jabra_IsRemoteMmiInFocus
 * @see Jabra_ReleaseRemoteMmiFocus
 * @see Jabra_SetRemoteMmiAction
 * @see Jabra_RegisterRemoteMmiCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetRemoteMmiFocus(unsigned short deviceID, RemoteMmiType type, RemoteMmiInput action, RemoteMmiPriority priority);

/**
 * @brief Releases the focus of the remote MMI specified. Note that focus on
 * all actions are removed.
 * @param[in] deviceID ID for the specific device.
 * @param[in] type Type of remote MMI to release focus of.
 * @return Return_Ok focus has been release successfully.
 * @return Not_Supported the device does not support remote MMI.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_WriteFail if it fails to write to the device.
 * @note RemoteMMIv2 only.
 * @see Jabra_GetRemoteMmiTypes
 * @see Jabra_FreeRemoteMmiTypes
 * @see Jabra_IsRemoteMmiInFocus
 * @see Jabra_GetRemoteMmiFocus
 * @see Jabra_SetRemoteMmiAction
 * @see Jabra_RegisterRemoteMmiCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_ReleaseRemoteMmiFocus(unsigned short deviceID, RemoteMmiType type);

/**
 * @brief Sets an output action on the remote MMI. Note that
 * #Jabra_GetRemoteMmiFocus must be called once for the RemoteMmiType in
 * question prior to setting the output action, else Return_ParameterFail is
 * returned.
 * @param[in] deviceID ID for the specific device.
 * @param[in] type Type of remote MMI to set action of.
 * @param[in] outputAction Output LED action to set.
 * @return Return_Ok action has been set successfully.
 * @return Not_Supported the device does not support remote MMI.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Device_WriteFail if it fails to write to the device e.g. if
 * #Jabra_GetRemoteMmiFocus has not called prior to calling.
 * #Jabra_SetRemoteMmiAction.
 * @return Return_ParameterFail if an incorrect/unsupported parameter has been
 * passed or #Jabra_GetRemoteMmiFocus has not been called for the
 * #RemoteMmiType in question.
 * @note RemoteMMIv2 only.
 * @see Jabra_GetRemoteMmiTypes
 * @see Jabra_FreeRemoteMmiTypes
 * @see Jabra_IsRemoteMmiInFocus
 * @see Jabra_GetRemoteMmiFocus
 * @see Jabra_ReleaseRemoteMmiFocus
 * @see Jabra_RegisterRemoteMmiCallback
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetRemoteMmiAction(unsigned short deviceID, RemoteMmiType type, RemoteMmiActionOutput outputAction);

/**
 * @brief Type definition of function pointer to use for
 * #Jabra_RegisterRemoteMmiCallBack.
 * @note RemoteMMIv2 only.
 */
typedef void(*RemoteMmiCallback)(unsigned short deviceID, RemoteMmiType type, RemoteMmiInput action);

/**
 * @brief Register for remote MMI event callback.
 * @param[in] callback RemoteMmiCallback callback method, called when a remote
 * MMI input event is generated.
 * @note RemoteMMIv2 only.
 * @see Jabra_GetRemoteMmiTypes
 * @see Jabra_FreeRemoteMmiTypes
 * @see Jabra_IsRemoteMmiInFocus
 * @see Jabra_GetRemoteMmiFocus
 * @see Jabra_ReleaseRemoteMmiFocus
 * @see Jabra_SetRemoteMmiAction
 */
LIBRARY_API void Jabra_RegisterRemoteMmiCallback(RemoteMmiCallback const callback);

/**
 * @brief Get the panic list.
 * @param[in] deviceID ID for a specific device.
 * @return Pointer to panic list or NULL if not available.
 * @note As memory is allocated through SDK, it must be freed by calling
 * #Jabra_FreePanicListType.
 * @see Jabra_FreePanicListType
 */
LIBRARY_API Jabra_PanicListType* Jabra_GetPanics(unsigned short deviceID);

/**
 * @brief Frees the #Jabra_PanicListType structure.
 * @param[in] panicList #Jabra_PanicListType structure to be freed.
 * @see Jabra_GetPanics
 */
LIBRARY_API void Jabra_FreePanicListType(Jabra_PanicListType *panicList);

/**
 * @brief Sets a static timestamp in the device. Can be used for later
 * referencing using #Jabra_GetTimestamp.
 * @param[in] deviceID ID for a specific device.
 * @param[in] newTime Timestamp to be set. Unix epoch.
 * @return Return_Ok if successful.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_Unknown if the deviceID specified is not known.
 * @see Jabra_GetTimestamp
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetTimestamp(unsigned short deviceID, const uint32_t newTime);

/**
 * @brief Gets the static timestamp in the device.
 * @param[in] deviceID ID for a specific device.
 * @param[out] result Timestamp from device. Unix epoch.
 * @return Return_Ok if successful.
 * @return Not_Supported if the functionality is not supported.
 * @return Device_Unknown if the deviceID specified is not known.
 * @see Jabra_SetTimestamp
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetTimestamp(unsigned short deviceID, uint32_t* const result);

/**
 * @deprecated Please use the DeviceCatalogue_params for #Jabra_Initialize.
 * @brief Preloads the configuration cache with the content of the specified
 * archive. To get the full benefit, this should happen before calling
 * #Jabra_Initialize, as that enables device connections and may initiate
 * background updates of device data. No existing data will be overwritten.
 * @param[in] zipFileName Full path name of the ZIP archive to preload from.
 * @return True if preloading succeeds, false otherwise.
 */
LIBRARY_API bool Jabra_PreloadDeviceInfo(const char* zipFileName);

/**
 * @brief Play ringtone in device.
 * @param[in] deviceID ID for a specific device.
 * @param[in] level Level to Play.
 * @param[in] type Type of ringtone to Play.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if device is not supporting ringtones or input
 * parameters are wrong.
 */
LIBRARY_API Jabra_ReturnCode Jabra_PlayRingtone(unsigned short deviceID, const uint8_t level, const uint8_t type);

/** @brief Subscribe/unsubscribe to JackStatus events. Not available on all devices. If not available, the client will get no events.
 * @param[in] deviceID ID for a specific device.
 * @param[in] listener The callback for JackStatus events. Set to nullptr to unsubscribe. Callback will occur on a separate thread.
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetJackConnectorStatusListener(unsigned short deviceID, JackConnectorStatusListener listener);

/**
  * @brief Subscribe/unsubscribe to HeadDetectionStatus events. Not available on all devices. If not available, the client will get no events.
  * @param[in] deviceID ID for a specific device.
  * @param[in] listener The callback for HeadDetectionStatus events. Set to nullptr to unsubscribe. Callback will occur on a separate thread.
  */
LIBRARY_API Jabra_ReturnCode Jabra_SetHeadDetectionStatusListener(unsigned short deviceID, HeadDetectionStatusListener listener);

/**
  * @brief Subscribe/unsubscribe to LinkConnectionStatus events. Not available on all devices. If not available, the client will get no events.
  * @param[in] deviceID ID for a specific device.
  * @param[in] listener The callback for LinkConnectiontatus events. Set to nullptr to unsubscribe. Callback will occur on a separate thread.
  */
LIBRARY_API Jabra_ReturnCode Jabra_SetLinkConnectionStatusListener(unsigned short deviceID, LinkConnectionStatusListener listener);

/**
 * @brief Reboot the device.
 * @param[in] deviceID ID for a specific device.
 * @return Return_Ok if success.
 * @return Device_Unknown if the deviceID specified is not known.
 * @return Not_Supported if not supported
  */
LIBRARY_API Jabra_ReturnCode Jabra_RebootDevice(unsigned short deviceID);

/** Dect information about density and error counts */

/** It is possible to calculate a “percentage density” like this:
 *   (100 * MaximumReferenceRSSI * NumberMeasuredSlots) / SumMeasuredRSSI. If this percentage number is high,
 *    and there is a large number of errors, then the problem is most likely too high density.
 */

typedef struct _DectInfoDensity {
  uint16_t SumMeasuredRSSI;		    /* This is the sum of RSSI measured for all slots. */
  uint8_t  MaximumReferenceRSSI;	/* This is the maximum RSSI expected to be measured from 1 slot. */
  uint8_t  NumberMeasuredSlots;	  /* Number of slots measured in current communication mode. */
  uint16_t DataAgeSeconds;		    /* Time since measurement was taken. */
} Jabra_DectInfoDensity;

/** The most interesting counter is the handoversCount, which states how many times the connection has
 *  moved to a different slot position. Moving doesn’t necessarily give any effect on the audio, but there is a risk that it is hearable.
 *  When you reach a level of 5 or above in multiple consecutive readings it will definitely be noticeable.
 *  The other counters describe what is the reason the a handover has occurred, there may be multiple errors resulting in a single handover.
 */
typedef struct _DectInfoErrorCount {
  uint16_t syncErrors;		/* Number of errors in SYNC field.*/
  uint16_t aErrors;			   /* Number of errors in A field.*/
  uint16_t xErrors;			   /* Number of errors in X field.*/
  uint16_t zErrors;			   /* Number of errors in Z field.*/
  uint16_t hubSyncErrors;	 /* Number of errors in HUB Sync field.*/
  uint16_t hubAErrors;		 /* Number of errors in HUB A field.*/
  uint16_t handoversCount; /* Handover count.*/
} Jabra_DectErrorCount;

typedef enum _DectInfoType {
  DectDensity = 0x00,
  DectErrorCount = 0x01,
} Jabra_DectInfoType;

typedef struct _DectInfo {
  Jabra_DectInfoType DectType;
  union {
    Jabra_DectInfoDensity DectDensity;
    Jabra_DectErrorCount  DectErrorCount;
  };
  unsigned int RawDataLen;
  uint8_t RawData[57];
} Jabra_DectInfo;

/**
 * @brief Registration for dect density and error count events.
 * @param[in] DectInfoFunc Callback method, called when
 * a dect device sends a dect density or dect error count event.
 * The Jabra_DectInfo structure must be freed with Jabra_freeDectInfoStr
 */
LIBRARY_API void Jabra_RegisterDectInfoHandler(void(*DectInfoFunc)(unsigned short deviceID, Jabra_DectInfo *dectInfo));

/**
 * Frees the #Jabra_DectInfo
 * @param[in] dectInfo #Jabra_DectInfo structure to be freed.
 */
LIBRARY_API void Jabra_FreeDectInfoStr(Jabra_DectInfo *dectInfo);


#endif /* COMMON_H */
