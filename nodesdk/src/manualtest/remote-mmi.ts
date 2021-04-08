import {
    createJabraApplication, DeviceType, JabraType, jabraEnums,
    _getJabraApiMetaSync, _JabraNativeAddonLog, AddonLogSeverity, 
    enumRemoteMmiType, enumRemoteMmiInput, enumRemoteMmiPriority, enumRemoteMmiSequence
} from '../main/index';

(async () => {
    try {
        let jabra = await createJabraApplication('A7tSsfD42VenLagL2mM6i2f0VafP/842cbuPCnC+uE8=')

        let meta = _getJabraApiMetaSync();
        // console.log("Got Jabra meta " + JSON.stringify(meta, null, 2));

        _JabraNativeAddonLog(AddonLogSeverity.info, "test2", "this is a test");

        jabra.getSDKVersionAsync().then(v => {
            console.log("SDK version is '" + v + "'");
        }).catch(err => {
            console.error("'get sdk version failed : " + err);
            console.log('get sdk version failed with error code : ' + err.code || "undefined"); 
        });

        jabra.on('attach', async (device: DeviceType) => {
            if (device.deviceName !== "Jabra Engage 50") {
                device.on('onRemoteMmiEvent', async (type, input) => {
                    console.log('onRemoteMmiEvent: ', type, input);
                    await device.releaseRemoteMmiFocusAsync(enumRemoteMmiType.MMI_TYPE_DOT3).catch(err => console.log(err)); 
                });     
          
                await device.getRemoteMmiFocusAsync(
                    enumRemoteMmiType.MMI_TYPE_DOT3, 
                    255, 
                    enumRemoteMmiPriority.MMI_PRIORITY_HIGH
                ).catch(err => console.log(err));
                        
                let isInFocus = await device.isRemoteMmiInFocusaAsync(enumRemoteMmiType.MMI_TYPE_DOT3).catch(err => console.log(err));
                console.log('isInFocus', isInFocus)
                
                let audioActionOutput = { 
                    red: 0, 
                    green: 0,
                    blue: 100, 
                    sequence: enumRemoteMmiSequence.MMI_LED_SEQUENCE_FAST 
                }

                await device.setRemoteMmiActionAsync(enumRemoteMmiType.MMI_TYPE_DOT3, audioActionOutput).catch(err => console.log(err));              
            }        
        });

        jabra.on('detach', (device: DeviceType) => {
            console.log('Device detached with device ', JSON.stringify(device));
            jabra.disposeAsync();
        });
    } catch (err) {
        console.error("Got exception err " + err);
        console.log('get exception error code : ' + err.code || "undefined"); 
    }
})();
