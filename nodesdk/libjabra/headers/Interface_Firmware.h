#ifndef INTERFACE_FIRMWARE_H
#define INTERFACE_FIRMWARE_H

/* --------------------------------------------------------------------
 *
 *  GGGGGG  NN    N
 *  G       N N   N
 *  G  GGG  N  N  N - Audio
 *  G    G  N   N N
 *  GGGGGG  N    NN
 *
 *  Copyright (c) 2021, GN-Audio
 * -------------------------------------------------------------------- */

/**
 * @file Interface_Firmware.h
 * @brief Defines the Firmware interfaces for the Jabra SDK.
 * Developer information can be found in the SDK documentation.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "Common.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/** This struct represents firmware version info of a firmware from
 * cloud. */
typedef struct _FirmwareInfo
{
    /** Version of firmware. */
    char* version;
    /** Size of firmware file in KB/MB. */
    char* fileSize;
    /** Release date of firmware. */
    char* releaseDate;
    /** Firmware stage. */
    char* stage;
    /** Release notes of firmware. */
    wchar_t* releaseNotes;
} Jabra_FirmwareInfo;

typedef struct _FirmwareInfoList
{
    unsigned count;
    Jabra_FirmwareInfo* items;
} Jabra_FirmwareInfoList;

/** This enum represents event type for callback. */
typedef enum  _FirmwareEventType
{
    Firmware_Download = 0,
    Firmware_Update
} Jabra_FirmwareEventType;

/** @todo needs to be consolidated with other result codes. */
typedef enum _FirmwareEventStatus
{
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
typedef enum _FirmwareUpdateReturnCode
{
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

/** This enum represents actions/parameters required to update firmware in a given device. */
typedef enum _DeviceFWURequirement
{
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
typedef enum _Regions
{
    EMEA_AU_NZ = 1,
    NA = 2,
    NA_Japan = 3,
    UK_APAC = 4,
    Korean = 5,
    EA_Oceania = 6,
    Global = 7
} Regions;

/** This struct represents a single language with an integer id and a wide character string. */
typedef struct _Language
{
    unsigned int id;
    char* languageName;      /** Language name in UTF-8 encoding */
} Language;

/** This struct represents a list of languages */
typedef struct _LanguageList
{
    int count;              /** Amount of languages stored in this list */
    Language* languages;
} LanguageList;

/**
 * @brief Get firmware version of the device.
 * @param[in] deviceID          ID for a specific device
 * @param[out] firmwareVersion  Pointer to a location where the firmware version
 * is written. Must be allocated by the caller
 * @param[in] count             Maximum number of characters to copy to firmwareVersion
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @return Return_ParameterFail A NULL pointer was passed
 * @sa @wrap{Jabra_GetFirmwareVersion}
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetFirmwareVersion(unsigned short deviceID, char* const firmwareVersion, int count);

/**
 * @brief Checks if firmware lock is enabled. If the firmware lock is enabled
 * it is not possible to upgrade nor downgrade the firmware. In this situation
 * the firmware can only be changed to the same version e.g. if you want to
 * change the language.
 * @param[in] deviceID      ID for specific device
 * @return true             Firmware lock is enabled
 * @return false            Firmware lock is disabled
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
 * @param[in] deviceID          ID for specific device
 * @param[in] enable            true = enable lock, false = disable lock
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @see Jabra_IsFirmwareLockEnabled
 */
LIBRARY_API Jabra_ReturnCode Jabra_EnableFirmwareLock(unsigned short deviceID, bool enable);

/**
 * @brief Check if Firmware update available for device.
 * @param[in] deviceID          ID for specific device
 * @param[in] authorizationId   Authorization ID
 * @return Device_Unknown       deviceID is unknown
 * @return Return_ParameterFail authorizationId is NULL or the
 * request to the server is invalid.
 * @return Firmware_UpToDate if device has latest firmware version.
 * @return Firmware_Available if new firmware version is available for the
 * device.
 * @return No_Information if firmware file is not available.
 * @return NetworkRequest_Fail if request to the server fails.
 * @return Invalid_Authorization if authorization is invalid.
 * @return Not_Supported        Functionality is not supported on this device
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
 * @param[in] deviceID          ID for specific device
 * @param[in] authorizationId   Authorization ID
 * @return #Jabra_FirmwareInfo Pointer for details of the latest firmware,
 * @return NULL if the device is unknown, authorizationId is NULL or
 * there was a request error.
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
 * @brief Frees the firmware information struct members.
 * @param[in] firmwareInfo  #Jabra_FirmwareInfo struct to be freed
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
 * @param[in] deviceID      ID for specific device
 * @param[in] version       Version for which the path is required
 * @return firmwareFilePath firmware file path of the device
 * @return NULL if the device is unknown or version is NULL.
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
 * @param[in] deviceID          ID for specific device
 * @param[in] authorizationId   Authorization ID
 * @return A list of information about all firmware versions.
 * @return NULL No information is available, the device is unknown
 * or authorizationId is NULL
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
 * @brief Frees the list of firmware information struct.
 * @param[in] firmwareInfolist  #Jabra_FirmwareInfoList struct to be freed
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
 * @param[in] deviceID          ID for specific device
 * @param[in] version           Version for which file download needs to be initiated.
 * @param[in] authorizationId   Authorization ID.
 * @return Device_Unknown       deviceID is unknown.
 * @return Device_Invalid       deviceID is invalid
 * @return Return_ParameterFail A NULL pointer was passed or version or authorizationId is incorrect
 * @return Return_Async         Firmware download is in progress and will be
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
 * @param[in] deviceID          ID for specific device
 * @param[in] authorizationId   Authorization ID.
 * @return Device_Unknown       deviceID is unknown
 * @return Device_Invalid       deviceID is invalid
 * @return Return_ParameterFail authorizationId is NULL or invalid
 * @return Return_Async         Firmware download is in progress and will be
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
 * @param[in] deviceID          ID for specific device
 * @param[in] filepath          Firmware file path
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Device_Invalid       deviceID is invalid
 * @return File_Not_Accessible  Firmware file path is invalid
 * @return FWU_Application_Not_Available Firmware updater folder/application
 * is not found.
 * @return Return_ParameterFail filepath is NULL or invalid
 * @return Return_Async         Firmware update is in progress. Use FWU progress
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
 * @param[in] deviceID          ID for specific device
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
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
 * @param[in] callback      Callback method, called when firmwareProgress event is
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

#endif /* INTERFACE_FIRMWARE_H */
