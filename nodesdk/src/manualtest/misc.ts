import readline = require('readline');
import util = require('util');

import { createJabraApplication, DeviceType, JabraType, jabraEnums, enumHidState, enumWizardMode, enumSecureConnectionMode, JabraError, AudioFileFormatEnum, DeviceTypeCallbacks, PanTilt, enumPTZPreset, enumColorControlPreset, DateTime } from '../main/index';

let reserved1 = {
  proxy: "this.httpProxyService.getProxy()",
  blockAllNetworkAccess: false,
  baseUrl_capabilities: "this.appConfig.jabraConfiguration.manifestEndpoint",
  baseUrl_fw: "this.appConfig.jabraConfiguration.firmwareEndpoint"
};

const read = new Proxy(readline.createInterface({
  input: process.stdin,
  output: process.stdout
}), {
  get: function(original :any, prop: any, proxy: any) {
    if (prop != 'question') {
      return Reflect.get(original, prop, proxy);
    }

    return (function(text: string) {
      return new Promise(resolve => original.question(text, resolve));
    }).bind(original);
  }
});

const appOperations :Array<{description: string, operation: (app :JabraType) => Promise<any>}> = [
  {
    description: 'Get SDK version',
    operation: jabra =>
      jabra.getSDKVersionAsync()
        .then(v => console.log("SDK version is '" + v + "'"))
        .catch(err => {
          console.error("'get sdk version failed : " + err);
          console.log('get sdk version failed with error code : ' + err.code || "undefined");
        })
  },
  {
    description: 'Get error string',
    operation: jabra =>
      jabra.getErrorStringAsync(8)
        .then(r => console.log("getErrorStringAsync success with result " + r))
        .catch(err => console.log("getErrorStringAsync failed with error " + err))
  },
  {
    description: 'SyncExperiment',
    // let's pretend this is asynchronous
    operation: jabra =>
      new Promise((resolve, reject) => {
        let r = jabra._SyncExperiment(0);
        console.log("_SyncExperiment returned '" + JSON.stringify(r, null, 3) + "'");
        resolve();
      })
  }
];

const deviceOperations :Array<{description: string, operation: (app :DeviceType) => Promise<any>}>= [
  {
    description: 'HID APIs',
    operation: async device => {
      try {
        await device.setHidWorkingStateAsync(enumHidState.GN_HID);
        console.info('setHidWorkingStateAsync returned');
      } catch (err) {
        console.error("setHidWorkingStateAsync failed with error " + err);
      }

      try {
        const result = await device.isGnHidStdHidSupportedAsync();
        console.log("isGnHidStdHidSupportedAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed isGnHidStdHidSupportedAsync with error: " + err);
      }

      try {
        var result = await device.getHidWorkingStateAsync();
        console.log("getHidWorkingStateAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getHidWorkingStateAsync with error: " + err);
      }
    }
  },
  {
    description: 'Devlog APIs',
    operation: async device => {
      // console.log('Subscribing to devlog events');
      // device.on('onDevLogEvent', (log: string) => {
      //   console.log('New dev log event received for my device ' + device.deviceID + ' : ', log);
      // });

      try {
        await device.enableDevLogAsync(true);
        console.log('Dev log enabling succeded');
      } catch (err) {
        console.log('Dev log enabling failed with error: ' + err);
      }
    }
  },
  {
    description: 'Firmware APIs',
    operation: async device => {
      try {
        let r = await device.checkForFirmwareUpdateAsync();
        console.log("checkForFirmwareUpdateAsync success with result " + r);
      } catch (err) {
        console.error("checkForFirmwareUpdateAsync failed with error " + err);
      }

      try {
        const result = await device.isFirmwareLockEnabledAsync();
        console.log("isFirmwareLockEnabledAsync returns " + result);
      } catch (err) {
        console.error("Failed isFirmwareLockEnabledAsync with error: " + err);
      }

      try {
        const result = await device.getFirmwareVersionAsync();
        console.log("Device firmware version is " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getting firmware version with error: " + err);
      }

      try {
        const result = await device.getLatestFirmwareInformationAsync();
        console.log("Latest device firmware version is " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getLatestFirmwareInformationAsync with error: " + err);
      }

      try {
        await device.enableFirmwareLockAsync(true);
        console.log('Enable firmware lock succeded');
      } catch (err) {
        console.log('Enable firmware lock failed with error: ' + err);
      }

      try {
        const r = await device.isFirmwareLockEnabledAsync();
        console.log('Is firmware lock enabled succeded ' + r);
      } catch (err) {
        console.log('Is firmware lock enabled failed with error: ' + err);
      }

      try {
        await device.enableFirmwareLockAsync(false);
        console.log('Enable firmware lock succeded');
      } catch (err) {
        console.log('Enable firmware lock failed with error: ' + err);
      }

      try {
        const r = await device.isFirmwareLockEnabledAsync();
        console.log('Is firmware lock enabled succeded ' + r);
      } catch (err) {
        console.log('Is firmware lock enabled failed with error: ' + err);
      }
    }
  },
  {
    description: 'Connection APIs',
    operation: async device => {
      try {
        await device.connectBTDeviceAsync();
        console.log("connectBTDeviceAsync returned");

      } catch (err) {
        console.error("Failed searconnectBTDevicconnectBTDeviceAsynceAsyncchNewDevicesAsync with error: " + err);
      }

      try {
        var r = await device.getSecureConnectionModeAsync();
        console.log("getSecureConnectionModeAsync success with result " + r);
      } catch (err) {
        console.error("getSecureConnectionModeAsync failed with error " + err);
      }

      try {
        await device.connectNewDeviceAsync("myname", "010AFF000F07", true);
        console.info('connectNewDeviceAsync returned');
      } catch (err) {
        console.error('connectNewDeviceAsync failed with error ' + err);
      }

      try {
        await device.searchNewDevicesAsync();
        console.log("searchNewDevicesAsync returned");
      } catch (err) {
        console.error("Failed searchNewDevicesAsync with error: " + err);
      }

      if (device.isDongleDevice) {
        console.log('Subscribing to pairing list change');
        device.on('onBTParingListChange', (pairedListInfo)  => {
          console.log('New pairing list for my device ' + JSON.stringify(pairedListInfo, null, 3));
        });

        try {
          const deviceList = await device.getPairingListAsync();
          console.log('getPairingListAsync: ', deviceList);
        } catch (err) {
          console.error('getPairingListAsync failed with error ' + err);
        }

        try {
          await device.clearPairedDeviceAsync('Jabra EVOLVE 65', '501aa56d87ab', false);
          console.log('clearPairedDeviceAsync successful');

          try {
            let deviceList = await device.getPairingListAsync();
            console.log('getPairingListAsync new: ', deviceList);
          } catch (err) {
            console.log('getPairingListAsync new failed with error: ' + err);
          }
        } catch (err) {
          console.error('clearPairedDeviceAsync failed with error ' + err);
        }
      }
    }
  },
  {
    description: 'Call control APIs',
    operation: async device => {
      try {
        var result = await device.isHoldSupportedAsync();
        console.log("isHoldSupportedAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed isHoldSupportedAsync with error: " + err);
      }

      try {
        await device.holdAsync();
        console.log('holdAsync returned');
      } catch (err) {
        console.error('holdAsync failed with error: ' + err);
      }

      try {
        await device.resumeAsync();
        console.log('resumeAsync returned');
      } catch (err) {
        console.error('resumeAsync failed with error: ' + err);
      }

      try {
        const result = await device.isMuteSupportedAsync();
        console.log("isMuteSupportedAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed isMuteSupportedAsync with error: " + err);
      }
    }
  },
  {
    description: 'Setting APIs',
    operation: async device => {
      try {
        const result = await device.isSettingProtectionEnabledAsync();
        console.log("isSettingProtectionEnabledAsync returns " + result);
      } catch (err) {
        console.error("Failed isSettingProtectionEnabledAsync with error: " + err);
      }

      try {
        const result = await device.getSettingsAsync();
        console.log('getSettings succeded with ' + JSON.stringify(result, null, 3));
      } catch (err) {
        console.error('getSettings failed with error: ' + err);
        console.error('getSettings failed with error code : ' + err.code || "undefined");
      }

      try {
        // D1BF2202-4AA1-4B2C-8D3C-6CF7D3EE072F (list key values)
        console.log('Getting key values settings');
        const result = await device.getSettingAsync("D1BF2202-4AA1-4B2C-8D3C-6CF7D3EE072F");
        console.log('getSetting succeded with ' + JSON.stringify(result, null, 3));

        try {
          console.log('Setting the previously retrieved setting');
          const setResult = await device.setSettingsAsync(result);
          console.log('setSettingsAsync succeded with ' + JSON.stringify(setResult, null, 3));
        } catch (err) {
          console.log('setSettingsAsync failed with error: ' + err);
          console.log('setSettingsAsync failed with error code : ' + err.code || "undefined");
        }
      } catch (err) {
        console.error('getSetting failed with error: ' + err);
        console.error('getSetting failed with error code : ' + err.code || "undefined");
      }

      try {
        const result = await device.getSettingAsync("D1BF2202-4AA1-4B2C-8D3C-6CF7D3EE072F");
        console.log('2nd getSetting succeded with ' + JSON.stringify(result, null, 3));
      } catch (err) {
        console.error('2nd getSettingsAsync failed with error: ' + err);
        console.error('2nd getSettingsAsync failed with error code : ' + err.code || "undefined");
      }

      try {
        const result = await device.isFactoryResetSupportedAsync();
        console.log('isFactoryResetSupportedAsync succeded with ' + result);
      } catch (err) {
          console.log('isFactoryResetSupportedAsync failed with error: ' + err);
          console.log('isFactoryResetSupportedAsync failed with error code : ' + err.code || "undefined");
      }
    }
  },
  {
    description: 'Feature querying APIs',
    operation: async device => {
      try {
        const v = await device.getSupportedFeaturesAsync();
        console.log("getSupportedFeaturesAsync returned " + JSON.stringify(v, null, 2));
      } catch (err) {
        console.error("Failed getSupportedFeaturesAsync with error: " + err);
      }

      try {
        const result = await device.isFeatureSupportedAsync(1001);
        console.log("isFeatureSupportedAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed isFeatureSupportedAsync with error: " + err);
      }
    }
  },
  {
    description: 'Battery APIs',
    operation: async device => {
      try {
        console.info("Subscribing to 'onBatteryStatusUpdate' events");
        device.on('onBatteryStatusUpdate', (levelInPercent, isCharging, isBatteryLow) => {
          console.info(`New battery status for my device ${ device.deviceID }: level == ${ levelInPercent }%, isCharging == ${ isCharging }, isBatteryLow == ${ isBatteryLow }`);
        });
      } catch (err) {
        console.error(err.toString());
      }

      try {
        let result = await device.isBatterySupportedAsync();
        console.log("isBatterySupportedAsync returns " + result);
      } catch (err) {
        console.error("Failed isBatterySupportedAsync with error: " + err);
      }

      try {
        let result = await device.getBatteryStatusAsync();
        console.log("getBatteryStatusAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getBatteryStatusAsync with error: " + err);
      }
    }
  },
  {
    description: 'Image APIs',
    operation: async device => {
      try {
        console.info("Subscribing to 'onUploadProgress' events");
        device.on('onUploadProgress', (status, levelInPercent)  => {
          console.log(`New upload status for my device ${ device.deviceID }: status == ${ status }, level == ${ levelInPercent }%`);
        });
      } catch (err) {
        console.error(err.toString());
      }

      try {
        const result = await device.getImagePathAsync();
        console.log("Latest ImagePath is " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getImagePathAsync with error: " + err);
      }

      try {
        const result = await device.getImageThumbnailPathAsync();
        console.log("Latest ImageThumbnailPath is " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getImageThumbnailPathAsync with error: " + err);
      }

      try {
        const result = await device.isUploadImageSupportedAsync();
        console.log("isUploadImageSupportedAsync returns " + result);
      } catch (err) {
        console.error("Failed isUploadImageSupportedAsync with error: " + err);
      }

      try {
        await device.uploadImageAsync("dummy.wav");
        console.log("uploadImageAsync success");
      } catch (err) {
        console.error("Failed uploadImageAsync with error: " + err);
      }

      try {
        await device.uploadImageAsync("dummyfilename.ext");
        await device.uploadImageAsync("dummyfilename.ext");
        console.log("uploadImageAsync success");
      } catch (err) {
        console.log("uploadImageAsync failed with error " + err);
      }
    }
  },
  {
    description: 'Device data APIs',
    operation: async device => {
      try {
        const result = await device.getNamedAssetAsyngetNamec("HERO_IMAGE");
        console.log("getNamedAssetAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed getNamedAssetAsync with error: " + err);
      }

      try {
        const deviceName = await device.getConnectedBTDeviceNameAsync();
        console.log('getConnectedBTDeviceName: ', deviceName);
      } catch (err) {
        console.error('getConnectedBTDeviceNameAsync failed with error: ' + err);
      }

      try {
        const result = await device.getSerialNumberAsync();
        console.log("getSerialNumberAsync returns " + result);
      } catch (err) {
        console.error("Failed getSerialNumberAsync with error: " + err);
      }

      try {
        const result = await device.getPanicsAsync();
        console.log("getPanicsAsync returns " + util.inspect(result));
      } catch (err) {
        console.error("Failed getPanicsAsync with error: " + err);
      }
    }
  },
  {
    description: 'Button APIs',
    operation: async device => {
      try {
        const result = await device.getSupportedButtonEventsAsync();
        console.log("getSupportedButtonEventsAsync returns " + util.inspect(result));
      } catch (err) {
        console.error("Failed getSupportedButtonEventsAsync with error: " + err);
      }

      console.log('Subscribing to button events');
      device.on('btnPress', (btnType: number, value: boolean) => {
        console.log('New input from device is received: ', jabraEnums.enumDeviceBtnType[btnType], value);
      });
    }
  },
  {
    description: 'Equalizer APIs',
    operation: async device => {
      try {
        var params = await device.getEqualizerParametersAsync();
        console.info("getEqualizerParametersAsync returned " + JSON.stringify(params, null, 2))
      } catch (err) {
        console.error(err.toString());
      }

      try {
        await device.setEqualizerParametersAsync([1, 2, 3]);
        console.info('setEqualizerParametersAsync returned')
      } catch (err) {
        console.error(err.toString());
      }

      try {
        var isSupported = await device.isEqualizerSupportedAsync();
        console.info(`Equalizer is ${ isSupported ? '' : 'not' } supported`);
      } catch (err) {
        console.error(err.toString());
      }

      try {
        await device.enableEqualizerAsync(true);
        console.info('enableEqializerAsync returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        var isEnabled = await device.isEqualizerEnabledAsync();
        console.info(`Equalizer is ${ isEnabled ? '' : 'not' } enabled`);
      } catch (err) {
        console.error(err.toString());
      }
    }
  },
  {
    description: 'Busylight APIs',
    operation: async device => {
      try {
        var result = await device.isBusyLightSupportedAsync();
        console.log('isBusyLightSupportedAsync: ', result);
      } catch (err) {
        console.error('isBusyLightSupportedAsync failed with error: ' + err);
      }

      try {
        await device.setBusyLightStatusAsync(true);
        console.log('setBusyLightStatusAsync returned');
      } catch (err) {
        console.error('setBusyLightStatusAsync failed with error ' + err);
      }

      try {
        var issupported = await device.getBusyLightStatusAsync();
        console.log('getBusyLightStatusAsync: ', issupported);
      } catch (err) {
        console.error('getBusyLightStatusAsync failed with error ' + err);
      }
    }
  },
  {
    description: 'Date-time APIs',
    operation: async device => {
      try {
        var result = await device.isSetDateTimeSupportedAsync();
        console.log("isSetDateTimeSupportedAsync returns " + JSON.stringify(result));
      } catch (err) {
        console.error("Failed isSetDateTimeSupportedAsync with error: " + err);
      }

      try {
        const newDate = new Date();
        const dt : DateTime = {sec: newDate.getSeconds(),
           min: newDate.getMinutes(),
           hour: newDate.getHours(),
           mon: newDate.getMonth(),
           year: newDate.getFullYear()-1900,
           mday: newDate.getDate(),
           wday: newDate.getDay() };
        await device.setDateTimeAsync(dt);
        console.log("setDateTimeAsync succeded ");
      } catch (err) {
        console.error("setDateTimeAsync failed with error " + err);
      }

      try {
        const n: DateTime = await device.getDateTimeAsync();
        console.log("getDateTimeAsync returned " + n.toString());
      } catch (err) {
        console.error("getDateTimeAsync failed with error " + err);
      }

      try {
        const newDate = new Date().getTime();
        await device.setTimestampAsync(newDate / 1000);
        console.log("setTimestampAsync succeded ");
      } catch (err) {
        console.error("Failed setTimestampAsync with error: " + err);
      }

      try {
        const n = await device.getTimestampAsync();
        console.log("getTimestampAsync returned " + n);
      } catch (err) {
        console.log("getTimestampAsync failed with error " + err);
      }
    }
  },
  {
    description: 'Ringtone APIs',
    operation: async device => {
      try {
        const result = await device.getAudioFileParametersForUploadAsync();
        if (result.audioFileType != AudioFileFormatEnum.AUDIO_FILE_FORMAT_NOT_USED) {
          console.log("getAudioFileParametersForUploadAsync returns " + JSON.stringify(result, null, 3));
        }
      } catch (err) {
        console.error("Failed getAudioFileParametersForUploadAsync with error: " + err);
      }

      try {
        const result = await device.isUploadRingtoneSupportedAsync();
        console.log("isUploadRingtoneSupportedAsync returns " + result);
      } catch (err) {
        console.error("Failed isUploadRingtoneSupportedAsync with error: " + err);
      }

      try {
        await device.uploadWavRingtoneAsync("dummy.wav");
        console.log("uploadWavRingtoneAsync success");
      } catch (err) {
        console.error("Failed uploadWavRingtoneAsync with error: " + err);
      }

      try {
        await device.uploadRingtoneAsync("dummy.notwav");
        console.log("uploadRingtoneAsync success");
      } catch (err) {
        console.error("Failed uploadRingtoneAsync with error: " + err);
      }
    }
  },
  {
    description: 'Get Failed Setting Names',
    operation: device =>
      device.getFailedSettingNamesAsync()
        .then(r => console.log("getFailedSettingNamesAsync success with result " + JSON.stringify(r, null, 3)))
        .catch(err => console.error("getFailedSettingNamesAsync failed with error " + err))
  },
  {
    description: 'Reboot Device',
    operation: device =>
      device.rebootDeviceAsync()
        .then(r => console.log("rebootDeviceAsync success with result " + r))
        .catch(err => console.error("rebootDeviceAsync failed with error " + err))
  },
  {
    description: 'Get Button Focus',
    operation: device =>
      device.getButtonFocusAsync([
          {
            buttonTypeKey: 27,
            buttonTypeValue: "hej",
            buttonEventType: [{key: 42, value: "bla"}]
          }
        ])
        .then(result => console.log("getButtonFocusAsync returned " + JSON.stringify(result, null, 2)))
        .catch(err => console.error("getButtonFocusAsync failed with error " + err))
  },
  {
    description: 'Whiteboard APIs',
    operation: async device => {
      try {
        await device.setWhiteboardPositionAsync(0, {
          lowerLeftCorner: {x: 0, y: 1},
          upperLeftCorner: {x: 2, y: 3},
          upperRightCorner: {x: 4, y: 5},
          lowerRightCorner: {x: 6, y: 7}
        });
        console.info('setWhiteboardPositionAsync returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        const whiteboard = await device.getWhiteboardPositionAsync(0);
        console.info(`Whiteboard returned: ${ util.inspect(whiteboard) }`);
      } catch (err) {
        console.error(err.toString());
      }
    }
  },
  {
    description: 'Zoom APIs',
    operation: async device => {
      try {
        await device.setZoomAsync(1.9);
        console.info('setZoom returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        let zoom = await device.getZoomAsync();
        console.info(`Zoom: ${ zoom }`);
      } catch (err) {
        console.error(err.toString());
      }

      try {
        let limits = await device.getZoomLimitsAsync();
        console.info(`Zoom limits: ${ util.inspect(limits) }`);
      } catch (err) {
        console.error(err.toString());
      }
    }
  },
  {
    description: 'Pan-Tilt APIs',
    operation: async device => {
      try {
        await device.setPanTiltAsync({pan: 2500, tilt: -1200});
        console.info('setPanTilt returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        let { pan, tilt } = await device.getPanTiltAsync();
        console.info(`Pan: ${ pan }`);
        console.info(`Tilt: ${ tilt }`);
      } catch (err) {
        console.error(err.toString());
      }

      try {
        let limits = await device.getPanTiltLimitsAsync();
        console.info(`PanTilt limits: ${ util.inspect(limits) }`);
      } catch (err) {
        console.error(err.toString());
      }

      // try {
      //   let limits = await device.getZoomLimitsAsync();
      //   console.info(`Zoom limits: ${ util.inspect(limits) }`);
      // } catch (err) {
      //   console.error(err.toString());
      // }
    }
  },
  {
    description: 'Preset APIs',
    operation: async device => {
      try {
        await device.applyPTZPresetAsync(enumPTZPreset.PRESET1);
        console.info('applyPTZPreset returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        await device.storePTZPresetAsync(enumPTZPreset.PRESET2);
        console.info('storePTZPreset returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        await device.applyColorControlPresetAsync(enumColorControlPreset.PRESET1);
        console.info('applyColorControlPreset returned');
      } catch (err) {
        console.error(err.toString());
      }

      try {
        await device.storeColorControlPresetAsync(enumColorControlPreset.PRESET1);
        console.info('storeColorControlPreset returned');
      } catch (err) {
        console.error(err.toString());
      }
    }
  },
];

(async () => {
  try {
    /**************************************
     * Creating Jabra app
     **************************************/
    let jabra = await createJabraApplication('A7tSsfD42VenLagL2mM6i2f0VafP/842cbuPCnC+uE8=', {}, true)
    console.log("!! Jabra initialized correctly !!");

    // The device operation to execute for every device
    var chosenDeviceOperation :((app :DeviceType) => Promise<any>) | null = null;

    // The nodejs callback that uses the device
    var deviceCallback = async (device: DeviceType) => {
      console.info(`Device attached: ${ device.deviceName }`);
      console.log(`Device: ${ JSON.stringify(device) }`);

      if (chosenDeviceOperation) {
        await chosenDeviceOperation(device);
      }
    };

    /**************************************
     * Setting up listeners
     **************************************/
    jabra.on('attach', deviceCallback);
    jabra.on('detach', device => {
        console.log('Device detached with device: ', JSON.stringify(device, null, 2));
        jabra.disposeAsync();
    });
    jabra.on('firstScanDone', () => console.log('First scan done'));

    /**************************************
     * Prompting for non-device operation
     **************************************/
    console.info('Any non-device API to test?');
    console.info('0. None');
    for (let [index, operation] of appOperations.entries()) {
      console.info(`${ index + 1 }. ${ operation.description }`);
    }

    var selectedNumber = await read.question('Enter a number: ');
    var index = parseInt(selectedNumber.trim());
    if (index !== 0) {
      const operation = appOperations[index - 1];
      if (!operation) {
        throw new Error(`Unknown operation: ${ index }`);
      }

      await operation.operation(jabra);
    }

    /**************************************
     * Prompting for device operation
     **************************************/
    console.info('Any device API to test?');
    console.info('0. None');
    for (let [index, operation] of deviceOperations.entries()) {
      console.info(`${ index + 1 }. ${ operation.description }`);
    }

    selectedNumber = await read.question('Enter a number: ');
    index = parseInt(selectedNumber.trim());
    if (index !== 0) {
      const operation = deviceOperations[index - 1];
      if (!operation) {
        throw new Error(`Unknown operation: ${ index }`);
      }
      chosenDeviceOperation = operation.operation;
    }

    /**************************************
     * Executing device operation on already connected devices
     **************************************/
    await Promise.all(jabra.getAttachedDevices().map(deviceCallback));

    console.info('Setup complete - waiting (enter twice Ctrl + C to exit)');
  } catch (err) {
    console.error("Got exception err " + err);
    console.log('get exception error code : ' + err.code || "undefined");
  }
})();
