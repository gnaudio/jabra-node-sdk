import { SdkIntegration } from "./sdkintegration";
import { isNodeJs } from './util';

/** @internal */
let sdkIntegration: SdkIntegration;

// @ts-ignore
if (isNodeJs()) { 
    let bindings = require('bindings');
    sdkIntegration = bindings('sdkintegration');
}

// Example usage:
// let c = new DeviceConstant(0, "video-resolution").get("width")?.asNumber();
// or simply:
// let c = new DeviceConstant(0, "video-resolution/width").asNumber();
// 
// Example accessing list element:
// let c = new DeviceConstant(0, "video-resolution").get("width/sensor")?.get(1)?.asNumber();

const invalidRefKey : number = -1;
export class DeviceConstants
{
    private refKey : number;
    private deviceID : number;

    /**
     * Construct a DeviceConstant object seated at the root or sub node in the tree of constants.
     * Traverse directly to a sub node by specifying the full path using delimiters (default "/") between element names

     * @param deviceID ID of device to query
     * @param key Root node or full path to Jabra Constant
     * @param delimiter Optional - delimiter used if full path was specified
     * @returns DeviceConstant object
     */
    constructor(deviceID: number, key: string | number, delimiter : string = "/") {
        this.deviceID = deviceID;
        if (typeof key == "number")
        {
            this.refKey = key;                                              // sub node
            return;
        }

        let elements : string[] = key.split(delimiter);
        this.refKey = sdkIntegration.GetConstSync(deviceID, elements[0]) ?? invalidRefKey; // root node
        if (elements.length > 1)
        {
            elements.splice(0, 1);
            // Traverse sub nodes
            elements.forEach(element => {
                if (this.isValid())
                {
                    this.refKey = sdkIntegration.GetConstFieldSync(this.deviceID, this.refKey, element) ?? invalidRefKey;                    
                }
            });
        }
    };
    
    /**
     * Get number value
     * @returns Value at node if it is a number. Returns undefined if not a number.
     */
    asNumber() : number | undefined {
        return sdkIntegration.GetConstIntegerSync(this.deviceID, this.refKey);
    };
    /**
     * Get string value
     * @returns Value at node if it is a string. Returns undefined if not a string.
     */
    asString() : string | undefined {
        return sdkIntegration.GetConstStringSync(this.deviceID, this.refKey);
    };
    /**
     * Get boolean value
     * @returns Value at node if it is a boolean. Returns undefined if not a boolean.
     */
    asBoolean() : boolean | undefined {
        return sdkIntegration.GetConstBooleanSync(this.deviceID, this.refKey);
    };
    /**
     * Get list node as Array
     * @returns Array of DeviceConstants if current node points at a list. Returns undefined if not a list.
     */
    asArray() : DeviceConstants[] | undefined {
        let idx : number = 0;
        let retList : DeviceConstants[] = [];
        let listNode : DeviceConstants | undefined;
        listNode = this.get(idx++);
        while (listNode) {
            retList = retList.concat(listNode);
            listNode = this.get(idx++);
        };
        if (retList.length == 0) return undefined;
        return retList;
    };
    /**
     * Get value
     * @returns Value at node if it is a number, string or boolean. Returns an array of constants
     * if node points at a list. Returns undefined if neither.
     */
    getValue(): number | string | boolean | DeviceConstants[] | undefined {
        return this.asNumber() ?? this.asString() ?? this.asBoolean() ?? this.asArray();
    };
    
    /**
     * Traverse sub node(s) (structs or lists)
     * Traverse nested structs in single call using delimiter (default "/") between element names
     * @param id Element name (struct) or index (list element)
     * @param delimiter Optional - delimiter used if multiple sub nodes were specified
     * @returns 
     */
    get(id : number | string, delimiter : string = "/"): DeviceConstants | undefined {
        let key : number | undefined;

        if (typeof id === "number")
        {
            key = sdkIntegration.GetConstListSync(this.deviceID, this.refKey, id);
        }
        else
        {
            key = this.refKey;
            id.split(delimiter).forEach(element => {
                if (typeof key !== "number" || key == invalidRefKey) return undefined;
                key = sdkIntegration.GetConstFieldSync(this.deviceID, key, element);
            });
        }

        if (typeof key === "number" && key != invalidRefKey) return new DeviceConstants(this.deviceID, key);
        return undefined;
    };
    
    /**
     * Get string representation of current value at node
     * @returns Value at node if it is a number, string or boolean, as a string.
     * Alternatively returns "array" or "struct" to indicate sub node type or 
     * "undefined" if node is invalid.
     */
    toString = () : string => {
        if (!this.isValid()) return "undefined";
        const val : number | string | boolean | DeviceConstants[] | undefined = this.getValue();

        if (typeof val === "number")
            return val.toString();
        if (typeof val === "string")
            return val;
        if (typeof val === "boolean")
            return val ? "true" : "false";
        if (typeof val !== "undefined")
            return "array";
        return "struct";
    }
    
    /**
     * Does this node point to an existing value?
     * @returns True if this node exists
     */
    isValid() : boolean {
        return this.refKey != invalidRefKey;
    };
};
