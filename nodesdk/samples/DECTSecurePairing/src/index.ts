import { ConfigParamsCloud, createJabraApplication, DeviceType, JabraType } from "@gnaudio/jabra-node-sdk";
const dectoperations = require('./dectoperations');
const ui = require('./ui');

//////////////
// Main program

ui.init();
start();
async function start(): Promise<void> {
    let jabra: JabraType;
    let devices : DeviceType[] = [];
    let jabraSDKConfig : ConfigParamsCloud = {};
    jabraSDKConfig.blockAllNetworkAccess = true;
    jabra = await createJabraApplication('A7tSsfD42VenLagL2mM6i2f0VafP/842cbuPCnC+uE8=', jabraSDKConfig);
    console.log('SDKVersion', await jabra.getSDKVersionAsync());

    let dongle : DeviceType, headset : DeviceType;

    console.log("Press 'q' to quit\n");
    console.log('Scanning for devices..\n')
    jabra.scanForDevicesDoneAsync()
    .then(() => {
        devices = jabra.getAttachedDevices();
        ui.listDevices(devices);
        return ui.selectDECTDongle(devices);
    })
    .then((dev : DeviceType) => {
        dongle = dev;
        return ui.selectDECTHeadset(devices);
    })
    .then((dev: DeviceType) => {
        headset = dev;
        console.log('\nAttempting to pair these devices:');
        console.log('Dongle: #' + dongle.deviceID + ': ' + dongle.deviceName);
        console.log('Headset: #' + headset.deviceID + ': ' + headset.deviceName + '\n');
        return dectoperations.pairDECTDevices(dongle, headset);
    })
    .then(() => {
        console.log('Pairing completed');
    })
    .catch((err) => {
        console.error('Pairing failed: ' + err);
    })
    .finally(() => {
        ui.emitKeypress(ui.enumKbPresses.Q);
    })

    await ui.waitForKeypress(ui.enumKbPresses.Q);
    console.log('\nClosing down...');
    await jabra.disposeAsync();
    process.exit();
}
