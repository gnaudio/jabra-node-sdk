Jabra Node.js SDK

# Table of Contents
- [Versions](#versions)
- [Pre-requisite](#pre-requisite)
- [Installation](#installation)
- [Debugging and Logging](#debugging-and-logging)
- [API Reference](#api-reference)
- [Examples](#examples)
    - [Simple Example](#simple-button-events-example-using-javascript-and-plain-promises)
    - [Multiple device management](#multiple-device-management-with-typescript-and-asyncawait)
- [Sequence Diagrams](#sequence-diagrams)
- [Setup VSCode](#setup-vscode)
    - [Windows](#windows)
    - [Linux](#linux)
- [Bug report](https://github.com/gnaudio/jabra-node-sdk#bug-reports)
- [Changelog](https://github.com/gnaudio/jabra-node-sdk/blob/master/CHANGELOG.md)
- [License](https://github.com/gnaudio/jabra-node-sdk/blob/master/LICENSE.md)
- [FAQ](https://github.com/gnaudio/jabra-node-sdk/blob/master/FAQ.md)

## Versions

[![npm version](https://badge.fury.io/js/%40gnaudio%2Fjabra-node-sdk.svg)](https://badge.fury.io/js/%40gnaudio%2Fjabra-node-sdk)

Latest **STABLE** release is  [distributed on npmjs.com](https://www.npmjs.com/package/@gnaudio/jabra-node-sdk) (see [changelog](https://github.com/gnaudio/jabra-node-sdk/blob/master/CHANGELOG.md) for changes and notable defects). 

In addition, **BETA** versions may be available on npmjs with a version name ending with "-beta.X" specifier. For such BETA versions,the following applies:

    > Disclaimer for Alpha and Beta Software Release

    > Please note that this is a copyrighted Jabra Product ((c) 2017 GN Audio A/S) (the “Product”) which is still undergoing final testing before its official release. The Product and all content found on it are provided on an “as is” and “as available” basis. Jabra does not provide any warranties, neither express nor implied, as to the suitability or usability of the Product or any of its content.

    > Jabra disclaims liability for any loss, whether direct, indirect, special or consequential, suffered by any person as a result of their use of the Product or the software embedded, and for any claim arising from any injury or damage, including but not limited to, any personal or bodily injury or tangible property damage, arising from or resulting in any way from any alleged defects in the material, workmanship, or performance of the Product, and Company shall indemnify Jabra in respect of any such claims. All usage of the Product is at users’ own risk and the user will be solely responsible for any resulting from such activities.

    > Should you encounter any bugs, glitches, dangers, lack of functionality, lack of comfort or other problems with the Product, please let us know immediately so we can rectify these accordingly. Your help in this regard is greatly appreciated.

    > The Product is released solely for test use in Company’s organization and must not under any circumstances be distributed to third parties without prior written acceptance from Jabra.

    > By using the software, I accept and acknowledge the content herein.`

## Pre-requisite
1. Node.js v8.x or later.
2. **On MacOS:** `xcode` & `python 2.7`. By default, Python is installed on macOS but make sure correct version(2.7.x) is installed. Install Xcode from App store or download it from [here](https://developer.apple.com/xcode/download/).
3. **On Windows:** `Visual C++ Build Tools` & `Python 2.7`. You can install all of these by following any of the below mentioned steps.

    3.1. **During Node.js installation** By ticking the checkbox when prompted during Node.js installation for `Tools for Native modules`, this installs both `Visual C++ Build Tools` & `Python 2.7`. **Note** This is prompted only for Windows platform. For other platforms like Linux or MAC, C++ compilation tools has to be installed separately as mentioned in other steps.

    3.2. Both `Visual C++ Build Tools` & `Python 2.7` can also be installed using command `npm install --global --production --add-python-to-path windows-build-tools`. To know more about this tool, see [this link.](https://github.com/felixrieseberg/windows-build-tools). **Note** For executing this command through `Windows Command Prompt` or `Windows PowerShell`, they should be ran in Administrator mode.
4. **On Linux:** `build-essential` package for C++ compilation & `Python 2.7`.

    4.1. **Udev rules:** To be able to communicate with Jabra devices with non-root privileges it is required to create a udev rule for Jabra devices. Place the udev rule in `/etc/udev/rules.d`, and follow naming guidelines for udev files.              
    
    **Example:** The file name should be something like `80-jabra.rules` where the number before the dash indicates in what order the system reads the rules (e.g. if you had another rules file called `70-someotherrule.rules`, it would read that one first), the word after the dash is just an identifier of sorts (it could be something other than jabra) and the extension must be `.rules`.

    The contents of the file are:

    `ATTRS{idVendor}=="0b0e", MODE="0666", GROUP="users"`

    After creating the udev file (as root), reload the udev rules using:

    `sudo udevadm control --reload`

    Reattach your Jabra device in order to get new permissions assigned.

5. **All Platforms:** `node-gyp`. You can install this using command `npm install -g node-gyp`. To know more about this, see [this link](https://www.npmjs.com/package/node-gyp)

6. **For Electron.JS:** If you are using `asar` packaging then you may need to `unpack` some of the resources used in this module. These resources are native library files i.e `libjabra.dll`, `libjabra.dylib` & `libjabra.so`, which is stored in a `build\Release` folder. By default latest electron builder will automatically unpack, but if it does not work then you can provide below option to your build process. To know more, see [this link](https://www.electron.build/configuration/configuration)

    ```
    "build": {
        "asarUnpack": ["node_modules/@gnaudio/jabra-node-sdk"]
    }
    ```

## Installation
This release is published to [npm](https://www.npmjs.com/package/@gnaudio/jabra-node-sdk) and can be installed via below command.
```
npm install --save @gnaudio/jabra-node-sdk
```

## Debugging and Logging
Below environment variables are defined for logging and debugging purpose. User can change the values as per preference.

Environment Variable | Value | Description
--- | --- | ---
LIBJABRA_TRACE_LEVEL | fatal, error, warning(default), info, debug | Log levels
LIBJABRA_RESOURCE_PATH | **On Mac:** ~/Library/Application Support/JabraSDK/ **On Windows:** %appdata%/Roaming/JabraSDK  | This determine the system path where logs and device related files are written.

## API Reference
API doc is in html format. See doc folder inside installed module `node_modules\@gnaudio\jabra-node-sdk\dist\doc` and open `index.html`.

## Examples

For all examples, user should first register the app on [Jabra developer site](https://developer.jabra.com/) to get appID. User should pass this appID to `createJabraApplication` in order to initialize the jabra module:

### Simple button events example using javascript and plain promises

```javascript
const j = require("@gnaudio/jabra-node-sdk");
j.createJabraApplication('123').then((jabra) => { // 123 is appID here
    jabra.on('attach', (device) => {
        console.log('Press any key on Jabra device ' + device.deviceName);
        
        device.on('btnPress', (btnType, btnValue) => {
          console.log('New input from device is received: ', j.enumDeviceBtnType[btnType], btnValue);
        });
    });    
});
```

### Simple button events example using typescript and plain promises

```typescript
import { createJabraApplication, enumDeviceBtnType, enumHidState } from '@gnaudio/jabra-node-sdk';
createJabraApplication('123').then((jabra) => { // 123 is appID here
    jabra.on('attach', (device) => {
        console.log('Press any key on Jabra device ' + device.deviceName);

        device.on('btnPress', (btnType, btnValue) => {
          console.log('New input from device is received: ', enumDeviceBtnType[btnType], btnValue);
        });
    });    
});
```

### Ring example using typescript and plain promises
This example shows how to ring a Jabra device

```typescript
import { createJabraApplication, JabraError } from '@gnaudio/jabra-node-sdk';

createJabraApplication('123').then((jabra) => { // 123 is appID here
    jabra.on('attach', (device) => {
        device.isRingerSupportedAsync().then( (supported) => {
            if (supported) {
              device.ringAsync().then ( () => {
                console.log('ringing');
              }).catch ((err: JabraError) => {
                console.log('ring failed with error ' + err);
              }); // ring the device
              setTimeout(() => {
                device.unringAsync().then(() => {
                    console.log('stopped ringing');
                }).catch ((err: JabraError) => {
                    console.log('unring failed with error ' + err);
                });
              }, 5000); // stop ringing the device after 5 second
            }          
        }).catch( (err: JabraError) => {
            console.error('Jabra call failed with error ' + err)
        });
    });
    
    jabra.on('detach', (device) => {
        console.log('Device detached with deviceID:', device.deviceID);

        // In this demo example, we will auto shutdown once all jabra devices are removed:
        let remainingAttachedDevices = jabra.getAttachedDevices();
        if (remainingAttachedDevices.length == 0) {
            jabra.disposeAsync().then (() => {}); // Cleanup and allow node process to exit.
        }
    });
});
```

### Multiple device management with typescript and async/await

```typescript
import { createJabraApplication } from '@gnaudio/jabra-node-sdk';

(async () => {
    let jabra = await createJabraApplication('123'); // 123 is appID here

    await jabra.scanForDevicesDoneAsync(); // Wait for all pre-attached devices to be scanned.

    const deviceInstanceList = jabra.getAttachedDevices();
    if (deviceInstanceList.length<2) {
        throw new Error('Please make sure 2 jabra devices are attached');
    }

    const firstDevice = deviceInstanceList[0];
    await firstDevice.ringAsync();

    const secondDevice = deviceInstanceList[1];
    await secondDevice.ringAsync();

    // Dispose jabra sdk to enable node process to shutdown.
    await jabra.disposeAsync();
})();
```

## Sequence diagrams

These sequence diagrams shows typical use of the Node.js sdk:

![Sequence diagram](https://github.com/gnaudio/gnaudio.github.io/raw/master/jabra-node-sdk/docs/outgoing-call-then-end-call.png)

![Sequence diagram](https://github.com/gnaudio/gnaudio.github.io/raw/master/jabra-node-sdk/docs/incoming-call-then-accept-on-device-then-end-call.png)

![Sequence diagram](https://github.com/gnaudio/gnaudio.github.io/raw/master/jabra-node-sdk/docs/incoming-call-then-user-rejects.png)

![Sequence diagram](https://github.com/gnaudio/gnaudio.github.io/raw/master/jabra-node-sdk/docs/mute-unmute-from-device.png)

![Sequence diagram](https://github.com/gnaudio/gnaudio.github.io/raw/master/jabra-node-sdk/docs/hold-resume-from-device.png)

## Setup VSCode

The project includes config files for debugging typescript and C++ source files
in [Visual Studio Code](https://code.visualstudio.com/) via the
[C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools),
for both Windows and Linux. The configuration files work without any
modification, provided that you set up your environment first, by installing
the necessary tools and defining some environment variable.

To debug a C++ file, open a typescript file referencing the C++ file and start
the debugger (Running the debugger on C++ files directly won't work). Make sure
the "Run"-dropdown at the top of the debugging-sidebar is set to "Current TS
File - native code only (&lt;your operating system&gt;)"

Below, the setup steps for Windows and Linux.

### Windows
You can more or less follow the instruction in
[the VSCode C++ extension guide](https://code.visualstudio.com/docs/cpp/config-msvc).
Then, you need to make sure all project [pre-requisites](#pre-requisite) are
installed correctly, and finally set the following environment variables:

Environment Variable | Value | Description
--- | --- | ---
LIBJABRA_WIN_NODE_GYP_CACHE_PATH | eg. %appdata%\Local\node-gyp\Cache\12.16.3\include\node | Used by the C/C++ extension to resolve include paths
LIBJABRA_WIN_CPP_COMPILER_PATH | eg. C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.25.28610/bin/Hostx64/x64/cl.exe | Used by the C/C++ extension to infer the path to the C++ standard library header files


### Linux
You can read a more detailed guide on
[the VSCode C++ extension page](https://code.visualstudio.com/docs/cpp/config-linux).
The important part is to install a C++ compiler and debugger. The easiest way
is to install `build-essential` and `gdb`, this way:

```sh
sudo apt-get install build-essential gdb
```

This maks your C++ compiler GCC, and you would likely use the `g++` command to
compile.

Then, you need to install the project [pre-requisites](#pre-requisite), and
finally export the following environment variables:

Environment Variable | Likely value | Description
--- | --- | ---
JABRA_NODESDK_CPP_COMPILER_PATH | /usr/bin/g++ | Used by the C/C++ extension to infer the path to the C++ standard library header files
JABRA_NODESDK_NODE_GYP_CACHE_PATH | $HOME/.cache/node-gyp/14.4.0 | Used by the C/C++ extension to resolve include paths. **NOTE**: The final `include/node` is added by the configuration and it's not necessary
