# Changelog
All notable changes to this project will be documented in this file

### General

Latest version 3.6.2 is a production release for all supported platforms: Windows (x86, x64, ARM64), Mac (Intel, M1) and Linux (x64).

This release includes miscellaneous bugfixes, feature wrappers, update to the underlying C-library and general improvements that results in faster operation and much improved stability.

--------------------------------

### v3.6.2 (2022-06-23)
- Upgraded embedded native libraries to version 1.12.2.0 which comes with these fixes:
  - Windows: Resolved potential memory leak through device attach/detach
  - Linux: Child devices would sometimes not appear or have incorrect properties
- Fixed issue where getFirmwareVersionBundleAsync() would sometimes return garbled characters.

### v3.6.1 (2022-05-09)
- Upgraded embedded native libraries to version 1.12.1.0 which comes with these changes:
  - Additional device features
  - mac SDK will again produce a log file as default
  - SDK will no longer show conference mode DECT headsets, only the primary ones
  - Updated embedded curl library in Windows DLL
  - Windows DLL now links against the DLL CRT (previously Static CRT) which should be available on Windows 10 and Windows 11.
    If not present, this can be installed manually by downloading Microsoft Visual C++ 2017 Redistributable (v140).

### v3.6.0 (2022-03-21)
- Upgraded embedded native libraries to version 1.12.0.0 which fixes these issues:
  - Unsubscribing from an event on a device would also disable the event for other connected SDK clients
  - Memory leak when handling ZIP files
  - Factory reset would sometimes report as failed on Engage 65/75 when operation did not complete within timeout period
- Added API functions:
  - getSubDevicePropertyAsync
  - getUserDefinedDeviceNameAsync
  - getConnectedHeadsetNamesAsync
  - getLanguagePackInformationAsync  
  - getRemoteControlFirmwareVersionAsync
  - getNetworkAuthenticationModeAsync
  - setNetworkAuthenticationModeAsync
  - setNetworkAuthenticationIdentityAsync
  - getMACAddressAsync
  - getConstantsAsync
- Also added:
  - Event onNetworkStatusChangedEvent
  - Device feature DECTOTAFWUSupported
  - DECT in enumDeviceConnectionType (reported for headsets connected to Link 400)
  - DeviceType now contains a parentDeviceId property which identifies which dongle a headset is connected through
  - Native lib for Apple M1
  - Support for Jabra Link 400 (DECT dongle)

### v3.5.1 (2021-12-27)
- Upgraded embedded native libraries to version 1.11.0.1 which fixes this issue:
  - For some devices, particularly older ones, the SDK would be unable to download resources which meant
    that many operations would fail.

### v3.5.0 (2021-10-19)
- Upgraded embedded native libraries to version 1.11.0.0 with these bug fixes, among other minor ones:
  - SDK will now download 1280x1280px product images into Resources folder
  - Fix crash on Mac while doing certain long operations such as Jabra_GetDiagnosticLogFile()
  - Jabra_ConfigureXpressManagement will now revert previous settings when validation failed
- Added API methods:
  - getXpressManagementNetworkStatusAsync()
  - getSensorRegionsAsync()
  - triggerDECTPairingAsync
  - triggerDECTPairingSecureAsync()
  - getDECTPairingKeyAsync()
  - setDECTPairingKeyAsync()
  - setBTLinkQualityChangeEventsEnabledAsync() (triggers event 'onBluetoothLinkQualityChangeEvent')
  - getFirmwareVersionBundleAsync()
- Added Windows ARM64 support

### v3.4.0 (2021-07-15)
- Upgraded embedded native libraries to version 1.10.1.0 with these bug fixes, among other minor ones:
  - Jabra_PreloadAttachedDeviceInfo() and Jabra_PreloadDeviceInfo() can now overwrite existing files
  - Native SDK GetSettings() will now return an empty string instead of NULL for undefined values
- Added API calls: configureXpressManagementAsync() and getUSBStateAsync()

### v3.3.0 (2021-06-01)
- Upgraded embedded native libraries to version 1.9.0.0 which includes these bug fixes:
  - macOS 11.x: Firmware updater failed to download
  - When requesting a factory reset on an actively streaming device, it should not return success
  - Calling Jabra_SetSettings (setSettingsAsync) a 2nd time with the same payload after one failed attempt should now properly try again
  - SDK should no longer attempt to connect to the backend for third party devices
  - Battery status is now reported through the device log
- Adding Video, Whiteboard and Network related APIs.
  The first product to support these new interfaces is Panacast 50.
- Added functions to support preloading of device manifest files.
  These can be used in environments where the network setup prohibits the SDK from
  reaching the backend server or if SDK network communication has been disabled.

### v3.2.3 (2021-05-05)
- Upgraded embedded native libraries for Windows to version 1.8.8.5 in order to fix
  some communication errors.

### v3.2.2 (2021-04-06)
- Upgraded embedded native libraries for Windows/Mac/Ubuntu to version 1.8.8.4.
- Added API calls: getDateTimeAsync(), getLockAsync(),
  releaseLockAsync(), isLockedAsync()
- Fixed a crash bug on Japanese mac system
- Updated 3rd party dependencies, incl. security fixes.
- Fixed invalid return values from getPanicsAsync()

### v3.2.1 (2020-12-02)
- Upgraded embedded native "C" sdk for Mac to v1.8.7.15 to support macOS 11.

### v3.2.0 (2020-07-09)
- Added support for DectInfo events (DectDensity and DectErrorCount)
  - Updated electron-renderer-helper to support DectInfo events
- Upgraded embedded native "C" sdk to v1.8.7.12

### v3.1.0 (2020-06-16)
- Upgraded embedded native "C" sdk to v1.8.7.11
- Added various new APIs for above "C" sdk, incl. reboot, locking etc.
- Added support for detecting non-Jabra devices (new optional argument to createJabraApplication).
- Updated related electron helper JabraApiServerFactory.create method arguments (optional package but breaking change if used).
- Updated 3rd party dependencies to latest versions, incl. security fixes.
- It is no longer necessary/advised to set GN_HID protocol to get 
  all call control events! Call control applications that upgrade should remove
  explicit setting of the protocol.
- Added support for RemoteMmi
- Updated electron-renderer-helper to support RemoteMmi events
- Fixed character encoding bug where special characters in Windows username would cause wrong file paths
- Fixed links in readme.

### v3.0.0 - (2020-2-28)
- Upgraded embedded native "C" sdk to v1.8.3.10 which fixes the following:
  - Sometimes, core library functions could cause 100% CPU load on Windows clients.
  - Inserting or removing chorded headsets sometime could cause host application to crash
- Removed getLastFirmwareUpdateErrorInfoAsync (no longer supported by "C" sdk).
- Versioning scheme documented, beta disclaimer updated.

For other minor functional changes/updates, see release notes for v1.8.3 on developer.jabra.com

Disclaimer: Exceptionally, this release is only validated for Windows. Mac and Linux core library versions do not suffer from the 100% CPU utilisation issue. We therefore recommend that you for production use stay on v2.0.0 for those platforms, until the next general relase is made for all platforms.

Known issues: 
  - Third party products can't be recognized by NodeJS SDK. Non-jabra headsets are no longer visible through the API, as this option is turned off be default in new version of the underlying core library.
  - Speak 710 connected via Link 370 can't set settings (also present in v2.0.0).
  - API function enableEquailzerAsync() do not work as expected (also present in v2.0.0).

Device support:
The device support is identical to the one for Windows SDK v1.8.3.10, but please note that this release of the node.js SDK has only been validation-tested with the following devices:

- Biz 2400 II
- Engage 50
- Engage 75
- Evolve 65t
- Evolve 75 / Link 370
- Speak 710 / Link 370

### v2.0.0 - (2019-12-10)
- New npm package with gnaudio scope "@gnaudio/jabra-node-sdk" that replaces previous "jabra" and "jabra-dev" packages.
- New optional "@gnaudio/jabra-electron-renderer-helper" to make secure client-side access to Jabra api much easier from Electron.

- Complete rewrite in typescript based on N-API (Next generation APIs for Node.js)
  Provided API is ~95% identical but there are few breaking changes and bugfixes such as:
    - The default initializer function export (antipattern) is removed!
      The initializer function is now a plain function export called "createJabraApplication" that unlike the previous function 
      returns a promise which resolves when the sdk is initialized
      (See examples for new usage syntax).
    - Added scanForDevicesDoneAsync async on JabraType and removed similar sync function on DeviceType.
    - device deattach event takes a DeviceType as single argument.
    - getAttachedDevices now returns an array instead of a map.
    - Changed devlog event to return a described object instead of string.
    - Some settings functions renamed, also now uses DeviceSettings as type.
    - Various bugfixes in typescript declarations.
    - Removed all FFI related artifacts, incl. constants and error codes.

- Known important issues: 
    - This version will nondeterministicly produce 100% CPU load on some Windows machines in some 
      CC scenarios. We aim to identify and fix this issue in an upcomming beta release.
                

### v1.0.5 (2017-02-27)
- Minor changes in documentation. 
    - Mentioned as a Beta release in README.
    - Cleanup of CHANGELOG

### v1.0.4 (2017-02-26)
- Beta public version for Winodws(SDK Version: 1.2.0.24) and Mac(SDK Version: 1.2.0.25)
- Minor changes in type definition file
- Removed package scoping
