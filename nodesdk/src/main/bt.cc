#include "bt.h"
#include "app.h"
#include <stdlib.h>

// Utility that coverts a hex string to a hex array for BT.
void toBTAddr(uint8_t *dest, const std::string& srcHex, size_t destSize) {
  for (unsigned int i = 0; i < srcHex.length() && destSize>0; i += 2) {
    std::string byteString = srcHex.substr(i, 2);
    uint8_t val = (uint8_t) strtol(byteString.c_str(), nullptr, 16);
    *(dest++) = val;
    --destSize;
  }
}

// Utility that coverts a hex array to a hex string for BT.
std::string toBTAddrString(const uint8_t *src, size_t srcSize) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned int i=0; i<srcSize;  ++i)
        ss << std::setw(2) << (int)src[i];
    return ss.str();
}

Napi::Value napi_ConnectNewDevice(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::STRING, util::BOOLEAN, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    std::string deviceName = info[1].As<Napi::String>();
    const std::string deviceBTAddr = info[2].As<Napi::String>();
    const bool isConnected = info[3].As<Napi::Boolean>().ToBoolean();
    const Napi::Function javascriptResultCallback = info[4].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, deviceName, deviceBTAddr, isConnected](){ 
        Jabra_PairedDevice pDevice;
        pDevice.deviceName = (char *)deviceName.c_str(); // This ought to be safe as Jabra_ConnectNewDevice should not change this.
        pDevice.isConnected = isConnected;
        toBTAddr(pDevice.deviceBTAddr, deviceBTAddr, sizeof(pDevice.deviceBTAddr)/sizeof(uint8_t));

        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_ConnectNewDevice(deviceId, &pDevice)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_ConnectPairedDevice(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::STRING, util::BOOLEAN, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    std::string deviceName = info[1].As<Napi::String>();
    std::string deviceBTAddr = info[2].As<Napi::String>();
    bool isConnected = info[3].As<Napi::Boolean>().ToBoolean();
    Napi::Function javascriptResultCallback = info[4].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, deviceName, deviceBTAddr, isConnected](){ 
        Jabra_PairedDevice pDevice;
        pDevice.deviceName = (char *)deviceName.c_str(); // Hopefully this is safe as Jabra_ConnectPairedDevice should not change this.
        pDevice.isConnected = isConnected;
        toBTAddr(pDevice.deviceBTAddr, deviceBTAddr, sizeof(pDevice.deviceBTAddr)/sizeof(uint8_t));

        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_ConnectPairedDevice(deviceId, &pDevice)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_DisconnectBTDevice(const Napi::CallbackInfo& info) {
 const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_DisconnectBTDevice(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_DisconnectPairedDevice(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::STRING, util::BOOLEAN, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    std::string deviceName = info[1].As<Napi::String>();
    std::string deviceBTAddr = info[2].As<Napi::String>();
    bool isConnected = info[3].As<Napi::Boolean>().ToBoolean();
    Napi::Function javascriptResultCallback = info[4].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, deviceName, deviceBTAddr, isConnected](){ 
        Jabra_PairedDevice pDevice;
        pDevice.deviceName = (char *)deviceName.c_str(); // Hopefully this is safe as Jabra_DisConnectPairedDevice should not change this.
        pDevice.isConnected = isConnected;
        toBTAddr(pDevice.deviceBTAddr, deviceBTAddr, sizeof(pDevice.deviceBTAddr)/sizeof(uint8_t));

        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_DisConnectPairedDevice(deviceId, &pDevice)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}


Napi::Value napi_GetAutoPairing(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info, 
    [functionName](unsigned short deviceId) {
      bool autoParing = Jabra_GetAutoPairing(deviceId);
      return autoParing;
    }, 
    [](const Napi::Env& env, const bool cppResult) { 
        return Napi::Boolean::New(env, cppResult);
      }
  );
}

Napi::Value napi_SetAutoPairing(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetAutoPairing(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_IsPairingListSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info, 
    [functionName](unsigned short deviceId) {
      bool pairingListSupported = Jabra_IsPairingListSupported(deviceId);
      return pairingListSupported;
    }, 
    [](const Napi::Env& env, const bool cppResult) { 
        return Napi::Boolean::New(env, cppResult);
      }
  );
}

Napi::Value napi_GetPairingList(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Object, Jabra_PairingList *>(functionName, info, 
    [functionName](unsigned short deviceId) {
      return Jabra_GetPairingList(deviceId);
    },
    [](const Napi::Env& env, const Jabra_PairingList *pairingList) { 
      Napi::Array array = Napi::Array::New(env);

      if (pairingList!=nullptr) {
        for (unsigned short i = 0; i < pairingList->count; i++)
        {
          Napi::Object item = Napi::Object::New(env);

          item.Set(Napi::String::New(env, "deviceName"), (Napi::String::New(env, pairingList->pairedDevice[i].deviceName)));
          item.Set(Napi::String::New(env, "deviceBTAddr"), (Napi::String::New(env, toBTAddrString(pairingList->pairedDevice[i].deviceBTAddr, sizeof(pairingList->pairedDevice[i].deviceBTAddr)/sizeof(uint8_t)))));
          item.Set(Napi::String::New(env, "isConnected"), (Napi::Boolean::New(env, pairingList->pairedDevice[i].isConnected)));

          array.Set(i, item);
        }
      }

      return array;
    },
    [](Jabra_PairingList *pairingList) {
      if (pairingList!=nullptr) {
        Jabra_FreePairingList(pairingList);
      }
    }
  );
}

Napi::Value napi_SetBTPairing(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_SetBTPairing(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_StopBTPairing(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_StopBTPairing(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_ClearPairingList(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_ClearPairingList(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_SearchNewDevices(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_SearchNewDevices(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_ConnectBTDevice(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_ConnectBTDevice(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_GetConnectedBTDeviceName(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info, [functionName](unsigned short deviceId) {
    if (char * result = Jabra_GetConnectedBTDeviceName(deviceId)) {
      std::string managedResult(result);
      Jabra_FreeString(result);
      return managedResult;
    } else {
      util::JabraException::LogAndThrow(functionName, "Jabra_GetConnectedBTDeviceName return null");
      return std::string(); // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, const std::string& cppResult) {  return Napi::String::New(env, cppResult); });
}

Napi::Value napi_ClearPairedDevice(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::STRING, util::BOOLEAN, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    std::string deviceName = info[1].As<Napi::String>();
    const std::string deviceBTAddr = info[2].As<Napi::String>();
    const bool isConnected = info[3].As<Napi::Boolean>().ToBoolean();
    const Napi::Function javascriptResultCallback = info[4].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, deviceName, deviceBTAddr, isConnected](){ 
        Jabra_PairedDevice pDevice;
        pDevice.deviceName = (char *)deviceName.c_str(); // This ought to be safe as Jabra_ClearPairedDevice should not change this.
        pDevice.isConnected = isConnected;
        toBTAddr(pDevice.deviceBTAddr, deviceBTAddr, sizeof(pDevice.deviceBTAddr)/sizeof(uint8_t));

        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_ClearPairedDevice(deviceId, &pDevice)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetSearchDeviceList(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Object, Jabra_PairingList *>(functionName, info, 
    [functionName](unsigned short deviceId) {
      return Jabra_GetSearchDeviceList(deviceId);
    },
    [](const Napi::Env& env, const Jabra_PairingList *searchDeviceList) { 
      Napi::Array array = Napi::Array::New(env);
      if (searchDeviceList!=nullptr)
      for (unsigned short i = 0; i < searchDeviceList->count; i++)
      {
        Napi::Object item = Napi::Object::New(env);

        item.Set(Napi::String::New(env, "deviceName"), (Napi::String::New(env, searchDeviceList->pairedDevice[i].deviceName)));
        item.Set(Napi::String::New(env, "deviceBTAddr"), (Napi::String::New(env, toBTAddrString(searchDeviceList->pairedDevice[i].deviceBTAddr, sizeof(searchDeviceList->pairedDevice[i].deviceBTAddr)/sizeof(uint8_t)))));
        item.Set(Napi::String::New(env, "isConnected"), (Napi::Boolean::New(env, searchDeviceList->pairedDevice[i].isConnected)));

        array.Set(i, item);
      }

      return array;
    },
    [](Jabra_PairingList *searchDeviceList) {
      if (searchDeviceList!=nullptr) {
        Jabra_FreePairingList(searchDeviceList);
      }
    }
  );
}

Napi::Value napi_BTLinkQualityChangeEventEnabled(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable)
  {
    LinkQualityStatusListener callback = nullptr;
    std::cout << "napi_BTLinkQualityChangeEventEnabled" << std::endl;
    if (enable) callback = internalCallbackManager::getLinkQualityCallback();
    const Jabra_ReturnCode result = Jabra_SetLinkQualityStatusListener(deviceId, callback);
    if (result != Return_Ok) {
      throw util::JabraReturnCodeException(functionName, result);
    }
  });
}
