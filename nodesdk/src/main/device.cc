#include "device.h"
#include "napiutil.h"
#include <string.h>



Napi::Value napi_GetDeviceImagePath(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info, [functionName](unsigned short deviceId) {
    if (char * result = Jabra_GetDeviceImagePath(deviceId)) {
      std::string managedResult(result);
      Jabra_FreeString(result);
      return util::toUtf8(managedResult, functionName);
    } else {
      util::JabraException::LogAndThrow(functionName, "null returned");
      return std::string(); // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, const std::string& cppResult) { 
    return Napi::String::New(env, cppResult); 
  });
}

Napi::Value napi_GetDeviceImageThumbnailPath(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info, [functionName](unsigned short deviceId) {
    if (char * result = Jabra_GetDeviceImageThumbnailPath(deviceId)) {
      std::string managedResult(result);
      Jabra_FreeString(result);
      return util::toUtf8(managedResult, functionName);
    } else {
      util::JabraException::LogAndThrow(functionName, "null returned");
      return std::string(); // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, const std::string& cppResult) {  return Napi::String::New(env, cppResult); });
}

Napi::Value napi_IsGnHidStdHidSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsGnHidStdHidSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_GetHidWorkingState(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Number, Jabra_HidState>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
    Jabra_HidState state;
    if ((retv = Jabra_GetHidWorkingState(deviceId, &state)) == Return_Ok) {
      return state;
    } else {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
      return state; // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, Jabra_HidState cppResult) {  return Napi::Number::New(env, cppResult); });
}

Napi::Value napi_SetHidWorkingState(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_HidState state = (Jabra_HidState)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, state](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetHidWorkingState(deviceId, state)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_IsRingerSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsRingerSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_SetRinger(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetRinger(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_IsOffHookSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsOffHookSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}


Napi::Value napi_SetOffHook(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetOffHook(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_IsOnlineSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsOnlineSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsMuteSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsMuteSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsHoldSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsHoldSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsBusyLightSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsBusylightSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_UploadRingtone(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string fileName = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, fileName](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_UploadRingtone(deviceId, fileName.c_str())) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_UploadWavRingtone(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string fileName = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, fileName](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_UploadWavRingtone(deviceId, fileName.c_str())) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_UploadImage(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string fileName = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, fileName](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_UploadImage(deviceId, fileName.c_str())) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetNamedAsset(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string assetName = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<CNamedAsset*, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, assetName](){ 
        Jabra_ReturnCode retv;
        CNamedAsset* asset = nullptr;
        if ((retv = Jabra_GetNamedAsset(deviceId, assetName.c_str(), &asset)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
        return asset;
      },
      [](const Napi::Env& env, CNamedAsset* asset) {
        Napi::Object jResult = Napi::Object::New(env);

        Napi::Array jElements = Napi::Array::New(env);

        if (asset != nullptr) {
          for (unsigned int i=0; i<asset->element_count; ++i) {
            CAssetElement src = asset->elements[i];
            Napi::Object jElement = Napi::Object::New(env);
            jElement.Set(Napi::String::New(env, "url"), Napi::String::New(env, src.url ? src.url : ""));
            jElement.Set(Napi::String::New(env, "mime"), Napi::String::New(env, src.mime ? src.mime : ""));
            jElements.Set(i, jElement);
          }
        }

        jResult.Set(Napi::String::New(env, "elements"), jElements);

        Napi::Array jMetadata = Napi::Array::New(env);
        if (asset != nullptr) {
          for (unsigned int i=0; i<asset->metadata_count; ++i) {
            CAssetMetadata src = asset->metadata[i];
            Napi::Object jSingleMetaData = Napi::Object::New(env);
            jSingleMetaData.Set(Napi::String::New(env, "name"), Napi::String::New(env, src.name ? src.name : ""));
            jSingleMetaData.Set(Napi::String::New(env, "value"), Napi::String::New(env, src.value ? src.value : ""));
            jMetadata.Set(i, jSingleMetaData);
          }
        }

        jResult.Set(Napi::String::New(env, "metadata"), jMetadata);

        return jResult;
      },
      [](CNamedAsset* asset) {
        if (asset != nullptr) {
          Jabra_FreeAsset(asset);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetSupportedButtonEvents(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<ButtonEvent*, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId](){ 
        ButtonEvent* buttonEvent = nullptr;
        buttonEvent = Jabra_GetSupportedButtonEvents(deviceId);
        return buttonEvent;
      },
      [](const Napi::Env& env, ButtonEvent* buttonEvent) {

        Napi::Array jElements = Napi::Array::New(env);

        if (buttonEvent != nullptr) { 
          for ( int i=0; i<buttonEvent->buttonEventCount; ++i) {
            ButtonEventInfo btnEventInfo = buttonEvent->buttonEventInfo[i];
            Napi::Object jElement = Napi::Object::New(env);
            jElement.Set(Napi::String::New(env, "buttonTypeKey"), Napi::Number::New(env, btnEventInfo.buttonTypeKey));
            jElement.Set(Napi::String::New(env, "buttonTypeValue"), Napi::String::New(env, btnEventInfo.buttonTypeValue ? btnEventInfo.buttonTypeValue : ""));
            Napi::Array newElements = Napi::Array::New(env);
            for (unsigned int j=0; j<(unsigned int)btnEventInfo.buttonEventTypeSize; ++j) {
              ButtonEventType btnEventTypeInfo = btnEventInfo.buttonEventType[j];
              Napi::Object newElement = Napi::Object::New(env);
              newElement.Set(Napi::String::New(env, "key"), Napi::Number::New(env, btnEventTypeInfo.key));
              newElement.Set(Napi::String::New(env, "value"), Napi::String::New(env, btnEventTypeInfo.value ? btnEventTypeInfo.value : ""));
              
              newElements.Set(j, newElement);
            }
            jElements.Set(i, jElement);
          }
        }
        return jElements;
        
      },
      [](ButtonEvent* buttonEvent) {
        if (buttonEvent != nullptr) {
          Jabra_FreeButtonEvents(buttonEvent);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
  
}

Napi::Value napi_SetMute(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetMute(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_IsSetDateTimeSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsSetDateTimeSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsFeatureSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const DeviceFeature feature = (DeviceFeature)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<bool, Napi::Boolean>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, feature](){ 
        return Jabra_IsFeatureSupported(deviceId, feature);
      },
      [](const Napi::Env& env, const bool result) {
        return Napi::Boolean::New(env, result);
      }
    ))->Queue();
  }

  return env.Undefined();
}


Napi::Value napi_IsEqualizerSupported(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsEqualizerSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsEqualizerEnabled(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsEqualizerEnabled(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) {  return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_EnableEqualizer(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_EnableEqualizer(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_SetEqualizerParameters(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::ARRAY, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());   
    Napi::Array bands =info[1].As<Napi::Array>();

    const int nbands = bands.Length();
    std::vector<float> managedBands(nbands);
    for (int i=0; i<nbands; ++i) {
      managedBands[i] = bands.Get(i).ToNumber().FloatValue();
    }

    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, managedBands](){ 
        Jabra_ReturnCode retv;       
        float * bands = const_cast<float*>(managedBands.data()); // Should be safe as SDK ought not to change data.
        const int nbands = managedBands.size();
        if ((retv = Jabra_SetEqualizerParameters(deviceId, bands, nbands)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_SetTimestamp(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const uint32_t newTime = (unsigned int)(info[1].As<Napi::Number>().Int32Value());//Int64Value() should be used once Jabra sdk supports it.
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName,deviceId,newTime](){ 
        Jabra_ReturnCode retv;   
        if ((retv = Jabra_SetTimestamp(deviceId, newTime)) != Return_Ok) {                    
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_PlayRingTone(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const uint8_t level = (unsigned short)(info[1].As<Napi::Number>().Int32Value());
    const uint8_t type = (unsigned short)(info[2].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName,deviceId,level,type](){ 
        Jabra_ReturnCode retv;   
        if ((retv = Jabra_PlayRingtone(deviceId,level,type)) != Return_Ok) {                    
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetESN(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info, 
    [functionName](unsigned short deviceId) {
      char esn[64];
      Jabra_ReturnCode retv;
      if ((retv = Jabra_GetESN(deviceId, &esn[0], sizeof(esn))) == Return_Ok) {
        return std::string(esn);
      } else {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        return std::string(); // Dummy return - avoid compiler warnings.
      }
    }, 
    [](const Napi::Env& env, const std::string& cppResult) {  return Napi::String::New(env, cppResult); 
    }
  );
}

Napi::Value napi_GetTimestamp(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<uint32_t, Napi::Number>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId](){ 
        Jabra_ReturnCode retv;  
        uint32_t result;
        if ((retv = Jabra_GetTimestamp(deviceId, &result)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
        return result;
      },
      [](const Napi::Env& env, const uint32_t cppResult) {  
        return Napi::Number::New(env, cppResult); 
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetAudioFileParametersForUpload(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Object, Jabra_AudioFileParams >(functionName, info, 
    [functionName](unsigned short deviceId) {
      return Jabra_GetAudioFileParametersForUpload(deviceId);
    },
    [](const Napi::Env& env, const Jabra_AudioFileParams& audioFileParams) { 
      Napi::Object result = Napi::Object::New(env);
      result.Set(Napi::String::New(env, "audioFileType"), (Napi::Number::New(env, audioFileParams.audioFileType)));
      result.Set(Napi::String::New(env, "numChannels"), (Napi::Number::New(env, audioFileParams.numChannels)));
      result.Set(Napi::String::New(env, "bitsPerSample"), (Napi::Number::New(env,audioFileParams.bitsPerSample)));
      result.Set(Napi::String::New(env, "sampleRate"), (Napi::Number::New(env,audioFileParams.sampleRate)));
      result.Set(Napi::String::New(env, "maxFileSize"), (Napi::Number::New(env,audioFileParams.maxFileSize)));
      return result;
    }
  );
}

Napi::Value napi_GetSecureConnectionMode(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Number, Jabra_SecureConnectionMode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
    Jabra_SecureConnectionMode secureConnectionModes;
    if ((retv = Jabra_GetSecureConnectionMode(deviceId, &secureConnectionModes)) == Return_Ok) {
      return secureConnectionModes;
    } else {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
      return secureConnectionModes; // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, Jabra_SecureConnectionMode cppResult) {  return Napi::Number::New(env, cppResult); });
}

Napi::Value napi_RebootDevice(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
     if ((retv = Jabra_RebootDevice(deviceId)) != Return_Ok) {
        util::JabraReturnCodeException::LogAndThrow(functionName, retv);
     }
     return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode retv) {  return env.Undefined(); });
}

Napi::Value napi_GetWizardMode(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Number, WizardModes>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
    WizardModes wizardModes;
    if ((retv = Jabra_GetWizardMode(deviceId, &wizardModes)) == Return_Ok) {
      return wizardModes;
    } else {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
      return wizardModes; // Dummy return - avoid compiler warnings.
    }
  }, [](const Napi::Env& env, WizardModes cppResult) {  return Napi::Number::New(env, cppResult); });
}

Napi::Value napi_SetWizardMode(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const WizardModes modes = (WizardModes)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId,modes](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetWizardMode(deviceId,modes)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

/**
 * Convert a napi Button event object to a native sdk ButtonEvent object.
 * 
 * Nb. Use Custom_FreeButtonEvents to free memory allocated by this function.
 */
static ButtonEvent *toButtonEventCType(const Napi::Array& src) {
    ButtonEvent * result = new ButtonEvent();
  
    result->buttonEventCount = src.Length();
    result->buttonEventInfo = new ButtonEventInfo[src.Length()];

    for ( int i=0; i<result->buttonEventCount; ++i) {
        ButtonEventInfo& btnEventDst = result->buttonEventInfo[i];
        Napi::Object btnEventSrc = src.Get(i).ToObject();

        btnEventDst.buttonTypeKey = util::getObjInt32OrDefault(btnEventSrc, "buttonTypeKey", -1);
        btnEventDst.buttonTypeValue = util::newCString(util::getObjStringOrDefault(btnEventSrc, "buttonTypeValue", ""));

        Napi::Array buttonEventTypeArray = btnEventSrc.Get("buttonEventType").As<Napi::Array>();

        btnEventDst.buttonEventTypeSize = buttonEventTypeArray.Length();
        btnEventDst.buttonEventType = new ButtonEventType[buttonEventTypeArray.Length()];

        if (buttonEventTypeArray.IsArray()) {
            for (int j=0; j<btnEventDst.buttonEventTypeSize; ++j) {
                ButtonEventType& btnEventTypeDst = btnEventDst.buttonEventType[j];
                Napi::Object btnEventTypeSrcObj = buttonEventTypeArray.Get(j).As<Napi::Object>();

                btnEventTypeDst.key = util::getObjInt32OrDefault(btnEventTypeSrcObj, "key", 0);
                btnEventTypeDst.value = util::newCString(util::getObjStringOrDefault(btnEventTypeSrcObj, "value", ""));
            }
        } else {
            btnEventDst.buttonEventTypeSize = 0;
            btnEventDst.buttonEventType = nullptr;
        }
    }

    return result;
}

static void Custom_FreeButtonEvent(ButtonEvent* buttonEvent) {
  ButtonEventInfo *btnEventInfos = buttonEvent->buttonEventInfo;
  if (btnEventInfos != nullptr) {
    for (int i = 0; i < buttonEvent->buttonEventCount; i++) {
      ButtonEventInfo& btnEventInfo = btnEventInfos[i];

      if (btnEventInfo.buttonTypeValue != nullptr) {
        delete[] btnEventInfo.buttonTypeValue;
      }

      ButtonEventType *btnEventTypes = btnEventInfo.buttonEventType;
      if (btnEventTypes != nullptr) {
        for (int j = 0; j < btnEventInfo.buttonEventTypeSize; j++) {
            ButtonEventType& btnEventType =  btnEventTypes[j];
            if (btnEventType.value != nullptr) {
              delete[] btnEventType.value;
            }
        }

        delete[] btnEventTypes;
      }
    }
    
    delete[] btnEventInfos;
  }

  delete buttonEvent;
}

// Common implementation for get/release focus functions:
Napi::Value doGetReleaseButtonFocus(const Napi::CallbackInfo& info, const char * const functionName, const GetReleaseButtonFocusEnum getOrRelease) {
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::ARRAY, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Array btnEvents = info[1].As<Napi::Array>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    ButtonEvent *rawButtonEvent = toButtonEventCType(btnEvents);

    (new util::JAsyncWorker<void, void>(
      functionName,
      javascriptResultCallback,
      [functionName, deviceId, rawButtonEvent, getOrRelease](){
        Jabra_ReturnCode retv;
        if (getOrRelease == GetReleaseButtonFocusEnum::GET_FOCUS) { 
          retv = Jabra_GetButtonFocus(deviceId, rawButtonEvent);
        } else if (getOrRelease == GetReleaseButtonFocusEnum::RELEASE_FOCUS) {
          retv = Jabra_ReleaseButtonFocus(deviceId, rawButtonEvent);
        } else {
          retv = Jabra_ReturnCode::Not_Supported;
        }

        if (retv != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }, [rawButtonEvent]() {
        if (rawButtonEvent) {
          Custom_FreeButtonEvent(rawButtonEvent);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetButtonFocus(const Napi::CallbackInfo& info) {
  return doGetReleaseButtonFocus(info, __func__, GetReleaseButtonFocusEnum::GET_FOCUS);
}


Napi::Value napi_ReleaseButtonFocus(const Napi::CallbackInfo& info) {
   return doGetReleaseButtonFocus(info, __func__, GetReleaseButtonFocusEnum::RELEASE_FOCUS);
}

Napi::Value napi_SetDatetime(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::OBJECT, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());

    Napi::Object dateTimeJsObj = info[1].As<Napi::Object>();
  
    timedate_t dateTime;
    dateTime.sec = util::getObjInt32OrDefault(dateTimeJsObj, "sec", 0);
    dateTime.min = util::getObjInt32OrDefault(dateTimeJsObj, "min", 0);
    dateTime.hour = util::getObjInt32OrDefault(dateTimeJsObj, "hour", 0);
    dateTime.mday = util::getObjInt32OrDefault(dateTimeJsObj, "mday", 0);
    dateTime.mon = util::getObjInt32OrDefault(dateTimeJsObj, "mon", 0);
    dateTime.year = util::getObjInt32OrDefault(dateTimeJsObj, "year", 0);
    dateTime.wday = util::getObjInt32OrDefault(dateTimeJsObj, "wday", 0);    

    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName,
      javascriptResultCallback,
      [functionName, deviceId, dateTime](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetDateTime(deviceId, &dateTime)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetSupportedFeatures(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<FeatureListCountPair, Napi::Array>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId](){
        FeatureListCountPair flcPair;
        flcPair.featureList = Jabra_GetSupportedFeatures(deviceId, &flcPair.featureCount);
        return flcPair;
      },
      [](const Napi::Env& env, const FeatureListCountPair& flcPair) {

        Napi::Array jElements = Napi::Array::New(env);

        if (flcPair.featureList != nullptr) { 
          for (unsigned int i=0; i<flcPair.featureCount; ++i) {
            Napi::Number feature = Napi::Number::New(env, (uint32_t)flcPair.featureList[i]);
            jElements.Set(i, feature);
          }
        }

        return jElements;        
      },
      [](const FeatureListCountPair& flcPair) {
        if (flcPair.featureList != nullptr) {
          Jabra_FreeSupportedFeatures(flcPair.featureList);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetEqualizerParameters(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
	  const unsigned int maxNbands = (unsigned short)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<EqualizerBandsListCountPair, Napi::Array>(
      functionName,
      javascriptResultCallback,
      [functionName, deviceId, maxNbands](){
        Jabra_ReturnCode retv;
        
        EqualizerBandsListCountPair pair;
        pair.bands = new Jabra_EqualizerBand[maxNbands > 0 ? maxNbands : 1];
        pair.bandsCount = maxNbands;
        if ((retv = Jabra_GetEqualizerParameters(deviceId, pair.bands, &pair.bandsCount)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
        return pair;
      }, [](const Napi::Env& env, const EqualizerBandsListCountPair& pair) {
        Napi::Array result = Napi::Array::New(env);

        for (unsigned int i=0; i<pair.bandsCount; ++i) {
          Jabra_EqualizerBand& src = pair.bands[i];

          Napi::Object newElement = Napi::Object::New(env);

          newElement.Set(Napi::String::New(env, "maxGain"), Napi::Number::New(env, src.max_gain));
          newElement.Set(Napi::String::New(env, "centerFrequency"), Napi::Number::New(env, src.centerFrequency));
          newElement.Set(Napi::String::New(env, "currentGain"), Napi::Number::New(env, src.currentGain));
               
          result.Set(i, newElement);
        }

        return result;
      }, [](const EqualizerBandsListCountPair& pair) {
        delete[] pair.bands;
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetRemoteMmiFocus(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  bool argsOk = util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::NUMBER, util::NUMBER, util::FUNCTION});

  if (argsOk) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const RemoteMmiType type = (RemoteMmiType)(info[1].As<Napi::Number>().Int32Value());
    const RemoteMmiInput input = (RemoteMmiInput)(info[2].As<Napi::Number>().Int32Value());
    const RemoteMmiPriority prio = (RemoteMmiPriority)(info[3].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[4].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, type, input, prio](){ 
        Jabra_ReturnCode retv = Jabra_GetRemoteMmiFocus(deviceId, type, input, prio);     

        if (retv != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        } 
      }
    ))->Queue();     
  } 

  return env.Undefined();
}

Napi::Value napi_ReleaseRemoteMmiFocus(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  bool argsOk = util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION});

  if (argsOk) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const RemoteMmiType type = (RemoteMmiType)(info[1].As<Napi::Number>().Int32Value());  
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();  

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, type](){ 
        Jabra_ReturnCode retv = Jabra_ReleaseRemoteMmiFocus(deviceId, type);     

        if (retv != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        } 
      }
    ))->Queue();         
  }

  return env.Undefined();
}

Napi::Value napi_IsRemoteMmiInFocus(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  bool argsOk = util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION});

  if (argsOk) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const RemoteMmiType type = (RemoteMmiType)(info[1].As<Napi::Number>().Int32Value());  
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();  

    (new util::JAsyncWorker<bool, Napi::Boolean>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, type](){ 
        bool isInFocus;

        Jabra_ReturnCode retv = Jabra_IsRemoteMmiInFocus(deviceId, type, &isInFocus);     

        if (retv != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        } 

        return isInFocus;
      }, [](const Napi::Env& env, bool isInFocus){
        return Napi::Boolean::New(env, isInFocus);
      }     
    ))->Queue();         
  }

  return env.Undefined();
}

Napi::Value napi_SetRemoteMmiAction(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();
  bool argsOk = util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::OBJECT, util::FUNCTION});

  if (argsOk) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const RemoteMmiType type = (RemoteMmiType)(info[1].As<Napi::Number>().Int32Value());  
    Napi::Object actionOutputArgs = info[2].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();  

    RemoteMmiActionOutput actionOutput;
    actionOutput.red = util::getObjInt32OrDefault(actionOutputArgs, "red", 0);
    actionOutput.green = util::getObjInt32OrDefault(actionOutputArgs, "green", 0);
    actionOutput.blue = util::getObjInt32OrDefault(actionOutputArgs, "blue", 0);
    actionOutput.sequence = util::getObjEnumValueOrDefault<RemoteMmiSequence>(actionOutputArgs, "sequence", RemoteMmiSequence::MMI_LED_SEQUENCE_OFF);   

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, type, actionOutput](){         
        Jabra_ReturnCode retv = Jabra_SetRemoteMmiAction(deviceId, type, actionOutput);     

        if (retv != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        } 
      }     
    ))->Queue();         
  }

  return env.Undefined();
}
