import { DeviceType } from "@gnaudio/jabra-node-sdk";
import { promisify } from "util";
import { emitKeypressEvents } from "readline";
import { EventEmitter } from "events";
const dectoperations = require('./dectoperations');

//////////////
// UI stuff
  
export enum enumKbPresses { ARROWDOWN, ARROWUP, P, Q, ENTER };
export const keypressEventName : string = 'kbEvent';
const emitter = new EventEmitter();

export async function init() : Promise<void> {
// We need to grab raw inputs from the keyboard
    if (process.stdin.isTTY) {
        process.stdin.setRawMode(true);
        emitKeypressEvents(process.stdin);
    }

    process.stdin.on('keypress', (str, key) => {
        if (key.code == '[A') {
            emitter.emit(keypressEventName, enumKbPresses.ARROWUP);
        }
        if (key.code == '[B') {
            emitter.emit(keypressEventName, enumKbPresses.ARROWDOWN);
        }
        if (str && str.toLowerCase() == 'p') {
            emitter.emit(keypressEventName, enumKbPresses.P);
        }
        if (str && str.toLowerCase() == 'q') {
            emitter.emit(keypressEventName, enumKbPresses.Q);
        }
        if (str == '\r') {
            emitter.emit(keypressEventName, enumKbPresses.ENTER);
        }
    });
}

export function listDevices(devices: DeviceType[]) : void
{
    let i : number = 0;
    console.log('Device list:');
    devices.forEach((device, index) => {
        console.log(index + ': ' + device.ESN + ', ' + device.deviceName);
    });
}

export async function selectDECTHeadset(devices: DeviceType[]) : Promise<DeviceType>
{
    let validDevices : DeviceType[] = [];
    for (const device of devices) {
        if (await dectoperations.isDECTHeadset(device)) validDevices.push(device);
    }
    return chooseDevice(validDevices, 'headset');
}

export async function selectDECTDongle(devices: DeviceType[]) : Promise<DeviceType>
{
    let validDevices : DeviceType[] = [];
    for (const device of devices) {
        if (await dectoperations.isDECTDongle(device)) validDevices.push(device);
    }
    return chooseDevice(validDevices, 'dongle');
}

/*
* Lets the user use the up/down arrow keys and enter to select a DeviceType object
*/
export async function chooseDevice(devices : DeviceType[], deviceName : string = 'device') : Promise<DeviceType> {
    if (devices.length == 0) throw new Error('No ' + deviceName + 's found');

    console.log('\nSelect ' + deviceName + ' using <up>/<down>/<enter> keys:');
    for (const device of devices) {
        console.log(device.deviceID + ': ' + device.deviceName);
    }

    let devIndex : number = 0;
    let selectionMade: boolean = false;

    const keypressHandler = (eventVal : number) => {
        if (eventVal == enumKbPresses.ARROWUP) if (devIndex > 0) devIndex--;
        if (eventVal == enumKbPresses.ARROWDOWN) if (devIndex < devices.length - 1) devIndex++;
        process.stdout.clearLine(0, () => {
            process.stdout.write('\rSelection: ' + devices[devIndex].deviceID + ': ' + devices[devIndex].deviceName);
            if (eventVal == enumKbPresses.ENTER) {
                selectionMade = true;
                process.stdout.write('\rSelected:  \n');
            }
        })
    }
    emitter.on(keypressEventName, keypressHandler);

    keypressHandler(-1);
    while(!selectionMade) await sleep(100);

    emitter.off(keypressEventName, keypressHandler);
    return devices[devIndex];
}

export async function sleep(ms: number) : Promise<void> {
    const _setTimeout = promisify(setTimeout);
    return _setTimeout(ms);
}

async function waitForEvent(eventName : string, value : number, checkIntervalms : number = 250) : Promise<void> {
    let terminate: boolean = false;
    function timetogo(eventVal : number) {
        if (eventVal == value) terminate = true;
    }
    emitter.on(eventName, timetogo);
    while(!terminate) await sleep(checkIntervalms)
    emitter.off(eventName, timetogo);
}

export async function emitKeypress(key : enumKbPresses) : Promise<void> {
    emitter.emit(keypressEventName, key);
}

export async function waitForKeypress(key : enumKbPresses) : Promise<void> {
    return waitForEvent(keypressEventName, key);
}
