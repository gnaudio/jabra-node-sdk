#include "fwu.h"

Napi::Value napi_DownloadFirmware(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string version = info[1].As<Napi::String>();
    const std::string authorization = info[2].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, version, authorization](){ 
        Jabra_ReturnCode ret = Jabra_DownloadFirmware(deviceId, version.c_str(), authorization.c_str());
        if (ret != Return_Async && ret != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, ret);
        }
      }
    ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_UpdateFirmware(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string firmFile = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, firmFile](){ 
        Jabra_ReturnCode ret = Jabra_UpdateFirmware(deviceId, firmFile.c_str());
        if (ret != Return_Async && ret != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, ret);
        }
      }
    ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_DownloadFirmwareUpdater(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string authorization = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, authorization](){ 
        Jabra_ReturnCode ret = Jabra_DownloadFirmwareUpdater(deviceId, authorization.c_str());
        if (ret != Return_Async && ret != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, ret);
        }
      }
    ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetFirmwareFilePath(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string version = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<std::string, Napi::String>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, version](){ 
        if (char * result = Jabra_GetFirmwareFilePath(deviceId, version.c_str())) {
          std::string managedResult(result);
          Jabra_FreeString(result);
          return util::toUtf8(managedResult, functionName);
        }
        throw util::JabraException(functionName, "Jabra_GetFirmwareFilePath yielded no result");
      },
      [](const Napi::Env& env, const std::string filePath) { 
        Napi::String napiResult = Napi::String::New(env, filePath.c_str());
        return napiResult;
      }))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_IsFirmwareLockEnabled(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsFirmwareLockEnabled(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_EnableFirmwareLock(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_EnableFirmwareLock(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_GetFirmwareVersion(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info, 
    [functionName](unsigned short deviceId) {
      char buf[64];
      Jabra_ReturnCode retv;
      if ((retv = Jabra_GetFirmwareVersion(deviceId, &buf[0], sizeof(buf))) == Return_Ok) {
        return std::string(buf);
      } else {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        return std::string(); // Dummy return - avoid compiler warnings.
      }
    }, 
    [](const Napi::Env& env, const std::string cppResult) {  return Napi::String::New(env, cppResult); }
  );
}

Napi::Value napi_GetFirmwareVersionBundle(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, FirmwareVersionBundle>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            FirmwareVersionBundle versionBundle{};
            Jabra_ReturnCode retCode = Jabra_GetFirmwareVersionBundle(deviceId, versionBundle.parent, versionBundle.child, FIRMWARE_VERSION_MAX_LENGTH);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return versionBundle;
        },
        [](const Napi::Env& env, const FirmwareVersionBundle versionBundle) {
            auto firmwareBundle = Napi::Object::New(env);

            firmwareBundle.Set("parent", Napi::String::New(env, versionBundle.parent));
            firmwareBundle.Set("child", Napi::String::New(env, versionBundle.child));

            return firmwareBundle;
        }
    );
}

Napi::Value napi_GetRemoteControlFirmwareVersion(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            char version[100]{};
            Jabra_ReturnCode retCode = Jabra_GetRemoteControlFirmwareVersion(deviceId, &version[0], sizeof(version));

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }

            return std::string(version);
        },
        [](const Napi::Env& env, const std::string fwVersion) { return Napi::String::New(env, fwVersion); }
    );
}

Napi::Value napi_GetLatestFirmwareInformation(const Napi::CallbackInfo& info) {  
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string authorizationId = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<Jabra_FirmwareInfo *, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, authorizationId](){ 
        Jabra_FirmwareInfo * fwInfo = Jabra_GetLatestFirmwareInformation(deviceId, authorizationId.c_str());

        if (!fwInfo) {
          util::JabraException::LogAndThrow(functionName, "null returned");
        }
        
        return fwInfo;
      },
      [](const Napi::Env& env, Jabra_FirmwareInfo *fwInfo) { 
        Napi::Object napiResult = Napi::Object::New(env);

        if (fwInfo) {
          if (fwInfo->version) {
            napiResult.Set(Napi::String::New(env, "version"), (Napi::String::New(env, fwInfo->version)));
          }
          if (fwInfo->fileSize) {
            napiResult.Set(Napi::String::New(env, "fileSize"), (Napi::String::New(env, fwInfo->fileSize)));
          }
          if (fwInfo->releaseDate) {
            napiResult.Set(Napi::String::New(env, "releaseDate"), (Napi::String::New(env, fwInfo->releaseDate)));
          }
          if (fwInfo->stage) {
            napiResult.Set(Napi::String::New(env, "stage"), (Napi::String::New(env, fwInfo->stage)));     
          } 
          if (fwInfo->releaseNotes) { 
            napiResult.Set(Napi::String::New(env, "releaseNotes"), (Napi::String::New(env, (const char16_t*)fwInfo->releaseNotes)));
          }
        }

        return napiResult;
      }, [](Jabra_FirmwareInfo * fwInfo) {
        if (fwInfo) {
          Jabra_FreeFirmwareInfo(fwInfo);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}  

Napi::Value napi_CancelFirmwareDownload(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_CancelFirmwareDownload(deviceId)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_CheckForFirmwareUpdate(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string authorizationId = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<bool, Napi::Boolean>(
      functionName, 
      javascriptResultCallback,
      [functionName,deviceId,authorizationId](){
        Jabra_ReturnCode retv = Jabra_CheckForFirmwareUpdate(deviceId, authorizationId.c_str());

        if (retv == Firmware_Available || retv == Return_Ok) {
          return true;
        } else if (retv == Firmware_UpToDate) {
          return false;
        } else {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }   

        return false; // Make compiler happy.
      },
      [](const Napi::Env& env, const bool result) {
        return Napi::Boolean::New(env, result);
      }
    ))->Queue();
  }

  return env.Undefined();
}
