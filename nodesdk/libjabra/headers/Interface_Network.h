#ifndef INTERFACE_NETWORK_H
#define INTERFACE_NETWORK_H

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
 * @file Interface_Network.h
 * @brief Defines the network-related interfaces for the Jabra SDK.
 * Developer information can be found in the SDK documentation.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "Common.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct _IPv4Status
{
    bool InterfaceEnabled;
    bool DHCPEnabled;
    bool ConnectionStatus;
    struct _IP
    {
        uint8_t octet1;
        uint8_t octet2;
        uint8_t octet3;
        uint8_t octet4;
    } IP, SubNetMask;
} IPv4Status;

/**
 * @brief     Enables or disables remote management. If enabling and there are no active network interfaces (eth/wlan),
 *            it will automatically enable the Ethernet interface as well since remote mgmnt requires a network.
 *            If a timeout is specified, it will wait for an interface to be IP-configured before returning.
 * @param[in] deviceID  ID for a specific device
 * @param[in] enable    Enable state: true=enabled, false=disabled
 * @param[in] timeout   Time to wait for ethernet interface to become ready. Set to 0 to return immediately.
 * @return Return_Ok            Remote management is enabled
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @return Device_WriteFail     Failed while writing to device
 * @return NetworkRequest_Fail  Management was enabled, but no working network interface was available at the time of return.
 * @see Jabra_GetEthernetIPv4Status
 */
LIBRARY_API Jabra_ReturnCode Jabra_EnableRemoteManagement(unsigned short deviceID, bool enable, unsigned int timeout);

/**
 * @brief       Get whether remote management is enabled on Newport device.
 * @param[in]   deviceID ID for a specific device
 * @param[out]  enable   Enable state: true=enabled, false=disabled
 *                       Will only be written to if return value == Return_Ok.
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @return Return_ParameterFail A NULL pointer was passed
 * @return Device_ReadFails     Failed while reading from device
 */
LIBRARY_API Jabra_ReturnCode Jabra_IsRemoteManagementEnabled(unsigned short deviceID, bool* enable);

/**
 * @brief     Set Xpress server URL. Will block until URL has been validated or timeout reached.
 *            In case of NetworkRequest_Fail or Return_Timeout return codes, URL will still be set,
 *            so if this is not wanted, clear the URL by setting url to an empty string.
 * @param[in] deviceID  ID for a specific device
 * @param[in] url       Pointer to char buffer containing NULL-terminated URL
 * @param[in] timeout   Time to wait for URL validation in ms. Set to 0 to return immediately.
 * @return Return_Ok            URL was changed and verified successfully
 * @return Device_Unknown       deviceID is unknown
 * @return Return_ParameterFail A NULL pointer was passed
 * @return Device_WriteFail     Failed while writing to device
 * @return System_Error         Some unspecified system error occurred
 * @return NetworkRequest_Fail  URL was evaluated to be incorrect or unreachable
 * @return Return_Timeout       URL validation did not complete before timeout
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetXpressUrl(unsigned short deviceID, const char* url, unsigned int timeout);

/**
 * @brief       Get Xpress server URL.
 * @param[in]   deviceID    ID for a specific device
 * @param[out]  url         Pointer to a buffer where the URL string will be written
 *                          Must be preallocated by the caller
 * @param[in]   size        Size of preallocated buffer 'url'. Max limit 2kB
 * @return Return_Ok            Call was successful. Will also return Return_Ok if url is not set
 * or device does not support xpress features, however output string will be "".
 * @return Device_Unknown       deviceID is unknown
 * @return Return_ParameterFail A NULL pointer was passed or buffer is too small
 * @return Device_ReadFails     Failed while reading from device
  */
LIBRARY_API Jabra_ReturnCode Jabra_GetXpressUrl(unsigned short deviceID, char* url, int size);

/**
 * @brief       Set password for password provisioning.
 * @param[in]   deviceID    ID for a specific device
 * @param[in]   password    Pointer to char buffer containing NULL-terminated password
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Return_ParameterFail A NULL pointer was passed
 * @return Device_WriteFail     Failed while writing to device
 */
LIBRARY_API Jabra_ReturnCode Jabra_SetPasswordProvisioning(unsigned short deviceID, const char* password);

/**
 * @brief       Get the password for provisioning.
 * @param[in]   deviceID    ID for a specific device
 * @param[out]  password    Pointer to a buffer where the password string will be written
 *                          Must be preallocated by the caller
 * @param[in]   size        Size of preallocated buffer 'password'. Max limit 2kB
 * @return Return_Ok            Call was successful. Will also return Return_Ok if password is not set
 * or device does not support password provisioning, however output string will be "".
 * @return Device_Unknown       deviceID is unknown
 * @return Return_ParameterFail A NULL pointer was passed or buffer is too small
 * @return Device_ReadFails     Failed while reading from device
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetPasswordProvisioning(unsigned short deviceID, char* password, int size);

/**
 * @brief       Notify the app connected to an Xpress capable device whether device is being remotely managed.
 * @param[in]   deviceID    ID for a specific device
 * @param[in]   isConnected true=connection to the backend is established, false=is not established
 * @return Return_Ok            Call was successful. Unsupported devices will silently
 * ignore this, so Return_Ok does not imply that the feature is supported.
 * @return Device_Unknown       deviceID is unknown
 * @return Device_WriteFail     Failed while writing to device
 */
LIBRARY_API Jabra_ReturnCode Jabra_NotifyXpressConnectionStatus(unsigned short deviceID, bool isConnected);

/**
 * @brief Registration for Xpress connection status change callback.
 * @param[in] xpressConnectionStatus Callback method to be called when the Xpress connection status is changed
 */
LIBRARY_API void Jabra_RegisterXpressConnectionStatusCallback(void(*xpressConnectionStatus)(unsigned short, bool));

/**
 * @brief Gets the status of the Ethernet connection; Enable, DHCP, Connected states and IP + subnet mask.
 *
 * @param[in] deviceID          ID for a specific device
 * @param[out] ethernetStatus   Pointer to an IPv4Status struct that will be filled with the details of the Ethernet connection.
 *
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @return Return_ParameterFail A NULL pointer was passed
 * @return Device_ReadFails     Failed while reading from device
*/
LIBRARY_API Jabra_ReturnCode Jabra_GetEthernetIPv4Status(const unsigned short deviceID, IPv4Status* ethernetStatus);

/**
 * @brief Gets the status of the WLAN connection; Enable, DHCP, Connected states and IP + subnet mask.
 *
 * @param[in] deviceID          ID for a specific device
 * @param[out] wlanStatus       Pointer to an IPv4Status struct that will be filled with the details of the WLAN connection.
 *
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @return Return_ParameterFail A NULL pointer was passed
 * @return Device_ReadFails     Failed while reading from device
*/
LIBRARY_API Jabra_ReturnCode Jabra_GetWLANIPv4Status(const unsigned short deviceID, IPv4Status* wlanStatus);

/**
 * @brief Gets the diagnostic log file and writes it to a file on local file system
 * Requires prior call to #Jabra_TriggerDiagnosticLogGeneration() to initiate file preparation
 * @param[in] deviceID          ID for a specific device
 * @param[in] FileName          Destination file name on local file system
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return File_Not_Accessible  File is not ready on device
 * @return FileWrite_Fail       Destination file writing failed
 */
LIBRARY_API Jabra_ReturnCode Jabra_GetDiagnosticLogFile(const unsigned short deviceID, const char* const FileName);

/**
 * @brief Type definition of function pointer to use for
 * #Jabra_TriggerDiagnosticLogGeneration.
 */
typedef void(*DiagnosticLogReadyEventHandler)(const unsigned short deviceID);

/**
 * @brief Trigger generation of diagnostic log.
 * @param[in] deviceID          ID for a specific device
 * @return Return_Ok            Call was successful
 * @return Device_Unknown       deviceID is unknown
 * @return Not_Supported        Functionality is not supported on this device
 * @return Device_WriteFail     Failed while writing to device
 */
LIBRARY_API Jabra_ReturnCode Jabra_TriggerDiagnosticLogGeneration(const unsigned short deviceID);

/**
 * @brief Registration for diagnostic log event.
 * @param[in] callback          Callback method, called when diagnosticLog event is
 * received from device. Set to NULL to disable
 * @see Jabra_TriggerDiagnosticLogGeneration
 * @see Jabra_GetDiagnosticLogFile
 */
LIBRARY_API void Jabra_RegisterDiagnosticLogCallback(DiagnosticLogReadyEventHandler const callback);

#endif /* INTERFACE_NETWORK_H */
