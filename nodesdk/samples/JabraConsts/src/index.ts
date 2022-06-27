import * as jabraNodeSdk from "@gnaudio/jabra-node-sdk";
import { argv } from "process";

let listIndent : number = 0;

function printValueWithType(val : jabraNodeSdk.DeviceConstants, idx : number | undefined = undefined) : void {
    let prefix : string = "\t".repeat(listIndent);
    if (typeof idx !== "undefined") prefix = prefix.concat("#", idx.toString(), ": ");
    let typeStr : string = "";
    let valStr : string | undefined = val.getValue()?.toString();

    if (typeof(val.getValue()) !== "undefined") typeStr = typeof(val.getValue());
    else typeStr = "struct";
    if (val.asArray()) {
        typeStr = "list";
        valStr = val.asArray()?.length.toString() + " constants";
    }

    console.log(prefix + valStr + " (" + typeStr + ")");
}

function printdeviceConstantArray(consts : jabraNodeSdk.DeviceConstants[]) : void {
    let idx : number = 0;
    consts.forEach(element => {
        printValueWithType(element, idx);
        let subArray : jabraNodeSdk.DeviceConstants[] | undefined = element.asArray();
        if (subArray)
        {
            listIndent++;
            printdeviceConstantArray(subArray);
            listIndent--;
        }
        idx++;
    });
}

async function printdeviceConstant(dev : jabraNodeSdk.DeviceType, path : string) : Promise<void> {
    await dev.getConstantsAsync(path)
    .then((constant: jabraNodeSdk.DeviceConstants) => {
        console.log(path + ":");

        let arr = constant.asArray();
        if (arr)
            printdeviceConstantArray(arr);
        else
            printValueWithType(constant);
    })
    .catch((err) => {
        console.error(err);
    })
}

function listDevices(devices: jabraNodeSdk.DeviceType[]) : void
{
    console.log('Device list:');
    devices.forEach((device, index) => {
        console.log(index + ': ' + device.ESN + ', ' + device.deviceName);
    });
}

//////////////
// Main program

start();
async function start(): Promise<void> {
    let jabra: jabraNodeSdk.JabraType;
    let devices : jabraNodeSdk.DeviceType[] = [];
    let jabraSDKConfig : jabraNodeSdk.ConfigParamsCloud = {};
    jabraSDKConfig.blockAllNetworkAccess = true;
    jabra = await jabraNodeSdk.createJabraApplication("A7tSsfD42VenLagL2mM6i2f0VafP/842cbuPCnC+uE8=", jabraSDKConfig);

    console.log("SDKVersion", await jabra.getSDKVersionAsync());
    console.log("Scanning for devices..\n");
    await jabra.scanForDevicesDoneAsync();
    devices = jabra.getAttachedDevices();

    if (argv.length < 4)
    {
        listDevices(devices);
        console.log("\nUsage: npm start <deviceName> <constantName>");
        console.log("Example: npm start \"Jabra PanaCast 50\" \"video-resolution/width\"\n");
        await jabra.disposeAsync();
        process.exit();
        }

    const deviceName : string = argv[2];
    const constantName : string = argv[3];

    let selectedDev : jabraNodeSdk.DeviceType | undefined;
    devices.forEach(dev => {
        if (deviceName == dev.deviceName) selectedDev = dev;
    });
    if (typeof selectedDev === "undefined")
    {
        console.error("Not a valid device name.\n");
        await jabra.disposeAsync();
        process.exit();
    }
    console.log("Selected device: " + selectedDev.deviceName + "\n");
    await printdeviceConstant(selectedDev, constantName);

    await jabra.disposeAsync();
    process.exit();
}
