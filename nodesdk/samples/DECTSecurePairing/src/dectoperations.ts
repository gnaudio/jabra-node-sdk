import { DeviceType, enumDeviceFeature, dongleConnectedHeadsetName } from "@gnaudio/jabra-node-sdk";

//////////////
// DECT hardware related functions

export async function isDECTDongle(device: DeviceType) : Promise<boolean>
{
    return await device.isFeatureSupportedAsync(enumDeviceFeature.DECTBasicPairing) &&
        await device.isFeatureSupportedAsync(enumDeviceFeature.DECTSecurePairing);
}

export async function isDECTHeadset(device: DeviceType) : Promise<boolean>
{
    let retVal : boolean = false;
    await device.setDECTPairingKeyAsync(0).then(() => {
        retVal = true;
    })
    .catch(() => {})
    return retVal;
}

export async function pairDECTDevices(dongle: DeviceType, headset: DeviceType) : Promise<void>
{
    if (dongle === undefined || headset === undefined) throw new Error('Need two devices for pairing');

    let batLevel : number = 0;
    return dongle.triggerDECTPairingSecureAsync()
    .then(() => {
        return dongle.getDECTPairingKeyAsync();
    })
    .then((pairingKey: number) => {
        console.log('Using pairing key: ' + pairingKey);
        return headset.setDECTPairingKeyAsync(pairingKey);
    })
    .then(() => {
        console.log('Starting secure pairing');
        return headset.triggerDECTPairingSecureAsync();
    })
    .then(() => {
        console.log('Waiting for headset battery level report');
        return waitForBatteryUpdateEvent(dongle);
    })
    .then((level: number) => {
        batLevel = level;
        if (level == 0) throw new Error("Pairing unsuccessful");
        return dongle.getConnectedHeadsetNamesAsync(true, { primary:true });
    })
    .then((name : dongleConnectedHeadsetName) => {
        if (name["primary"] == "") throw new Error("Pairing unsuccessful");
        console.log('Connected to headset \'' + name["primary"] + '\' (battery: ' + batLevel + '%)');
    })
}

async function waitForBatteryUpdateEvent(device: DeviceType, timeout: number = 30000) : Promise<number> {
    return new Promise((resolve, reject) => {
        const timer = setTimeout(() => {
            device.off('onBatteryStatusUpdate', (levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => {});
            reject("Timeout while waiting for battery event");
        }, timeout);

        let callback = (levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => {
            device.off('onBatteryStatusUpdate', (levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => {});
            clearTimeout(timer);
            resolve(levelInPercent);
        };

        device.on('onBatteryStatusUpdate', callback);
    })
}
