#include "stdafx.h"
#include <unordered_map>
#include <chrono>
#include <string.h>
#include "bt.h"

// -----------------------------------------------------------

/**
 * Holds all init state to pass between threads, including
 * all paramters to Jabra_Initialize, Jabra_SetAppID and
 * all event handler functions.
 */
class StateJabraInitialize {
  private:
  Napi::Env env;

  std::string appId;
  ThreadSafeCallback *initializedCallback;
  ThreadSafeCallback *firstScanDoneCallback;
  ThreadSafeCallback *attachedCallback;
  ThreadSafeCallback *deAttachedCallback;
  ThreadSafeCallback *buttonInDataTranslatedCallback;
  ThreadSafeCallback *devLogCallback;
  ThreadSafeCallback *batteryStatusCallback;
  ThreadSafeCallback *remoteMmiCallback;
  ThreadSafeCallback *downloadFirmwareProgressCallback;
  ThreadSafeCallback *uploadProgressCallback;
  ThreadSafeCallback *registerPairingListCallback;
  ThreadSafeCallback *gNPButtonEventCallBack;
  ThreadSafeCallback *dectInfoCallback;

  std::string proxy;
  std::string baseUrl_capabilities;
  std::string baseUrl_fw;
  bool blockAllNetworkAccess;
  bool nonJabraDeviceDectection;

  bool initializationStartedState;
  
  static void releaseCallback(ThreadSafeCallback* & callback) {
    if (callback) {
      ThreadSafeCallback* exception_safe_copy = callback;
      callback = nullptr;
      delete exception_safe_copy;
    }
  }

  public:
  StateJabraInitialize() : env(NULL), 
                           initializedCallback(nullptr),
                           firstScanDoneCallback(nullptr),
                           attachedCallback(nullptr),
                           deAttachedCallback(nullptr),
                           buttonInDataTranslatedCallback(nullptr),
                           devLogCallback(nullptr),
                           batteryStatusCallback(nullptr),
                           remoteMmiCallback(nullptr),
                           downloadFirmwareProgressCallback(nullptr),
                           uploadProgressCallback(nullptr),
                           registerPairingListCallback(nullptr),
                           gNPButtonEventCallBack(nullptr),
                           dectInfoCallback(nullptr),
                           initializationStartedState(false) {}

  void set(const Napi::Env& _env,
           const std::string& _appId,
           ThreadSafeCallback* _initializedCallback,
           ThreadSafeCallback* _firstScanDoneCallback,
           ThreadSafeCallback* _attachedCallback,
           ThreadSafeCallback* _deAttachedCallback,
           ThreadSafeCallback* _buttonInDataTranslatedCallback,
           ThreadSafeCallback* _devLogCallback,
           ThreadSafeCallback* _batteryStatusCallback,
           ThreadSafeCallback* _remoteMmiCallback,
           ThreadSafeCallback* _downloadFirmwareProgressCallback,
           ThreadSafeCallback* _uploadProgressCallback,
           ThreadSafeCallback* _registerPairingListCallback,
           ThreadSafeCallback* _gNPButtonEventCallBack,
           ThreadSafeCallback* _dectInfoCallback,
           const std::string& _proxy,
           const std::string& _baseUrl_capabilities,
           const std::string& _baseUrl_fw,
           const bool _blockAllNetworkAccess,
           const bool _nonJabraDeviceDectection 
           ) {
      env = _env;
      appId = _appId;

      initializedCallback = _initializedCallback;
      firstScanDoneCallback = _firstScanDoneCallback;
      attachedCallback = _attachedCallback;
      deAttachedCallback = _deAttachedCallback;
      buttonInDataTranslatedCallback = _buttonInDataTranslatedCallback;
      devLogCallback = _devLogCallback;
      batteryStatusCallback = _batteryStatusCallback;
      remoteMmiCallback = _remoteMmiCallback;
      downloadFirmwareProgressCallback = _downloadFirmwareProgressCallback;
      uploadProgressCallback = _uploadProgressCallback;
      registerPairingListCallback = _registerPairingListCallback;
      gNPButtonEventCallBack = _gNPButtonEventCallBack;
      dectInfoCallback = _dectInfoCallback;

      proxy = _proxy;
      baseUrl_capabilities = _baseUrl_capabilities;
      baseUrl_fw = _baseUrl_fw;
      blockAllNetworkAccess = _blockAllNetworkAccess;

      nonJabraDeviceDectection = _nonJabraDeviceDectection;

      initializationStartedState = true;
  }

  
  bool isInitializationStarted() {
    return initializationStartedState;
  }

  const std::string& getAppId() const {
    return appId;
  }

  ThreadSafeCallback * getInitializedCallback() {
    return initializedCallback;
  }

  ThreadSafeCallback * getFirstScanDoneCallback() {
    return firstScanDoneCallback;
  }

  ThreadSafeCallback * getAttachedCallback() {
    return attachedCallback;
  }

  ThreadSafeCallback * getDeAttachedCallback() {
    return deAttachedCallback;
  }

  ThreadSafeCallback * getButtonInDataTranslatedCallback() {
    return buttonInDataTranslatedCallback;
  }

  ThreadSafeCallback * getDevLogCallback() {
    return devLogCallback;
  }

  ThreadSafeCallback * getBatteryStatusCallback() {
    return batteryStatusCallback;
  }

  ThreadSafeCallback * getRemoteMmiCallback () {
    return remoteMmiCallback;
  }
  
  ThreadSafeCallback * getDownloadFirmwareProgressCallback() {
    return downloadFirmwareProgressCallback;
  }

  ThreadSafeCallback * getUploadProgressCallback() {
    return uploadProgressCallback;
  }

  ThreadSafeCallback * getRegisterPairingListCallback() {
    return registerPairingListCallback;
  }

  ThreadSafeCallback * getGNPButtonEventCallBack() {
    return gNPButtonEventCallBack;
  }

  ThreadSafeCallback * getDectInfoCallBack() {
    return dectInfoCallback;
  }

  std::string& getProxy() {
    return proxy;
  }

  std::string& getBaseUrl_fw() {
    return baseUrl_fw;
  }

  std::string& getBaseUrl_capabilities() {
    return baseUrl_capabilities;
  }

  bool getBlockAllNetworkAccess() {
    return blockAllNetworkAccess;
  }

  bool getNonJabraDeviceDectection() {
    return nonJabraDeviceDectection;
  }

  // Must be called to free resources (both to ensure no memory/resource leaks AND 
  // to make sure the node process won't block on exit).
  // Needs to be called from main thread.
  void done() {
    releaseCallback(initializedCallback);
    releaseCallback(firstScanDoneCallback);
    releaseCallback(attachedCallback);
    releaseCallback(deAttachedCallback);
    releaseCallback(buttonInDataTranslatedCallback);
    releaseCallback(devLogCallback);
    releaseCallback(batteryStatusCallback);
    releaseCallback(remoteMmiCallback);
    releaseCallback(downloadFirmwareProgressCallback);
    releaseCallback(uploadProgressCallback);
    releaseCallback(registerPairingListCallback);
    releaseCallback(gNPButtonEventCallBack);
    releaseCallback(dectInfoCallback);
 
    // Re-allow init again.
    initializationStartedState = false;
  }
};

/**
 * Get ms since epoc.
 */
static std::uint64_t getTimeSinceEpoc() {
   return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

/** 
 * As a hack, use global to pass state between thread and callbacks. Could be avoided if
 * the Jabra SDK c-type callbacks could be extended with a arbitary context parameter.
 */
static StateJabraInitialize state_Jabra_Initialize;

/**
 * Implements a combination of Jabra_Initialize, Jabra_SetAppID and all Jabra_RegisterXXX 
 * event handler setup functions. The implementation creates it's own thread to call 
 * all Jabra SDK functions. 
 * 
 * Important tempoary implementation note:
 * Callbacks are currently managed using a 3rd party napi-thread-safe-callback
 * library as the node-addon-api does not yet provide a c++ api for
 * cross thread callbacks. This 3rd-party implementation is not as portable
 * or future-proof n-api. Hence, once a official ThreadSafeFunction
 * (https://github.com/nodejs/node-addon-api/pull/442) is released 
 * the implementation should switch to the official 
 *
 */
Napi::Value napi_Initialize(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  const Napi::Env env = info.Env();
  
  LOG_DEBUG_(LOGINSTANCE) << functionName << " called";

  // Guard that we don't initialize twice. We use global memory for init so this could be a problem if allowed.
  if (state_Jabra_Initialize.isInitializationStarted()) {
    Napi::Error::New(env, "Jabra library already initialized or initialization already in progress").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (util::verifyArguments(__func__, info, { util::STRING,
      util::FUNCTION, util::FUNCTION, util::FUNCTION,
      util::FUNCTION, util::FUNCTION, util::FUNCTION,
      util::FUNCTION, util::FUNCTION, util::FUNCTION,
      util::FUNCTION, util::FUNCTION, util::FUNCTION,
      util::FUNCTION, util::OBJECT })) {

    int argNr = 0;

    std::string appId = info[argNr++].As<Napi::String>();
    auto initializedCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto firstScanDoneCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto attachedCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto deAttachedCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto buttonInDataTranslatedCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto devLogCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto batteryStatusCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto remoteMmiCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto downloadFirmwareProgressCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto uploadProgressCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto registerPairingListCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto gNPButtonEventCallBack = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());
    auto dectInfoCallback = new ThreadSafeCallback(info[argNr++].As<Napi::Function>());

    Napi::Object configParams = info[argNr++].As<Napi::Object>();
    
    const std::string proxy = configParams.Has("proxy") ? (std::string)configParams.Get("proxy").As<Napi::String>() : "";
    const std::string baseUrl_capabilities =  configParams.Has("baseUrl_capabilities") ? (std::string)configParams.Get("baseUrl_capabilities").As<Napi::String>() : "";
    const std::string baseUrl_fw = configParams.Has("baseUrl_fw") ? (std::string)configParams.Get("baseUrl_fw").As<Napi::String>() : "";
    const bool blockAllNetworkAccess =  configParams.Has("blockAllNetworkAccess") ? (bool)configParams.Get("blockAllNetworkAccess").As<Napi::Boolean>() : false;
    const bool nonJabraDeviceDectection =  configParams.Has("nonJabraDeviceDectection") ? (bool)configParams.Get("nonJabraDeviceDectection").As<Napi::Boolean>() : false;


    state_Jabra_Initialize.set(env,
                               appId,
                               initializedCallback,
                               firstScanDoneCallback,
                               attachedCallback,
                               deAttachedCallback,
                               buttonInDataTranslatedCallback,
                               devLogCallback,
                               batteryStatusCallback,
                               remoteMmiCallback,
                               downloadFirmwareProgressCallback,
                               uploadProgressCallback,
                               registerPairingListCallback,
                               gNPButtonEventCallBack,
                               dectInfoCallback,
                               proxy,
                               baseUrl_capabilities,
                               baseUrl_fw,
                               blockAllNetworkAccess,
                               nonJabraDeviceDectection);

    std::thread initThread([functionName](){
      try {                  
          ConfigParams_cloud configParams_cloud;
          configParams_cloud.blockAllNetworkAccess = state_Jabra_Initialize.getBlockAllNetworkAccess();
          configParams_cloud.proxy = state_Jabra_Initialize.getProxy().c_str();
          configParams_cloud.baseUrl_capabilities = state_Jabra_Initialize.getBaseUrl_capabilities().c_str();
          configParams_cloud.baseUrl_fw = state_Jabra_Initialize.getBaseUrl_fw().c_str();

          Config_params config;
          config.deviceCatalogue_params = nullptr;
          config.cloudConfig_params = &configParams_cloud;
          config.reserved2 = nullptr;

          bool nonJabraDeviceDectection = state_Jabra_Initialize.getNonJabraDeviceDectection();

          LOG_DEBUG_(LOGINSTANCE) << "Calling Jabra_SetAppID";
          Jabra_SetAppID(state_Jabra_Initialize.getAppId().c_str());

          LOG_DEBUG_(LOGINSTANCE) << "Calling Jabra_Initialize";
          if (Jabra_InitializeV2([]() {  // First scan done.
              try {
                LOG_DEBUG_(LOGINSTANCE) << "First scan done";

                auto eventTime = getTimeSinceEpoc();

                auto firstScanCallback = state_Jabra_Initialize.getFirstScanDoneCallback();
                if (firstScanCallback) {
                  firstScanCallback->call([eventTime](Napi::Env env, std::vector<napi_value>& args) {
                    args = { Napi::Number::New(env, eventTime) };
                  });            
                }
              } catch (const std::exception &e) {       
                const std::string errorMsg = "Init firstScanDone callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "Init firstScanDone callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            }, [](Jabra_DeviceInfo _deviceInfo) { // attached            
              try {
                LOG_DEBUG_(LOGINSTANCE) << "Device #" << _deviceInfo.deviceID << " attached";

                auto eventTime = getTimeSinceEpoc();

                auto attachedCallback = state_Jabra_Initialize.getAttachedCallback();

                if (attachedCallback) {
                  // Make safe copy to avoid refering to memeory freed by Jabra_FreeDeviceInfo below.
                  ManagedDeviceInfo deviceInfo(_deviceInfo);                
                  Jabra_FreeDeviceInfo(_deviceInfo);

                  attachedCallback->call([deviceInfo, eventTime](Napi::Env env, std::vector<napi_value>& args) {
                      Napi::Object result = Napi::Object::New(env);
                      result.Set(Napi::String::New(env, "deviceID"), (Napi::Number::New(env, deviceInfo.deviceID)));
                      result.Set(Napi::String::New(env, "productID"), (Napi::Number::New(env, deviceInfo.productID)));
                      result.Set(Napi::String::New(env, "vendorID"), (Napi::Number::New(env, deviceInfo.vendorID)));
                      result.Set(Napi::String::New(env, "deviceName"), (Napi::String::New(env, deviceInfo.deviceName)));

                      result.Set(Napi::String::New(env, "usbDevicePath"), (Napi::String::New(env, deviceInfo.deviceName)));
                      result.Set(Napi::String::New(env, "parentInstanceId"), (Napi::String::New(env, deviceInfo.deviceName)));

                      result.Set(Napi::String::New(env, "errorStatus"), (Napi::Number::New(env, deviceInfo.errStatus)));
                      result.Set(Napi::String::New(env, "isDongleDevice"), (Napi::Boolean::New(env, deviceInfo.isDongle)));
                      result.Set(Napi::String::New(env, "dongleName"), (Napi::String::New(env, deviceInfo.dongleName)));
                      result.Set(Napi::String::New(env, "variant"), (Napi::String::New(env, deviceInfo.variant)));
                      result.Set(Napi::String::New(env, "ESN"), (Napi::String::New(env, deviceInfo.serialNumber)));

                      result.Set(Napi::String::New(env, "isInFirmwareUpdateMode"), (Napi::Boolean::New(env, deviceInfo.isInFirmwareUpdateMode)));
                      result.Set(Napi::String::New(env, "connectionType"), (Napi::Number::New(env, deviceInfo.deviceconnection)));
                      result.Set(Napi::String::New(env, "connectionId"), (Napi::Number::New(env, deviceInfo.connectionId)));
                      result.Set(Napi::String::New(env, "parentDeviceId"), (Napi::Number::New(env, deviceInfo.parentDeviceId)));

                      args = { result, Napi::Number::New(env, eventTime) };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Device attach callback handling finished";
              } catch (const std::exception &e) {       
                const std::string errorMsg = "Init attached callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "Init attached callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            }, [](unsigned short deviceID) { // deattached 
              try {
                LOG_DEBUG_(LOGINSTANCE) << "Device #" << deviceID << " de-attached";

                auto eventTime = getTimeSinceEpoc();

                auto deAttachedCallback = state_Jabra_Initialize.getDeAttachedCallback();
                if (deAttachedCallback) {
                  deAttachedCallback->call([deviceID, eventTime](Napi::Env env, std::vector<napi_value>& args) {
                    args = { Napi::Number::New(env, deviceID), Napi::Number::New(env, eventTime) };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Device de-attach callback handling finished";
              } catch (const std::exception &e) {       
                const std::string errorMsg = "Init deattached callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "Init deattached callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            }, [](unsigned short deviceID, unsigned short usagePage, unsigned short usage, bool buttonInData) { // Buttons raw.
                // Ignore - not used.
            },
            [](unsigned short deviceID, Jabra_HidInput translatedInData, bool buttonInData) { // Buttons translated
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Device #" << deviceID << " button press " << translatedInData << ", " << buttonInData;

                auto buttonInDataTranslatedCallback = state_Jabra_Initialize.getButtonInDataTranslatedCallback();
                if (buttonInDataTranslatedCallback) {
                  buttonInDataTranslatedCallback->call([deviceID, translatedInData, buttonInData](Napi::Env env, std::vector<napi_value>& args) {
                      args = { Napi::Number::New(env, deviceID), Napi::Number::New(env, (int)translatedInData), Napi::Boolean::New(env, buttonInData) };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Device button press callback handling finished";
              } catch (const std::exception &e) {       
                const std::string errorMsg = "Init translatedInData callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "Init translatedInData callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            },
            nonJabraDeviceDectection, &config
          )) { // Init success
            LOG_DEBUG_(LOGINSTANCE) << "Jabra_Initialize successful - now registering callbacks";

            // Now that sdk is initialized, we should register all callbacks before we are done:

            Jabra_RegisterDevLogCallback([](unsigned short deviceID, char* _eventStr) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterDevLogCallback callback got eventStr " << _eventStr;
                if (_eventStr) {
                  auto devLogCallback = state_Jabra_Initialize.getDevLogCallback();

                  // Make safe copy to avoid refering to memeory freed by Jabra_FreeString below.
                  std::string eventStr(_eventStr);

                  if (devLogCallback) {
                    devLogCallback->call([deviceID, eventStr](Napi::Env env, std::vector<napi_value>& args) {
                        args = { Napi::Number::New(env, deviceID), Napi::String::New(env, eventStr) };
                    });
                  }
                  Jabra_FreeString(_eventStr);
                }
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterDevLogCallback callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "DevLogCallback callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "DevLogCallback callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }              
            });

            Jabra_RegisterFirmwareProgressCallBack([](unsigned short deviceID, Jabra_FirmwareEventType type, Jabra_FirmwareEventStatus status, unsigned short percentage) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterFirmwareProgressCallBack callback got " << type << " " << status << " " << percentage;

                auto downloadFirmwareProgressCallback = state_Jabra_Initialize.getDownloadFirmwareProgressCallback();
                if (downloadFirmwareProgressCallback) {
                  downloadFirmwareProgressCallback->call([deviceID, type, status, percentage](Napi::Env env, std::vector<napi_value>& args) {
                      args = { Napi::Number::New(env, deviceID), Napi::Number::New(env, (int)type), Napi::Number::New(env, (int)status), Napi::Number::New(env, percentage) };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterFirmwareProgressCallBack callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "FirmwareProgress callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "FirmwareProgress callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }

            });

            Jabra_RegisterPairingListCallback([](unsigned short deviceID, Jabra_PairingList *lst) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterPairingListCallback callback called with " << (lst!=nullptr ? std::to_string(lst->count) : "null") << " pairings";
                if (lst != nullptr) {
                  ManagedPairingList mlst(*lst);

                  auto registerPairingListCallback = state_Jabra_Initialize.getRegisterPairingListCallback();
                  if (registerPairingListCallback) {
                    registerPairingListCallback->call([deviceID, mlst](Napi::Env env, std::vector<napi_value>& args) {
                        Napi::Object jlst = Napi::Object::New(env);
                        jlst.Set(Napi::String::New(env, "listType"), Napi::Number::New(env, mlst.listType));

                        Napi::Array jPairedDevices = Napi::Array::New(env);

                        int i = 0;
                        for (auto it = mlst.pairedDevice.begin(); it != mlst.pairedDevice.end(); it++) {
                          const ManagedPairedDevice& src =  *it;

                          Napi::Object jDev = Napi::Object::New(env);

                          jDev.Set(Napi::String::New(env, "deviceName"), Napi::String::New(env, src.deviceName));

                          std::string btAddrStr = toBTAddrString(src.deviceBTAddr.data(), src.deviceBTAddr.size());

                          jDev.Set(Napi::String::New(env, "deviceBTAddr"), Napi::String::New(env, btAddrStr));

                          jDev.Set(Napi::String::New(env, "isConnected"), Napi::Boolean::New(env, src.isConnected));

                          jPairedDevices.Set(i++, jDev);
                        }                    

                        jlst.Set(Napi::String::New(env, "pairedDevice"), jPairedDevices);
                        args = { Napi::Number::New(env, deviceID), jlst };
                    });
                  }
                  
                  Jabra_FreePairingList(lst);
                }
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterPairingListCallback callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "Jabra_RegisterPairingListCallback callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "Jabra_RegisterPairingListCallback callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            });

            Jabra_RegisterForGNPButtonEvent([] (unsigned short deviceID, ButtonEvent *buttonEvent) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterForGNPButtonEvent callback called with " << (buttonEvent!=nullptr ? std::to_string(buttonEvent->buttonEventCount) : "null") << " button events";

                // First unpack individual key/values into a managed structure that we can safely pass to the callback.
                std::vector<ManagedButtonEventInfo> buttonInfos;
                for (int i=0; i<buttonEvent->buttonEventCount; ++i) {
                  const ButtonEventInfo src = buttonEvent->buttonEventInfo[i];

                  const unsigned short buttonTypeKey = src.buttonTypeKey;
                  const std::string buttonTypeValue = std::string(src.buttonTypeValue);

                  for (int j=0; j<src.buttonEventTypeSize; ++j) {
                    ManagedButtonEventInfo e = { buttonTypeKey, buttonTypeValue, src.buttonEventType[j].key, std::string(src.buttonEventType[j].value) };
                    buttonInfos.push_back(e);
                  }
                }

                auto gNPButtonEventCallBack = state_Jabra_Initialize.getGNPButtonEventCallBack();
                if (gNPButtonEventCallBack) {
                  gNPButtonEventCallBack->call([deviceID, buttonInfos](Napi::Env env, std::vector<napi_value>& args) {
                      Napi::Array buttonEvents = Napi::Array::New(env);

                      // Now repack individual key/value entries into a json structure similar to the orginal:
                      std::unordered_map<unsigned short, uint32_t> targets;
                      for (auto itr = buttonInfos.begin(); itr != buttonInfos.end(); itr++) {
                        const ManagedButtonEventInfo& src = *itr;

                        // Find out if there is there is already an entry for this buttontype, so we can
                        // and add to that if it exist.
                        if (targets.find(src.buttonTypeKey) == targets.end()) {
                           Napi::Array buttonEventInfos = Napi::Array::New(env);
                           
                           Napi::Object o = Napi::Object::New(env);
                           o.Set(Napi::String::New(env, "buttonTypeKey"), Napi::Number::New(env, src.buttonTypeKey));
                           o.Set(Napi::String::New(env, "buttonTypeValue"), Napi::String::New(env, src.buttonTypeValue));
                           o.Set(Napi::String::New(env, "buttonEventType"), buttonEventInfos);

                           uint32_t buttonEventsIndex = buttonEvents.Length();
                           buttonEvents.Set(buttonEventsIndex, o);
                           targets.insert(std::pair<unsigned short, uint32_t>(src.buttonTypeKey, buttonEventsIndex)); 
                        }

                        int buttonEventsIndex = targets[src.buttonTypeKey]; // Should always succed.
                        Napi::Value targetButtonEventInfo = buttonEvents.Get(buttonEventsIndex);

                        if (!targetButtonEventInfo.IsUndefined()) {
                          Napi::Object targetButtonEventInfoObj = targetButtonEventInfo.As<Napi::Object>();
                          Napi::Array targetArray = targetButtonEventInfoObj.Get("buttonEventType").As<Napi::Array>();

                          Napi::Object keyValue = Napi::Object::New(env);
                          keyValue.Set(Napi::String::New(env, "key"), Napi::Number::New(env, src.key));
                          keyValue.Set(Napi::String::New(env, "value"), Napi::String::New(env, src.value));

                          targetArray.Set(targetArray.Length(), keyValue);
                        } else { // We should not get here.
                          LOG_ERROR_(LOGINSTANCE) << "Jabra_RegisterForGNPButtonEvent callback internal error - could not lookup target";
                        }
                      }

                      args = { Napi::Number::New(env, deviceID), buttonEvents };
                  });
                }
                
                Jabra_FreeButtonEvents(buttonEvent);
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterForGNPButtonEvent callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "Jabra_RegisterForGNPButtonEvent callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "Jabra_RegisterForGNPButtonEvent callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            });

            Jabra_RegisterBatteryStatusUpdateCallback([] (unsigned short deviceID, int levelInPercent, bool charging, bool batteryLow) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterBatteryStatusUpdateCallback callback got " << levelInPercent << " " << charging << " " << batteryLow;

                auto batteryStatusCallback = state_Jabra_Initialize.getBatteryStatusCallback();

                if (batteryStatusCallback) {
                  batteryStatusCallback->call([deviceID, levelInPercent, charging, batteryLow](Napi::Env env, std::vector<napi_value>& args) {
                      args = { Napi::Number::New(env, deviceID), Napi::Number::New(env, levelInPercent), Napi::Boolean::New(env, charging), Napi::Boolean::New(env, batteryLow) };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterBatteryStatusUpdateCallback callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "BatteryStatusUpdate callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "BatteryStatusUpdate callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            });  
           
            Jabra_RegisterRemoteMmiCallback([] (unsigned short deviceID, RemoteMmiType type, RemoteMmiInput action){
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterRemoteMmiCallback callback got " << type << " " << action;

                auto remoteMmiCallback = state_Jabra_Initialize.getRemoteMmiCallback();

                if (remoteMmiCallback) {
                  remoteMmiCallback->call([deviceID, type, action](Napi::Env env, std::vector<napi_value>& args) {
                    args = { Napi::Number::New(env, deviceID), Napi::Number::New(env, type), Napi::Number::New(env, action)};
                  });
                }                
              } catch (const std::exception &e) {
                const std::string errorMsg = "RegisterRemoteMmiCallback callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "RegisterRemoteMmiCallback callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }             
            });
            
            Jabra_RegisterUploadProgress([] (unsigned short deviceID, Jabra_UploadEventStatus status, unsigned short percentage) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterUploadProgress got " << status << " " << percentage;

                auto uploadProgressCallback = state_Jabra_Initialize.getUploadProgressCallback();

                if (uploadProgressCallback) {
                  uploadProgressCallback->call([deviceID, status, percentage](Napi::Env env, std::vector<napi_value>& args) {
                      args = { Napi::Number::New(env, deviceID), Napi::Number::New(env, status), Napi::Number::New(env, percentage) };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterUploadProgress callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "RegisterUploadProgress callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "RegisterUploadProgress callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            });

            Jabra_RegisterDectInfoHandler([] (unsigned short deviceID, Jabra_DectInfo* dectInfo) {
              try {
                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterDectInfoHandler got " << dectInfo;

                auto dectInfoCallback = state_Jabra_Initialize.getDectInfoCallBack();

                if (dectInfoCallback) {
                  /*
                      Jabra_DectInfo is a C struct with a bunch of numbers
                      and a statically allocate array. It's safe to copy it
                      this way.
                  */
                  Jabra_DectInfo dectInfoStack = *dectInfo;
                  Jabra_FreeDectInfoStr(dectInfo);

                  dectInfoCallback->call([deviceID, dectInfoStack](Napi::Env env, std::vector<napi_value>& args) {
                    Napi::Object dectInfoNapi = Napi::Object::New(env);

                    Napi::Uint8Array rawData = Napi::Uint8Array::New(env, dectInfoStack.RawDataLen);
                    for (unsigned int k = 0; k < dectInfoStack.RawDataLen; ++k) {
                      rawData[k] = dectInfoStack.RawData[k];
                    }
                    dectInfoNapi.Set(Napi::String::New(env, "rawData"), rawData);

                    switch (dectInfoStack.DectType) {
                      case DectDensity: {
                        const Jabra_DectInfoDensity& dectDensity = dectInfoStack.DectDensity;
                        dectInfoNapi.Set(Napi::String::New(env, "kind"), Napi::String::New(env, "density"));
                        dectInfoNapi.Set(Napi::String::New(env, "sumMeasuredRSSI"), Napi::Number::New(env, dectDensity.SumMeasuredRSSI));
                        dectInfoNapi.Set(Napi::String::New(env, "maximumReferenceRSSI"), Napi::Number::New(env, dectDensity.MaximumReferenceRSSI));
                        dectInfoNapi.Set(Napi::String::New(env, "numberMeasuredSlots"), Napi::Number::New(env, dectDensity.NumberMeasuredSlots));
                        dectInfoNapi.Set(Napi::String::New(env, "dataAgeSeconds"), Napi::Number::New(env, dectDensity.DataAgeSeconds));
                        break;
                      }

                      case DectErrorCount: {
                        const Jabra_DectErrorCount& dectError = dectInfoStack.DectErrorCount;
                        dectInfoNapi.Set(Napi::String::New(env, "kind"), Napi::String::New(env, "errorCount"));
                        dectInfoNapi.Set(Napi::String::New(env, "syncErrors"), Napi::Number::New(env, dectError.syncErrors));
                        dectInfoNapi.Set(Napi::String::New(env, "aErrors"), Napi::Number::New(env, dectError.aErrors));
                        dectInfoNapi.Set(Napi::String::New(env, "xErrors"), Napi::Number::New(env, dectError.xErrors));
                        dectInfoNapi.Set(Napi::String::New(env, "zErrors"), Napi::Number::New(env, dectError.zErrors));
                        dectInfoNapi.Set(Napi::String::New(env, "hubSyncErrors"), Napi::Number::New(env, dectError.hubSyncErrors));
                        dectInfoNapi.Set(Napi::String::New(env, "hubAErrors"), Napi::Number::New(env, dectError.hubAErrors));
                        dectInfoNapi.Set(Napi::String::New(env, "handoversCount"), Napi::Number::New(env, dectError.handoversCount));
                        break;
                      }
                    }

                    args = { Napi::Number::New(env, deviceID), dectInfoNapi };
                  });
                }

                LOG_VERBOSE_(LOGINSTANCE) << "Jabra_RegisterDectInfoHandler callback handling finished";
              } catch (const std::exception &e) {
                const std::string errorMsg = "RegisterDectInfoHandler callback failed: " + std::string(e.what());
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              } catch (...) {
                const std::string errorMsg = "RegisterDectInfoHandler callback failed failed with unknown exception";
                LOG_FATAL_(LOGINSTANCE) << errorMsg;
              }
            });

            // Finally, notify caller that init succeded:
            auto initCallback = state_Jabra_Initialize.getInitializedCallback();
            if (initCallback) {
              initCallback->call([](Napi::Env env, std::vector<napi_value>& args) {
                  args = { };
              });
            }
          } else { // Init failed.
            LOG_FATAL_(LOGINSTANCE) << "Jabra_Initialize failed";

            auto initCallback = state_Jabra_Initialize.getInitializedCallback();
            if (initCallback) {
              initCallback->error("Could not initialize jabra sdk");
            }
          }
      } catch (const std::exception &e) {       
        const std::string errorMsg = std::string(functionName) + " worker thread failed: " + std::string(e.what());
        LOG_FATAL_(LOGINSTANCE) << errorMsg;
      } catch (...) {
        const std::string errorMsg =  std::string(functionName) + " worker thread failed with unknown exception";
        LOG_FATAL_(LOGINSTANCE) << errorMsg;
      }
    });

    // Let thread safely live on after thead object goes out of scope (is destroyed).
    initThread.detach();

    LOG_DEBUG_(LOGINSTANCE) << functionName << " worker thread started";
  }

  return env.Null(); 
}

Napi::Value napi_UnInitialize(const Napi::CallbackInfo& info) {
  return util::JSyncWrapper<Napi::Value>(__func__, info, [](const char * const functionName, const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    bool retv = Jabra_Uninitialize();
    if (retv) {
      // Properly need to be called from main thread - so not sure this can be async if we should want this ?
      state_Jabra_Initialize.done();
    }
    return Napi::Boolean::New(env, retv);
  });
}

Napi::Value napi_ConnectToJabraApplication(const Napi::CallbackInfo& info)
{
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::STRING, util::STRING, util::FUNCTION})) {
    const std::string guid = info[0].As<Napi::String>();
    const std::string softphoneName = info[1].As<Napi::String>();    
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<bool, Napi::Boolean>(
      functionName, 
      javascriptResultCallback,
      [functionName, guid, softphoneName](){              
        return Jabra_ConnectToJabraApplication(guid.c_str(), softphoneName.c_str());
      }, 
      [](const Napi::Env& env, const bool result) {
        return Napi::Boolean::New(env, result);
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_DisconnectFromJabraApplication(const Napi::CallbackInfo& info)
{
  const char * const functionName = __func__;
  return util::SimpleAsyncFunction<Napi::Value, int>(functionName, info, 
    [functionName]() {
      Jabra_DisconnectFromJabraApplication();
      return 0;
    }, 
    [](const Napi::Env& env, const int dummy) { 
      return env.Null();
    }
  );
}

Napi::Value napi_SetSoftphoneReady(const Napi::CallbackInfo& info) 
{
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::BOOLEAN, util::FUNCTION})) {
    const bool isReady = info[0].As<Napi::Boolean>().ToBoolean();
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, isReady](){              
        Jabra_SetSoftphoneReady(isReady);
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_IsSoftphoneInFocus(const Napi::CallbackInfo& info) 
{
  const char * const functionName = __func__;
  return util::SimpleAsyncFunction<Napi::Boolean, bool>(functionName, info, 
    [functionName]() {
      return Jabra_IsSoftphoneInFocus();
    }, 
    [](const Napi::Env& env, const bool result) {
       return Napi::Boolean::New(env, result);
    }
  );
}

Napi::Value napi_GetErrorString(const Napi::CallbackInfo& info)
{
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const Jabra_ReturnCode errorCode = (Jabra_ReturnCode)info[0].As<Napi::Number>().Uint32Value();  
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<const char *, Napi::String>(
      functionName,
      javascriptResultCallback,
      [functionName, errorCode](){              
        const char * result = Jabra_GetReturnCodeString(errorCode);

        // Warning: This error handling is very brittle.
        // TODO: Find/arrange a better way with SDK team.
        if (result == nullptr) {
          util::JabraException::LogAndThrow(functionName, "Could not lookup error");
        } else if (strcmp(result, "Unknown error code") == 0) {
          util::JabraException::LogAndThrow(functionName, result);
        }

        return result;
      }, 
      [](const Napi::Env& env, const char * result) {
        return Napi::String::New(env, result ? result : "");
      },
      [](const char * result) {
        // Assume error strings does not need to be freed.
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetVersion(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleAsyncFunction<Napi::String, std::string>(functionName, info, [functionName]() {
    char buf[64];
    Jabra_ReturnCode retv;
    if ((retv = Jabra_GetVersion(&buf[0], sizeof(buf))) == Return_Ok) {
      return std::string(buf);
    } else {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
      return std::string(); // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, const std::string& cppResult) { return Napi::String::New(env, cppResult); });
}

/**
 * This function is for N-API experiments. Content may be deleted or replaced at will for new experiments.
 * 
 * Do not call this function in production - it is for experiments only.
 */
Napi::Value napi_SyncExperiment(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  const Napi::Env env = info.Env();
  return env.Undefined();
}
