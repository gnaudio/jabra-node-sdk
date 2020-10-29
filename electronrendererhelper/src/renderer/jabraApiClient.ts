
// Browser friendly type-only imports as we can assume the electron app has these types loaded already.
type IpcRenderer = import('electron').IpcRenderer;

import { ClassEntry, JabraType, DeviceInfo, 
         enumDeviceBtnType, enumFirmwareEventType, enumFirmwareEventStatus, PairedListInfo, enumUploadEventStatus,
         JabraTypeEvents, DeviceTypeEvents, JabraEventsList, DeviceEventsList, DeviceType, MetaApi, MethodEntry, 
         AddonLogSeverity, NativeAddonLogConfig, DeviceTiming, enumRemoteMmiType, enumRemoteMmiInput, DectInfo } from '@gnaudio/jabra-node-sdk';
import { getExecuteDeviceTypeApiMethodEventName, getDeviceTypeApiCallabackEventName, getJabraTypeApiCallabackEventName, 
         getExecuteJabraTypeApiMethodEventName, getExecuteJabraTypeApiMethodResponseEventName, 
         getExecuteDeviceTypeApiMethodResponseEventName, createApiClientInitEventName,
         jabraApiClientReadyEventName, jabraLogEventName, ApiClientInitEventData, createApiClientInitResponseEventName } from '../common/ipc';
import { nameof, isBrowser, serializeError } from '../common/util';

/**
 * Serialized error passed by message.
 */
declare type SerializedError = {
    name: string;
    message: string;
    stack?: string;
    code?: number;
};

/**
 * Promise singleton tthat createApiClient creates/reuses.
 */
let cachedApiClientPromise: Promise<JabraType> | null = null;

/**
* Factory method for creating promise returning remote client-side singleton instance of JabraType.
*/
export function createApiClient(ipcRenderer: IpcRenderer) : Promise<JabraType> {
    if (!isBrowser()) {
        return Promise.reject(new Error("This createApiClient() function needs to run in a browser process"));
    }

    if (!ipcRenderer) {  
        return Promise.reject(new Error("ipcRenderer argument missing to createApiClient() factory method"));
    }

    if (cachedApiClientPromise) {
        return cachedApiClientPromise;
    } else {
        cachedApiClientPromise = new Promise<JabraType>((resolve, reject) => {           
            try {
                JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.info, "createApiClient", "Looking up Jabra API meta data");

                // Notify server that client is up and running and that we would like to get meta data as a response event.
                ipcRenderer.send(createApiClientInitEventName);

                // Wait for answer from server.
                ipcRenderer.once(createApiClientInitResponseEventName, (event, setupConfigResponse: SerializedError | ApiClientInitEventData) => {
                    // Make return value easier to use and print:
                    addToStringToDeserializedObject(setupConfigResponse);

                    // If we have some log configuration, save it locally for optimaization purposes.
                    if (setupConfigResponse && setupConfigResponse.hasOwnProperty(nameof<ApiClientInitEventData>("logConfig"))) {
                        logConfig = (setupConfigResponse as any).logConfig;
                        
                        JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.verbose, "createApiClient", "Got jabra log configuration:" + JSON.stringify(logConfig, null, 3));
                    }

                    // Get meta information from setup response.
                    if (setupConfigResponse && setupConfigResponse.hasOwnProperty(nameof<ApiClientInitEventData>("apiMeta"))) {
                        const apiMeta : ReadonlyArray<ClassEntry> = (setupConfigResponse as any).apiMeta;
                        JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.verbose, "createApiClient", "Got jabra apiMeta:" + JSON.stringify(apiMeta, null, 3));

                        const jabraClassName = JabraType.name;
                        let jabraTypeMeta = apiMeta.find((c) => c.name === jabraClassName);
                        if (!jabraTypeMeta) {
                            let error = new Error("Could not find meta data for " + jabraClassName);
                            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createApiClient", error);
                            return Promise.reject(error);
                        }
                
                        const deviceClassName = DeviceType.name;
                        let deviceTypeMeta = apiMeta.find((c) => c.name === deviceClassName);
                        if (!deviceTypeMeta) {
                            let error = new Error("Could not find meta data for " + deviceClassName);
                            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createApiClient", error);
                            return Promise.reject(error);
                        }

                        const result = doCreateRemoteJabraType(jabraTypeMeta, deviceTypeMeta, ipcRenderer);

                        // Calulate the ready time used to replay old events. There is a potential unsolved
                        // theoretical problem if events hanppened while doCreateRemoteJabraType is being executed.
                        // We could improve this marginally by setting the ready time inside this function at 
                        // the exact right place after event handlers are setup, but even that might fail
                        // in theory. However, as USB scanning takes time (so events will come after this
                        // code) these kind of problems are unlikely to happen in real life.
                        // 
                        // Another potential problem with this timing call, is that the browser values are
                        // by design fuzzied because of secruity issues with Meltdown/Spectre. The time 
                        // returned here might thus be off by a couple of milliseconds which creates
                        // another potential race condition, that might cause a attach event to be missed
                        // or repeated if we are extremely unlucky.
                        const clientReadyTime = Date.now();
                        
                        JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.info, "createApiClient", "Client side JabraType proxy succesfully created at t=" + clientReadyTime);

                        // Ask server to re-send attach events before now
                        ipcRenderer.send(jabraApiClientReadyEventName, clientReadyTime);

                        return resolve(result);
                    } else {
                        let failure;
                        if ((setupConfigResponse as any).name) {
                            failure = deserializeError(setupConfigResponse as SerializedError);
                            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createApiClient", failure);
                        } else {
                            failure = setupConfigResponse;
                        }
                        return reject(failure);
                    }

                });
            } catch (err) {
                let combinedError = new Error("Internal error during meta retrivial / construction of remote proxy. Got error " + err);
                JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createApiClient", combinedError);
                return reject(combinedError);
            }
        });
    
        return cachedApiClientPromise;
    }
}

/**
 * The generalized event callback used for jabra api.
 */
type EventCallback =  (...args: any[]) => void;

/**
 * The generalized event handling api in commen for all jabra api objects.
 */
interface JabraEventManagementApi {
    on: (eventName: any, callback: EventCallback) => any,
    off: (eventName: any, callback: EventCallback) => any
}

interface DeviceTypeExtras {
    /**
     * Internal method that is called whenever we are finished with a device object, 
     * so relevant resources can be freed.
     */
    _shutdown(): void;

    /**
     * Internal method that is called when a device is deatached.
     */
    _update_detached_time_ms(time_ms: number): void;
}

interface JabraTypeExtras {
    // Nothing so far.
}

/**
 * Internal helper that stores information about the promise to resolve/reject
 * for a command being processed.
 */
interface PromiseCallbacks {
    methodName: string,
    resolve: (value?: any | PromiseLike<any> | undefined) => void;
    reject: (err: Error) => void;
}

/**
 * Helper that create a js proxy handler for a api class meta description. Mehod execution and event management
 * must be handled by provided delegating callbacks.
 */
function doCreateProxy<T extends (MetaApi & JabraEventManagementApi)>(meta: ClassEntry, 
                                          validCheck: () => boolean,
                                          methodExecutor: (methodName: string, methodMeta: MethodEntry, ...args : any[]) => any,
                                          on: (eventName: string, callback: EventCallback) => any,
                                          off: (eventName: string, callback: EventCallback) => any
                                         ) : ProxyHandler<T> {
    return {
        get: (target, propKey, receiver) => {
            const isValid = validCheck();

            const propName = propKey.toString();
            let methodEntry: MethodEntry | undefined;
            if (propKey === Symbol.toPrimitive) {
                return undefined; // Not supported.
            } else if (propName === nameof<Object>("toString") || propName === nameof<Object>("toLocaleString")) {
                return () => "[object proxy for " + meta.name + "]";
            } else if (propName === nameof<Object>("valueOf")) {
                return () => JSON.stringify(target, null, 2);
            } else if (propName === "toJSON") {
                return () => target;
            } else if (propName ===  nameof<T>("getMeta")) {
                return () => meta; // Use local value for efficiency rather than server side value.
            } else if (propName === nameof<T>("on")) {
                return (eventName: string, callback: EventCallback) => {
                    on(eventName, callback);  
                };
            } else if (propName === nameof<T>("off")) {
                return (eventName: string, callback: EventCallback) => {
                    off(eventName, callback);
                };
            } else if (meta.properties.find(p => p.name === propName)) {
                // Properties (if any) are stored on local object (all readonly).
                return Reflect.get(target, propKey);
            } else if ((methodEntry = meta.methods.find(m => m.name === propName)) || propName.startsWith("_")) {
                return (...args : any[]) => {
                    // Normal (non-internal) calls are only allowed if we are fully valid, otherwise provide error.
                    if (isValid || propName.startsWith("_")) {
                        return methodExecutor(propKey.toString(), methodEntry!, ...args);
                    } else {
                        const error = new Error(meta.name + "instance no longer active/valid. Can not call method");
                        if (methodEntry && methodEntry.jsType === Promise.name) {
                            return Promise.reject(error);
                        } else {
                            throw error;
                        } 
                    }
                };
            } else {
                return undefined;
            }
        },

        setPrototypeOf: (target: T, v: any) : boolean => {
            throw new TypeError("setPrototypeOf not supported");
        },
        
        isExtensible: (target: T): boolean => {
            return false;
        },

        preventExtensions: (target: T): boolean => {
            return true;
        },

        getOwnPropertyDescriptor: (target: T, p: PropertyKey): PropertyDescriptor | undefined => {
            let key = p.toString();
            let propIndex = meta.properties.findIndex(p => p.name === key);
            if (propIndex) {
                return {
                    configurable: false,
                    enumerable: true,
                    writable: !meta.properties[propIndex].readonly,
                    value: (target as any)[key]
                }
            } 

            let methodIndex =  meta.methods.findIndex(p => p.name === key);
            if (methodIndex) {
                return {
                    configurable: false,
                    enumerable: true,
                    writable: false,
                    value: undefined
                }
            }

            return undefined;
        },

        has: (target: T, p: PropertyKey): boolean => {
            let key = p.toString();
            return meta.properties.findIndex(p => p.name === key) >=0 || meta.methods.findIndex(p => p.name === key) >= 0;
        },

        set: (target: T, p: PropertyKey, value: any, receiver: any): boolean => {
            throw new TypeError("set not supported");
        },

        deleteProperty: (target: T, p: PropertyKey): boolean => {
            throw new TypeError("deleteProperty not supported");
        },

        defineProperty: (target: T, p: PropertyKey, attributes: PropertyDescriptor): boolean => {
            throw new TypeError("defineProperty not supported");
        },

        enumerate: (target: T): PropertyKey[] => {
            return [...meta.properties.map(p => p.name), ...meta.methods.map(p => p.name)];
        },

        ownKeys: (target: T): PropertyKey[] => {
            return [...meta.properties.map(p => p.name), ...meta.methods.map(p => p.name)];
        },

        apply: (target: T, thisArg: any, argArray?: any): any => {
            throw new TypeError("apply not supported");
        },

        construct: (target: T, argArray: any, newTarget?: any): object => {
            throw new TypeError("construct not supported");
        }
    };

}

/**
 * Handles event subscription and emits like a simple EventEmitter
 * that can be used client-side.
 */
class SimpleEventEmitter<T extends string> {   
    private _eventListeners: Map<T, Array<EventCallback>> = new Map<T, Array<EventCallback>>();
    private _events: ReadonlyArray<T>;
    
    constructor(events: ReadonlyArray<T>) {
        this._events = events;
        this._events.forEach((event: T) => this._eventListeners.set(event, []));
    }

    emit(eventName: T, ...args: any[]) {
        let callbacks = this._eventListeners.get(eventName);
        if (callbacks) {
            callbacks.forEach((callback) => {
                callback(...args);
            });
        } 
    }
    
    on(eventName: T, callback: EventCallback) {
        let callbacks = this._eventListeners.get(eventName);
        if (!callbacks!.find((c) => c === callback)) {
          callbacks!.push(callback);
        }
    }

    off(eventName: T, callback: EventCallback) {
        let callbacks = this._eventListeners.get(eventName);
        let findIndex = callbacks!.findIndex((c) => c === callback);
        if (findIndex >= 0) {
            callbacks!.splice(findIndex, 1);
        }
    }
    
    removeAllListeners() {
        this._eventListeners.forEach((l) => {
            l.length = 0;
        });
    }
}

// Execution id should be unique across API instances so needs to be a global.
let methodExecutionId : number = 0;

/**
 * Create remote remote JabraType using a proxy that forwards events and commands using ipc.
 * There should only be once instance running at the same time
 */
function doCreateRemoteJabraType(jabraTypeMeta: ClassEntry, deviceTypeMeta: ClassEntry, ipcRenderer: IpcRenderer) : JabraType & JabraTypeExtras {
    const devices = new Map<number, DeviceType & DeviceTypeExtras>();
    const resultsByExecutionId = new Map<Number, PromiseCallbacks>();

    // Find out where the range of method execitions start, so we
    // can dismss messages from an earlier API instance (in case we diposed client and recreated it).
    // Nb. this assumes that only one instance is functioning at the same time.
    const startMethodExecutionId = methodExecutionId;

    const eventEmitter = new SimpleEventEmitter<JabraTypeEvents>(JabraEventsList);

    let shutDownStatus: boolean = false;

    function isValid(): boolean
    {
        return !shutDownStatus;
    }

    function emitEvent(eventName: JabraTypeEvents, ...args: any[]) {
        eventEmitter.emit(eventName, ...args);
    }
    
    function executeOn(eventName: string, callback: EventCallback) {
        eventEmitter.on(eventName as JabraTypeEvents, callback);
    }

    function executeOff(eventName: string, callback: EventCallback) {
        eventEmitter.off(eventName as JabraTypeEvents, callback);
    }   

    function executeApiMethod(methodName: string, methodMeta: MethodEntry, ...args : any[]) : any {
        if (methodName == nameof<JabraType>("getAttachedDevices")) {
            // Return our own list of proxies devices for this method !!
            return Array.from(devices.values());
        } else if (methodMeta) {
            const thisMethodId = methodExecutionId++;
            let combinedEventArgs = [ methodName, thisMethodId, ...args];
            ipcRenderer.send(getExecuteJabraTypeApiMethodEventName(), ...combinedEventArgs);

            if (methodMeta.jsType===Promise.name) {
                return new Promise(function(resolve, reject) {
                    resultsByExecutionId.set(thisMethodId, { methodName, resolve, reject });
                });
            } else {
                // For now, we only need to support async remote method (returning promises). If needed in the future, 
                // such methods could be easily supported by calling ipcRenderer.sendSync instead and handle that on the server.
                let error = new Error("This remote client currently only support async remote methods that return promises unlike '" + methodName + "'."); 
                console.warn(error);
                JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "doCreateRemoteJabraType.executeApiMethod", error);
                throw error;
            }
        } else {
          JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "doCreateRemoteJabraType.executeApiMethod", "Do not know how to execute " + methodName);
        }
    }

    /**
     * Receive async method execiution results and resolve/reject corresponding promises.
     */
    ipcRenderer.on(getExecuteJabraTypeApiMethodResponseEventName(), (event, methodName: string, executionId: number, err: SerializedError | undefined, result: any) => {
        // First, ignore responses from old methods from an earlier instance of the client API:
        if (executionId<startMethodExecutionId) {
            return;
        }

        // First make it easier to debug/inspect results:
        addToStringToDeserializedObject(err);
        addToStringToDeserializedObject(result);

        let promiseCallbacks = resultsByExecutionId.get(executionId);
        if (promiseCallbacks) {
            resultsByExecutionId.delete(executionId);
            
            if (methodName !== promiseCallbacks.methodName) {
                let internalError = new Error("Internal error - Expected method name " + methodName + " does match actual method name " + promiseCallbacks.methodName + " for executionId " + executionId);
                console.warn(internalError.message);
                JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.warning, "doCreateRemoteJabraType", internalError);
                promiseCallbacks.reject(internalError);
            } else if (err) {
                let properError = deserializeError(err);
                promiseCallbacks.reject(properError);
            } else {
                promiseCallbacks.resolve(result);
            }

            if (methodName == nameof<JabraType>("disposeAsync")) {
                shutdown();
            };
        } else {
            let internalError = new Error("Internal error - Could not find callback for method name " + methodName + " with executionId " + executionId);
            console.warn(internalError.message);
            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.warning, "doCreateRemoteJabraType", internalError);
        }
    });

    ipcRenderer.on(getJabraTypeApiCallabackEventName('attach'), (event, deviceInfo: (DeviceInfo & DeviceTiming)) => {
        // First make it easier to debug/inspect results:
        addToStringToDeserializedObject(deviceInfo);

        let device = createRemoteDeviceType(deviceInfo, deviceTypeMeta, ipcRenderer);       
        devices.set(deviceInfo.deviceID, device);
        emitEvent('attach', device);
    });
  
    ipcRenderer.on(getJabraTypeApiCallabackEventName('detach'), (event, deviceInfo: (DeviceInfo & DeviceTiming)) => {
        // First make it easier to debug/inspect results:
        addToStringToDeserializedObject(deviceInfo);

        let device = devices.get(deviceInfo.deviceID);
        if (device) {
            devices.delete(deviceInfo.deviceID);
            device._update_detached_time_ms(deviceInfo.detached_time_ms!);
            emitEvent('detach', device);
            device._shutdown();
        } else {
            // If we can't find the device it must be because it was attached in a previous session.
            let error = new Error("Failure to find device with id " + deviceInfo.deviceID + " in mapping.");
            console.warn(error);
            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.warning, "doCreateRemoteJabraType", error);
        }
    });

    ipcRenderer.on(getJabraTypeApiCallabackEventName('firstScanDone'), (event) => {
        emitEvent('firstScanDone');
    });

    function shutdown() {
        // Mark this instance.
        shutDownStatus = true;

        // Invalidate singleton so a new can be created.
        cachedApiClientPromise = null;

        // Unsubscriber everything:
        ipcRenderer.removeAllListeners(getExecuteJabraTypeApiMethodResponseEventName());
        JabraEventsList.forEach((e) => {
            ipcRenderer.removeAllListeners(getJabraTypeApiCallabackEventName(e));
        });

        // Unsubscriber everything for each device also:
        devices.forEach( (device, key) => {
            device._shutdown();
        });

        // Fail all API calls in progress:
        const shutdownError = new Error("Operation cancelled - API is shutdown");
        const inProgressResultsCopy = Array.from(resultsByExecutionId.values());
        resultsByExecutionId.clear();
        inProgressResultsCopy.forEach((e) => {
            e.reject(shutdownError);
        });

        // Remove all listeners.
        eventEmitter.removeAllListeners();
    }

    const proxyHandler = doCreateProxy<JabraType>(jabraTypeMeta, isValid, executeApiMethod, executeOn, executeOff);

    const jabraTypeReadonlyProperties = { 
        appID: undefined // unsupported by proxy at this time (and properly for good for security).
    };

    return new Proxy<JabraType & JabraTypeExtras>(jabraTypeReadonlyProperties as (JabraType & JabraTypeExtras), proxyHandler);
}

/**
 * Create remote DeviceType using a proxy that forwards events and commands using ipc.
 */
function createRemoteDeviceType(deviceInfo: DeviceInfo & DeviceTiming, deviceTypeMeta: ClassEntry, ipcRenderer: IpcRenderer) : DeviceType & DeviceTypeExtras {
    const eventEmitter = new SimpleEventEmitter<DeviceTypeEvents>(DeviceEventsList);

    const resultsByExecutionId = new Map<Number, PromiseCallbacks>();

    // We use a unique channel for each device (since deviceId is ever increasing), so we don't have
    // to care with possible collisions of device execution ids. Thus, just use local i
    let methodExecutionId : number = 0;

    let shutDownStatus: boolean = false;

    function isValid(): boolean
    {
        return deviceInfo.detached_time_ms === undefined && !shutDownStatus;
    }

    function emitEvent(eventName: DeviceTypeEvents, ...args: any[]) {
        eventEmitter.emit(eventName, ...args);
    }
    
    function executeOn(eventName: string, callback: EventCallback) {
        eventEmitter.on(eventName as DeviceTypeEvents, callback);
    }

    function executeOff(eventName: string, callback: EventCallback) {
        eventEmitter.off(eventName as DeviceTypeEvents, callback);
    }  
    
    function executeApiMethod(methodName: string, methodMeta: MethodEntry, ...args : any[]) : any {
        if (deviceInfo.detached_time_ms) {
            return;
        }
        if (methodName == nameof<DeviceTypeExtras>("_shutdown")) {
            // Special local handling for when we are finshed with the device.
            shutdown();
        } else if (methodName == nameof<DeviceTypeExtras>("_update_detached_time_ms"))  {
            const time_ms = args[0];
            // Assign to detached_time_ms even though it is formally a readonly because we don't want clients to change it.
            (deviceInfo.detached_time_ms as DeviceType['detached_time_ms']) = time_ms;
        } else if (methodMeta) {
            const thisMethodExecutionId = methodExecutionId++;
            let combinedEventArgs = [ methodName, thisMethodExecutionId, ...args];
            if (methodMeta.jsType===Promise.name) {
                ipcRenderer.send(getExecuteDeviceTypeApiMethodEventName(deviceInfo.deviceID), ...combinedEventArgs);
                return new Promise(function(resolve, reject) {
                    resultsByExecutionId.set(thisMethodExecutionId, { methodName, resolve, reject });
                });
            } else {
                // For now, we only need to support async remote method (returning promises). If needed in the future, 
                // such methods could be easily supported by calling ipcRenderer.sendSync instead and handle that on the server.
                let error = new Error("This remote client currently only support async remote methods that return promises unlike '" + methodName + "'.");
                JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createRemoteDeviceType.executeApiMethod", error);
                throw error;
            }
        } else {
            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createRemoteDeviceType.executeApiMethod", "Do not know how to execute " + methodName);
        }
    }

    /**
     * Receive async method execiution results and resolve/reject corresponding promises.
     */
    ipcRenderer.on(getExecuteDeviceTypeApiMethodResponseEventName(deviceInfo.deviceID), (event, methodName: string, executionId: number, err: SerializedError | undefined, result: any) => {
        // First make it easier to debug/inspect results:
        addToStringToDeserializedObject(err);
        addToStringToDeserializedObject(result);

        let promiseCallbacks = resultsByExecutionId.get(executionId);
        if (promiseCallbacks) {
            resultsByExecutionId.delete(executionId);
            
            if (methodName !== promiseCallbacks.methodName) {
                let internalError = new Error("Internal error - Expected method name " + methodName + " does match actual method name " + promiseCallbacks.methodName + " for executionId " + executionId + " and device with Id " + deviceInfo.deviceID);
                console.error(internalError.message);
                JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.error, "createRemoteDeviceType", internalError);
                promiseCallbacks.reject(internalError);
            } else if (err) {
                let properError = deserializeError(err);
                promiseCallbacks.reject(properError);
            } else {
                promiseCallbacks.resolve(result);
            }
        } else {
            let internalError = new Error("Internal error - Could not find callback for method name " + methodName + " with executionId " + executionId + " and device with Id " + deviceInfo.deviceID);
            console.warn(internalError.message);
            JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.warning, "createRemoteDeviceType", internalError)
        }
    });
        
    ipcRenderer.on(getDeviceTypeApiCallabackEventName('btnPress', deviceInfo.deviceID), (event, btnType: enumDeviceBtnType, value: boolean) => {
        emitEvent('btnPress', btnType, value);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('busyLightChange', deviceInfo.deviceID), (event, status: boolean) => {
        emitEvent('busyLightChange', status);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('downloadFirmwareProgress', deviceInfo.deviceID), (event, type: enumFirmwareEventType, status: enumFirmwareEventStatus, dwnldStatusInPrcntg: number) => {
        emitEvent('downloadFirmwareProgress', type, status, dwnldStatusInPrcntg);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onBTParingListChange', deviceInfo.deviceID), (event, pairedListInfo: PairedListInfo) => {
        emitEvent('onBTParingListChange', pairedListInfo);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onGNPBtnEvent', deviceInfo.deviceID), (event, btnEvents: Array<{
        buttonTypeKey: number;
        buttonTypeValue: string;
        buttonEventType: Array<{
            key: number;
            value: string;
        }>;
    }>) => {
        emitEvent('onGNPBtnEvent', btnEvents);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onDevLogEvent', deviceInfo.deviceID), (event, eventString: string) => {
        emitEvent('onDevLogEvent', eventString);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onBatteryStatusUpdate', deviceInfo.deviceID), (event, levelInPercent: number, isCharging: boolean, isBatteryLow: boolean) => {
        emitEvent('onBatteryStatusUpdate', levelInPercent, isCharging, isBatteryLow);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onUploadProgress', deviceInfo.deviceID), (event, status: enumUploadEventStatus, levelInPercent: number) => {
        emitEvent('onUploadProgress', status, levelInPercent);
    });

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onRemoteMmiEvent', deviceInfo.deviceID), (event, type: enumRemoteMmiType, input: enumRemoteMmiInput) => {
        emitEvent('onRemoteMmiEvent', type, input);
    });    

    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onDectInfoEvent', deviceInfo.deviceID), (event, dectInfo: DectInfo) => {
        emitEvent('onDectInfoEvent', dectInfo);
    });
  
    ipcRenderer.on(getDeviceTypeApiCallabackEventName('onDiagLogEvent', deviceInfo.deviceID), (event) => {
        emitEvent('onDiagLogEvent');
    });

    /*  
    The above can most likely be replaced by looping over the DeviceEventsList like below. 
    Would require testing all event types to make sure no edgecases are overlooked. 
    Is left outcommented until that opportunity arises: 

    DeviceEventsList.forEach((eventHandle) => {
        ipcRenderer.on(getDeviceTypeApiCallabackEventName(eventHandle, deviceInfo.deviceID), (event, ...props) => {
            emitEvent(eventHandle, ...props);
        });    
    })
    */

    function shutdown() {
        JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.verbose, "createRemoteDeviceType.shutdown", "device #" + deviceInfo.deviceID + " shutdown.");
 
        // Signal that device is no longer valid:
        shutDownStatus = true;

        // Remove all event subscriptions:
        DeviceEventsList.forEach((e) => {
            ipcRenderer.removeAllListeners(getDeviceTypeApiCallabackEventName(e, deviceInfo.deviceID));
        });

        // Fail all API calls in progress:
        const shutdownError = new Error("Operation cancelled - Device no longer attached / api shutdown");
        const inProgressResultsCopy = Array.from(resultsByExecutionId.values());
        resultsByExecutionId.clear();
        inProgressResultsCopy.forEach((e) => {
            e.reject(shutdownError);
        });

        // Remove all subscribers.
        eventEmitter.removeAllListeners();
    }

    const proxyHandler = doCreateProxy(deviceTypeMeta, isValid, executeApiMethod, executeOn, executeOff);

    JabraNativeAddonLog(ipcRenderer, AddonLogSeverity.verbose, "createRemoteDeviceType", "device #" + deviceInfo.deviceID + " created.");

    return new Proxy<DeviceType & DeviceTypeExtras>(deviceInfo as (DeviceType & DeviceTypeExtras), proxyHandler);
}

/**
 * Patch deserialized object to make it more friendly to use.
 */
function addToStringToDeserializedObject(o: any) : void {
    if (o != undefined && o != null && typeof o === 'object') {
        o.toString = () => {
            return JSON.stringify(o, null, 3);
        }
    }
}

/**
 * Return a proper new Error object based on a deserialized one.
 */
function deserializeError(o: SerializedError): Error {
    let result = new Error(o.message)
    result.stack = o.stack;
    (result as any).code = o.code;
    return result;
}

/**
 * Set during createApiClient initialization and used for optimization. 
 */
let logConfig: NativeAddonLogConfig | undefined  = undefined;

/**
 * Internal helper for sending log info to Jabra native log - used to integrate logs for diagnosing errors.
 * 
 * If logConfig is available use this to optimize and filter out log events that are beneath selected log threshold.
 */
function JabraNativeAddonLog(ipcRenderer: IpcRenderer, severity: AddonLogSeverity, caller: string, msg: string | Error)
{
    try {
      const maxSeverity = logConfig ? logConfig.maxSeverity : AddonLogSeverity.verbose;
      if (severity <= maxSeverity) {
        // Always send strings - serialize if needed:
        const serializedMsg = (typeof msg === 'string' || msg instanceof String) ? msg : JSON.stringify(serializeError(msg as Error), null, 3);
        ipcRenderer.send(jabraLogEventName, severity, caller, serializedMsg);
      }
    } catch (e) { // Swallow exceptions to make this call safe to call anywhere.
        console.error(e);
    }
}

