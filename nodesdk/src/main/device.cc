#include "device.h"
#include "napiutil.h"
#include <string.h>
#include <ctime>
#include <cstring>

// ----------------------------------------- Helper functions ------------------------------------------------

static Napi::Object makePoint2D(const Napi::Env& env, const uint16_t x, const uint16_t y) {
    Napi::Object point2D = Napi::Object::New(env);
    point2D.Set("x", x);
    point2D.Set("y", y);
    return point2D;
};

// ----------------------------------------- Napi functions  ------------------------------------------------

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

Napi::Value napi_SetVideoMode(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_VideoMode mode = (Jabra_VideoMode)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, mode](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetVideoMode(deviceId, mode)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetVideoMode(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, Jabra_VideoMode>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            Jabra_VideoMode mode;
            Jabra_ReturnCode retCode = Jabra_GetVideoMode(deviceId, &mode);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
            return mode;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetWhiteboardOnMainStream(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetWhiteboardOnMainStream(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_GetWhiteboardOnMainStream(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            bool enable;
            Jabra_ReturnCode retCode = Jabra_GetWhiteboardOnMainStream(deviceId, &enable);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }
            return enable;
        },
        Napi::Boolean::New
    );
}

Napi::Value napi_IsCameraStreaming(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            bool streamingStatus;
            Jabra_ReturnCode retCode = Jabra_IsCameraStreaming(deviceId, &streamingStatus);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }
            return streamingStatus;
        },
        Napi::Boolean::New
    );
}

Napi::Value napi_IsVideoDeviceStreaming(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Object, std::pair<bool, bool>>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
    bool videoStreaming = false, audioStreaming = false;
    if ((retv = Jabra_IsVideoDeviceStreaming(deviceId, &videoStreaming, &audioStreaming)) != Return_Ok) {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
    }
    return std::make_pair(videoStreaming, audioStreaming);
  }, [](const Napi::Env& env, std::pair<bool, bool> streamingStatus) {
    Napi::Object jsStreamingStatus = Napi::Object::New(env);
    jsStreamingStatus.Set(Napi::String::New(env, "videoStreamingStatus"), Napi::Boolean::New(env, streamingStatus.first));
    jsStreamingStatus.Set(Napi::String::New(env, "audioStreamingStatus"), Napi::Boolean::New(env, streamingStatus.second));
    return jsStreamingStatus;
    });
}

Napi::Value napi_SetPictureInPicture(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](const unsigned short deviceId, const bool enable) {
        const Jabra_ReturnCode result = Jabra_SetPictureInPicture(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_GetPictureInPicture(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info, [functionName](const unsigned short deviceId) {
    Jabra_ReturnCode retv;
    bool enable = false;
    if ((retv = Jabra_GetPictureInPicture(deviceId, &enable)) != Return_Ok) {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
    }
    return enable;
  }, Napi::Boolean::New
  );
}

Napi::Value napi_IsCertifiedForSkypeForBusiness(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            bool certified = Jabra_IsCertifiedForSkypeForBusiness(deviceId);
            return certified;
        },
        Napi::Boolean::New
    );
}

Napi::Value napi_GetLock(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                Jabra_ReturnCode retCode = Jabra_GetLock(deviceId);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_ReleaseLock(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                Jabra_ReturnCode retCode = Jabra_ReleaseLock(deviceId);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_IsLocked(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<bool, Napi::Boolean>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                bool retCode = Jabra_IsLocked(deviceId);
                return retCode;
            }, 
            [](const Napi::Env& env, bool isLocked){
              return Napi::Boolean::New(env, isLocked);
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

Napi::Value napi_SetIntelligentZoomLatency(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const int latency = info[1].As<Napi::Number>().Int32Value();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, latency](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetIntelligentZoomLatency(deviceId, latency)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetIntelligentZoomLatency(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Number, int>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
    int latency;
    if ((retv = Jabra_GetIntelligentZoomLatency(deviceId, &latency)) != Return_Ok)
    {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
    }
    return latency;
  }, [](const Napi::Env& env, int latency) {  return Napi::Number::New(env, latency); });
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

    Napi::Object timeInfo = info[1].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    timedate_t dateTime;
    dateTime.sec = util::getObjInt32OrDefault(timeInfo, "sec", 0);
    dateTime.min = util::getObjInt32OrDefault(timeInfo, "min", 0);
    dateTime.hour = util::getObjInt32OrDefault(timeInfo, "hour", 0);
    dateTime.mday = util::getObjInt32OrDefault(timeInfo, "mday", 0);
    dateTime.mon = util::getObjInt32OrDefault(timeInfo, "mon", 0);
    dateTime.year = util::getObjInt32OrDefault(timeInfo, "year", 0);
    dateTime.wday = util::getObjInt32OrDefault(timeInfo, "wday", 0);
    
    const bool setCurrentTime =
      dateTime.sec==0 &&
      dateTime.min==0 &&
      dateTime.hour==0 &&
      dateTime.mday==0 &&
      dateTime.mon==0 &&
      dateTime.year==0 &&
      dateTime.wday==0;
    
    (new util::JAsyncWorker<void, void>(
      functionName,
      javascriptResultCallback,
      [functionName, deviceId, dateTime, setCurrentTime](){ 
        Jabra_ReturnCode retv;

        // If input is all zeros, set current time
        const timedate_t* ptrDateTime = (setCurrentTime) ? NULL : &dateTime;
        if ((retv = Jabra_SetDateTime(deviceId, ptrDateTime)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetDatetime(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Object, timedate_t>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;
    timedate_t timeDate = {};
    if ((retv = Jabra_GetDateTime(deviceId, &timeDate)) == Return_Ok) {
      return timeDate;
    }
    else {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
      return timeDate;
    }
  }, [](const Napi::Env& env, timedate_t timeDate) {
    Napi::Object jsDate = Napi::Object::New(env);
    jsDate.Set(Napi::String::New(env, "sec"), Napi::Number::New(env, timeDate.sec));
    jsDate.Set(Napi::String::New(env, "min"), Napi::Number::New(env, timeDate.min));
    jsDate.Set(Napi::String::New(env, "hour"), Napi::Number::New(env, timeDate.hour));
    jsDate.Set(Napi::String::New(env, "mday"), Napi::Number::New(env, timeDate.mday));
    jsDate.Set(Napi::String::New(env, "mon"), Napi::Number::New(env, timeDate.mon));
    jsDate.Set(Napi::String::New(env, "year"), Napi::Number::New(env, timeDate.year));
    jsDate.Set(Napi::String::New(env, "wday"), Napi::Number::New(env, timeDate.wday));
    return jsDate;
    });
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

Napi::Value napi_IsRemoteManagementEnabled(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(functionName, info,
        [functionName](unsigned short deviceId) {
            bool result = false;
            Jabra_ReturnCode retCode = Jabra_IsRemoteManagementEnabled(deviceId, &result);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }

            return result;
        }, Napi::Boolean::New);
}

Napi::Value napi_EnableRemoteManagement(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::BOOLEAN, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const bool enable = info[1].As<Napi::Boolean>();
        const unsigned int timeout = info[2].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, enable, timeout](){
                const Jabra_ReturnCode retCode = Jabra_EnableRemoteManagement(deviceId, enable, timeout);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }
    return env.Undefined();
}

Napi::Value napi_SetXpressUrl(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const std::string url = info[1].As<Napi::String>();
        const unsigned int timeout = info[2].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, url, timeout](){
                Jabra_ReturnCode retCode = Jabra_SetXpressUrl(deviceId, url.c_str(), timeout);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_ConfigureXpressManagement(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::OBJECT, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const std::string xpressurl = info[1].As<Napi::String>();
        Napi::Object proxy = info[2].As<Napi::Object>();
        const unsigned int timeout = info[3].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[4].As<Napi::Function>();

        const std::string proxyurl = util::getObjStringOrDefault(proxy, "url", "");
        const std::string proxyusername = util::getObjStringOrDefault(proxy, "username", "");
        const std::string proxypassword = util::getObjStringOrDefault(proxy, "password", "");
        const unsigned short proxyport = (unsigned short)util::getObjInt32OrDefault(proxy, "port", 0);
        const auto proxytype = util::getObjEnumValueOrDefault<ProxySettings::_ProxyType>(proxy, "type", ProxySettings::PROXY_HTTPS);
        
        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, xpressurl, timeout,
            proxyurl, proxyusername, proxypassword, proxytype, proxyport](){
                ProxySettings prxsettings{}, *prx = NULL;
                if (!proxyurl.empty())
                {
                  char _url[1000]{}, _username[1000]{}, _password[1000]{};
                  strncpy(_url, proxyurl.c_str(), sizeof(_url)-1);
                  strncpy(_username, proxyusername.c_str(), sizeof(_url)-1);
                  strncpy(_password, proxypassword.c_str(), sizeof(_url)-1);
                  prxsettings.URL = _url;
                  prxsettings.Username = _username;
                  prxsettings.Password = _password;
                  prxsettings.Type = proxytype;
                  prxsettings.Port = proxyport;
                  prx = &prxsettings;
                }
                Jabra_ReturnCode retCode = Jabra_ConfigureXpressManagement(deviceId, xpressurl.c_str(), prx, timeout);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_PreloadAttachedDeviceInfo(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string zipFilename = info[1].As<Napi::String>();  
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();  

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, zipFilename](){ 
        Jabra_ReturnCode retv = Jabra_PreloadAttachedDeviceInfo(deviceId, zipFilename.c_str());     

        if (retv != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        } 
      }
    ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetLocalManifestVersion(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info,
        [functionName](unsigned short deviceId) {
            char* strManifestVersion = Jabra_GetLocalManifestVersion(deviceId);
            if (strManifestVersion == nullptr) {
                util::JabraReturnCodeException::LogAndThrow(functionName, Jabra_ReturnCode::File_Not_Accessible);
            }
            std::string result(strManifestVersion);
            Jabra_FreeString(strManifestVersion);
            return util::toUtf8(result, functionName);
        }, [](const Napi::Env& env, const std::string& cppResult) {
            return Napi::String::New(env, cppResult);
        });
}

Napi::Value napi_GetXpressUrl(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info,
        [functionName](unsigned short deviceId) {
            int size = 2000;
            std::vector<char> buffer(size);

            Jabra_ReturnCode retCode = Jabra_GetXpressUrl(deviceId, buffer.data(), size);
            if (retCode != Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }

            std::string result(buffer.data());
            return util::toUtf8(result, functionName);
        }, [](const Napi::Env& env, const std::string& cppResult) {
            return Napi::String::New(env, cppResult);
        });
}

Napi::Value napi_GetDiagnosticLogFile(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const std::string filename = info[1].As<Napi::String>();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, filename](){
                Jabra_ReturnCode retCode = Jabra_GetDiagnosticLogFile(deviceId, filename.c_str());
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_TriggerDiagnosticLogGeneration(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                Jabra_ReturnCode retCode = Jabra_TriggerDiagnosticLogGeneration(deviceId);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_RestoreVideoRoomDefaults(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                Jabra_ReturnCode retCode = Jabra_RestoreVideoRoomDefaults(deviceId);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_ResetPanTiltZoom(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                Jabra_ReturnCode retCode = Jabra_ResetPanTiltZoom(deviceId);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_ResetImageQualityControls(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId](){
                Jabra_ReturnCode retCode = Jabra_ResetImageQualityControls(deviceId);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
    }

  return env.Undefined();
}

Napi::Value napi_SetPasswordProvisioning(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const std::string password = info[1].As<Napi::String>();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, password](){
                Jabra_ReturnCode retCode = Jabra_SetPasswordProvisioning(deviceId, password.c_str());
                if (retCode != Return_Ok) {
                  util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetPasswordProvisioning(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    return util::SimpleDeviceAsyncFunction<Napi::String, std::string>(functionName, info,
        [functionName](unsigned short deviceId) {
            // 2 * 1024 == 2 Kb is the maximum allowed by Jabra_GetPasswordProvisioning
            int size = 2 * 1024;
            std::vector<char> buffer(size);

            Jabra_ReturnCode retCode = Jabra_GetPasswordProvisioning(deviceId, buffer.data(), size);
            if (retCode != Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                return std::string(); // Dummy return - avoid compiler warnings.
            }

            std::string result(buffer.data());
            return util::toUtf8(result, functionName);
        }, [](const Napi::Env& env, const std::string& cppResult) {
            return Napi::String::New(env, cppResult);
        });
}

Napi::Value napi_GetWhiteboardPosition(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        return env.Undefined();
    }

    /*
     * If you wonder why we don't use SimpleDeviceAsyncFunction it's because
     * there's an extra parameter (the whiteboard id)
     */
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const uint8_t whiteboardId = (uint8_t)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<Jabra_WhiteboardPosition, Napi::Object>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, whiteboardId]() {
            Jabra_WhiteboardPosition whiteboardPosition;
            Jabra_ReturnCode retCode = Jabra_GetWhiteboardPosition(deviceId,
                whiteboardId, &whiteboardPosition);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return whiteboardPosition;
        },
        [](const Napi::Env& env, const Jabra_WhiteboardPosition& whiteboard) {
            Napi::Object jsWhiteboard = Napi::Object::New(env);

            jsWhiteboard.Set("lowerLeftCorner", makePoint2D(env,
                whiteboard.lowerLeftCornerX, whiteboard.lowerLeftCornerY));
            jsWhiteboard.Set("lowerRightCorner", makePoint2D(env,
                whiteboard.lowerRightCornerX, whiteboard.lowerRightCornerY));
            jsWhiteboard.Set("upperRightCorner", makePoint2D(env,
                whiteboard.upperRightCornerX, whiteboard.upperRightCornerY));
            jsWhiteboard.Set("upperLeftCorner", makePoint2D(env,
                whiteboard.upperLeftCornerX, whiteboard.upperLeftCornerY));

            return jsWhiteboard;
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_SetWhiteboardPosition(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::OBJECT, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const uint8_t whiteboardId = (uint8_t)(info[1].As<Napi::Number>().Int32Value());
    Napi::Object whiteboardCorners = info[2].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();

    Napi::Object lowerLeftCorner = whiteboardCorners.Get("lowerLeftCorner").As<Napi::Object>();
    Napi::Object lowerRightCorner = whiteboardCorners.Get("lowerRightCorner").As<Napi::Object>();
    Napi::Object upperRightCorner = whiteboardCorners.Get("upperRightCorner").As<Napi::Object>();
    Napi::Object upperLeftCorner = whiteboardCorners.Get("upperLeftCorner").As<Napi::Object>();

    Jabra_WhiteboardPosition whiteboardPosition;
    whiteboardPosition.lowerLeftCornerX = (uint16_t) util::getObjInt32OrDefault(lowerLeftCorner, "x", 0);
    whiteboardPosition.lowerLeftCornerY = (uint16_t) util::getObjInt32OrDefault(lowerLeftCorner, "y", 0);
    whiteboardPosition.lowerRightCornerX = (uint16_t) util::getObjInt32OrDefault(lowerRightCorner, "x", 0);
    whiteboardPosition.lowerRightCornerY = (uint16_t) util::getObjInt32OrDefault(lowerRightCorner, "y", 0);
    whiteboardPosition.upperRightCornerX = (uint16_t) util::getObjInt32OrDefault(upperRightCorner, "x", 0);
    whiteboardPosition.upperRightCornerY = (uint16_t) util::getObjInt32OrDefault(upperRightCorner, "y", 0);
    whiteboardPosition.upperLeftCornerX = (uint16_t) util::getObjInt32OrDefault(upperLeftCorner, "x", 0);
    whiteboardPosition.upperLeftCornerY = (uint16_t) util::getObjInt32OrDefault(upperLeftCorner, "y", 0);

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, whiteboardId, whiteboardPosition]() {
            Jabra_ReturnCode retCode = Jabra_SetWhiteboardPosition(deviceId,
                whiteboardId, &whiteboardPosition);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_GetZoom(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, uint16_t>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            uint16_t zoom;
            Jabra_ReturnCode retCode = Jabra_GetZoom(deviceId, &zoom);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return zoom; // Dummy return - avoid compiler warnings.
            }

            return zoom;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetZoom(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    uint16_t zoom = (uint16_t) info[1].As<Napi::Number>().Int32Value();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, zoom]() {
            Jabra_ReturnCode retCode = Jabra_SetZoom(deviceId, zoom);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_GetZoomLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, Jabra_ZoomLimits>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            Jabra_ZoomLimits limits;
            Jabra_ReturnCode retCode = Jabra_GetZoomLimits(deviceId, &limits);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return limits;
        },
        [](const Napi::Env& env, const Jabra_ZoomLimits& cLimits) {
            auto jsLimits = Napi::Object::New(env);

            jsLimits.Set("min", Napi::Number::New(env, cLimits.min));
            jsLimits.Set("max", Napi::Number::New(env, cLimits.max));
            jsLimits.Set("stepSize", Napi::Number::New(env, cLimits.stepSize));

            return jsLimits;
        }
    );
}

Napi::Value napi_SetZoomRelativeAction(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::OBJECT, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Object jsAction = info[1].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    Jabra_ZoomRelative zoom;
    zoom.ZoomDirection = static_cast<enumZoomDirection>(util::getObjInt32OrDefault(jsAction, "direction", 0));
    zoom.ZoomSpeed = util::getObjInt32OrDefault(jsAction, "speed", 0);

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, zoom]() {
            Jabra_ReturnCode retCode = Jabra_SetZoomRelativeAction(deviceId, zoom);
            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_GetContrastLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::tuple<int, int, int>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int min, max, stepsize;
            Jabra_ReturnCode retCode = Jabra_GetContrastLimits(deviceId, &min, &max, &stepsize);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return std::make_tuple(min, max, stepsize);
        },
        [](const Napi::Env& env, const std::tuple<int, int, int>& limits) {
            auto jsLimits = Napi::Object::New(env);

            jsLimits.Set("min", Napi::Number::New(env, std::get<0>(limits)));
            jsLimits.Set("max", Napi::Number::New(env, std::get<1>(limits)));
            jsLimits.Set("stepSize", Napi::Number::New(env, std::get<2>(limits)));

            return jsLimits;
        }
    );
}

Napi::Value napi_GetSharpnessLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::tuple<int, int>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int min, max;
            Jabra_ReturnCode retCode = Jabra_GetSharpnessLimits(deviceId, &min, &max);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return std::make_tuple(min, max);
        },
        [](const Napi::Env& env, const std::tuple<int, int>& limits) {
            auto jsLimits = Napi::Object::New(env);

            jsLimits.Set("min", Napi::Number::New(env, std::get<0>(limits)));
            jsLimits.Set("max", Napi::Number::New(env, std::get<1>(limits)));

            return jsLimits;
        }
    );
}

Napi::Value napi_GetBrightnessLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::tuple<int, int, int>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int min, max, stepsize;
            Jabra_ReturnCode retCode = Jabra_GetBrightnessLimits(deviceId, &min, &max, &stepsize);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return std::make_tuple(min, max, stepsize);
        },
        [](const Napi::Env& env, const std::tuple<int, int, int>& limits) {
            auto jsLimits = Napi::Object::New(env);

            jsLimits.Set("min", Napi::Number::New(env, std::get<0>(limits)));
            jsLimits.Set("max", Napi::Number::New(env, std::get<1>(limits)));
            jsLimits.Set("stepSize", Napi::Number::New(env, std::get<2>(limits)));

            return jsLimits;
        }
    );
}

Napi::Value napi_GetSaturationLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::tuple<int, int>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int min, max;
            Jabra_ReturnCode retCode = Jabra_GetSaturationLimits(deviceId, &min, &max);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return std::make_tuple(min, max);
        },
        [](const Napi::Env& env, const std::tuple<int, int>& limits) {
            auto jsLimits = Napi::Object::New(env);

            jsLimits.Set("min", Napi::Number::New(env, std::get<0>(limits)));
            jsLimits.Set("max", Napi::Number::New(env, std::get<1>(limits)));

            return jsLimits;
        }
    );
}

Napi::Value napi_GetWhiteBalanceLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::tuple<int, int>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int min, max;
            Jabra_ReturnCode retCode = Jabra_GetWhiteBalanceLimits(deviceId, &min, &max);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return std::make_tuple(min, max);
        },
        [](const Napi::Env& env, const std::tuple<int, int>& limits) {
            auto jsLimits = Napi::Object::New(env);

            jsLimits.Set("min", Napi::Number::New(env, std::get<0>(limits)));
            jsLimits.Set("max", Napi::Number::New(env, std::get<1>(limits)));

            return jsLimits;
        }
    );
}

Napi::Value napi_GetPanTilt(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::pair<int32_t, int32_t>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            std::pair<int32_t, int32_t> panTilt;

            Jabra_ReturnCode retCode = Jabra_GetPanTilt(deviceId,
                &panTilt.first, &panTilt.second);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return panTilt; // Dummy return - avoid compiler warnings.
            }

            return panTilt;
        },
        [](const Napi::Env& env, const std::pair<int32_t, int32_t>& cPanTilt) {
            Napi::Object jsPanTilt = Napi::Object::New(env);
            jsPanTilt.Set("pan", Napi::Number::New(env, cPanTilt.first));
            jsPanTilt.Set("tilt", Napi::Number::New(env, cPanTilt.second));
            return jsPanTilt;
        }
    );
}

Napi::Value napi_SetPanTilt(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::OBJECT, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Object jsPanTilt = info[1].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    int32_t pan = util::getObjInt32OrDefault(jsPanTilt, "pan", 0);
    int32_t tilt = util::getObjInt32OrDefault(jsPanTilt, "tilt", 0);

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, pan, tilt]() {
            Jabra_ReturnCode retCode = Jabra_SetPanTilt(deviceId,
                pan, tilt);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_GetPanTiltLimits(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    return util::SimpleDeviceAsyncFunction<Napi::Object, std::pair<Jabra_PanTiltLimits, Jabra_PanTiltLimits>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            std::pair<Jabra_PanTiltLimits, Jabra_PanTiltLimits> panTiltLimits; // pan,tilt

            Jabra_ReturnCode retCode = Jabra_GetPanTiltLimits(deviceId,
                &panTiltLimits.first, &panTiltLimits.second);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }
            return panTiltLimits;
        },
        [](const Napi::Env& env, const std::pair<Jabra_PanTiltLimits, Jabra_PanTiltLimits>& cPanTiltLimits) {
            Napi::Object jsPanLimits = Napi::Object::New(env);
            jsPanLimits.Set("min", Napi::Number::New(env, cPanTiltLimits.first.min));
            jsPanLimits.Set("max", Napi::Number::New(env, cPanTiltLimits.first.max));
            jsPanLimits.Set("stepSize", Napi::Number::New(env, cPanTiltLimits.first.stepSize));
            Napi::Object jsTiltLimits = Napi::Object::New(env);
            jsTiltLimits.Set("min", Napi::Number::New(env, cPanTiltLimits.second.min));
            jsTiltLimits.Set("max", Napi::Number::New(env, cPanTiltLimits.second.max));
            jsTiltLimits.Set("stepSize", Napi::Number::New(env, cPanTiltLimits.second.stepSize));
            Napi::Object jsPanTiltLimits = Napi::Object::New(env);
            jsPanTiltLimits.Set("pan", jsPanLimits);
            jsPanTiltLimits.Set("tilt", jsTiltLimits);
            return jsPanTiltLimits;
        }
    );
}

Napi::Value napi_SetPanTiltRelativeAction(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::OBJECT, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Object jsAction = info[1].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    Jabra_PanTiltRelative pantilt;
    pantilt.PanDirection = static_cast<enumPanDirection>(util::getObjInt32OrDefault(jsAction, "panDirection", 0));
    pantilt.PanSpeed = util::getObjInt32OrDefault(jsAction, "panSpeed", 0);
    pantilt.TiltDirection = static_cast<enumTiltDirection>(util::getObjInt32OrDefault(jsAction, "tiltDirection", 0));
    pantilt.TiltSpeed = util::getObjInt32OrDefault(jsAction, "tiltSpeed", 0);

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, pantilt]() {
            Jabra_ReturnCode retCode = Jabra_SetPanTiltRelativeAction(deviceId, pantilt);
            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_SetVideoHDR(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetVideoHDR(deviceId, enable);
        if (result != Return_Ok) {
            throw util::JabraReturnCodeException(functionName, result);
        }
    });
}

Napi::Value napi_GetVideoHDR(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            bool enable = false;
            Jabra_ReturnCode retCode = Jabra_GetVideoHDR(deviceId, &enable);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }
            return enable;
        },
        Napi::Boolean::New
    );
}

Napi::Value napi_SetVideoHDRDefault(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetVideoHDRDefault(deviceId, enable);
        if (result != Return_Ok) {
            throw util::JabraReturnCodeException(functionName, result);
        }
    });
}

Napi::Value napi_GetVideoHDRDefault(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            bool enable = false;
            Jabra_ReturnCode retCode = Jabra_GetVideoHDRDefault(deviceId, &enable);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }
            return enable;
        },
        Napi::Boolean::New
    );
}

Napi::Value napi_SetContrastLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const int level = info[1].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, level](){
                Jabra_ReturnCode retCode = Jabra_SetContrastLevel(deviceId, level);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetContrastLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, int32_t>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int32_t level;
            Jabra_ReturnCode retCode = Jabra_GetContrastLevel(deviceId, &level);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return level; // Dummy return - avoid compiler warnings.
            }

            return level;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetSharpnessLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const int level = info[1].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, level](){
                Jabra_ReturnCode retCode = Jabra_SetSharpnessLevel(deviceId, level);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetSharpnessLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, int32_t>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int32_t level;
            Jabra_ReturnCode retCode = Jabra_GetSharpnessLevel(deviceId, &level);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return level; // Dummy return - avoid compiler warnings.
            }

            return level;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetBrightnessLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const int level = info[1].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, level](){
                Jabra_ReturnCode retCode = Jabra_SetBrightnessLevel(deviceId, level);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetBrightnessLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, int32_t>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int32_t level;
            Jabra_ReturnCode retCode = Jabra_GetBrightnessLevel(deviceId, &level);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return level; // Dummy return - avoid compiler warnings.
            }

            return level;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetSaturationLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const int level = info[1].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, level](){
                Jabra_ReturnCode retCode = Jabra_SetSaturationLevel(deviceId, level);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetSaturationLevel(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, int32_t>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int32_t level;
            Jabra_ReturnCode retCode = Jabra_GetSaturationLevel(deviceId, &level);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return level; // Dummy return - avoid compiler warnings.
            }

            return level;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetWhiteBalance(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::OBJECT, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        Napi::Object jsWhiteBalance = info[1].As<Napi::Object>();
        const int value = util::getObjInt32OrDefault(jsWhiteBalance, "value", 0);
        const Jabra_AutoWhiteBalance autoWB = static_cast<Jabra_AutoWhiteBalance>(util::getObjInt32OrDefault(jsWhiteBalance, "autoWB", 0));
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, value, autoWB](){
                Jabra_ReturnCode retCode = Jabra_SetWhiteBalance(deviceId, value, autoWB);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetWhiteBalance(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, std::pair<int, Jabra_AutoWhiteBalance>>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int value = 0;
            Jabra_AutoWhiteBalance autoWB;
            Jabra_ReturnCode retCode = Jabra_GetWhiteBalance(deviceId, &value, &autoWB);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return std::make_pair(value, autoWB);
        },
        [](const Napi::Env& env, const std::pair<int, Jabra_AutoWhiteBalance>& whiteBalance) {
            Napi::Object jsWhiteBalance = Napi::Object::New(env);
            jsWhiteBalance.Set("value", Napi::Number::New(env, whiteBalance.first));
            jsWhiteBalance.Set("autoWB", Napi::Number::New(env, whiteBalance.second));
            return jsWhiteBalance;
        }
    );
}

Napi::Value napi_SetRoomCapacity(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
        const int16_t capacity = info[1].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        (new util::JAsyncWorker<void, void>(
            functionName,
            javascriptResultCallback,
            [functionName, deviceId, capacity](){
                Jabra_ReturnCode retCode = Jabra_SetRoomCapacity(deviceId, capacity);
                if (retCode != Return_Ok) {
                    util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
                }
            }
        ))->Queue();
  }
  return env.Undefined();
}

Napi::Value napi_GetRoomCapacity(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, int16_t>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            int16_t capacity = 0;
            Jabra_ReturnCode retCode = Jabra_GetRoomCapacity(deviceId, &capacity);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return capacity; // Dummy return - avoid compiler warnings.
            }

            return capacity;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetRoomCapacityNotificationEnabled(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncBoolSetter(functionName, info, [functionName](unsigned short deviceId, bool enable) {
        const Jabra_ReturnCode result = Jabra_SetRoomCapacityNotificationEnabled(deviceId, enable);
        if (result != Return_Ok) {
          throw util::JabraReturnCodeException(functionName, result);
        }
  });
}

Napi::Value napi_GetRoomCapacityNotificationEnabled(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            bool enable = false;
            Jabra_ReturnCode retCode = Jabra_GetRoomCapacityNotificationEnabled(deviceId, &enable);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return enable; // Dummy return - avoid compiler warnings.
            }

            return enable;
        },
        Napi::Boolean::New
    );
}

Napi::Value napi_SetNotificationStyle(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_NotificationStyle style = (Jabra_NotificationStyle)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, style]() {
            Jabra_ReturnCode retCode = Jabra_SetNotificationStyle(deviceId, style);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_GetNotificationStyle(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, Jabra_NotificationStyle>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            Jabra_NotificationStyle style;
            Jabra_ReturnCode retCode = Jabra_GetNotificationStyle(deviceId, &style);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return style; // Dummy return - avoid compiler warnings.
            }

            return style;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetNotificationUsage(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;
    Napi::Env env = info.Env();

    if (!util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
        return env.Undefined();
    }

    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_NotificationUsage enable = (Jabra_NotificationUsage)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
        functionName,
        javascriptResultCallback,
        [functionName, deviceId, enable]() {
            Jabra_ReturnCode retCode = Jabra_SetNotificationUsage(deviceId, enable);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }
        }
    ))->Queue();

    return env.Undefined();
}

Napi::Value napi_GetNotificationUsage(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, Jabra_NotificationUsage>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            Jabra_NotificationUsage enable;
            Jabra_ReturnCode retCode = Jabra_GetNotificationUsage(deviceId, &enable);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
                return enable; // Dummy return - avoid compiler warnings.
            }

            return enable;
        },
        Napi::Number::New
    );
}

Napi::Value napi_StoreColorControlPreset(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_ColorControlPreset preset = (Jabra_ColorControlPreset)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, preset](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_StoreColorControlPreset(deviceId, preset)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_ApplyColorControlPreset(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_ColorControlPreset preset = (Jabra_ColorControlPreset)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, preset](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_ApplyColorControlPreset(deviceId, preset)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_StorePTZPreset(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_PTZPreset preset = (Jabra_PTZPreset)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, preset](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_StorePTZPreset(deviceId, preset)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_ApplyPTZPreset(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_PTZPreset preset = (Jabra_PTZPreset)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, preset](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_ApplyPTZPreset(deviceId, preset)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_SetSecondVideoStream(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const SecondaryStreamContent content = (SecondaryStreamContent)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, content](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetSecondVideoStream(deviceId, content)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetSecondVideoStream(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, SecondaryStreamContent>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            SecondaryStreamContent content;
            Jabra_ReturnCode retCode = Jabra_GetSecondVideoStream(deviceId, &content);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return content;
        },
        Napi::Number::New
    );
}

Napi::Value napi_SetVideoTransitionStyle(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Jabra_VideoTransitionStyle style = (Jabra_VideoTransitionStyle)(info[1].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, style](){ 
        Jabra_ReturnCode retv;                       
        if ((retv = Jabra_SetVideoTransitionStyle(deviceId, style)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetVideoTransitionStyle(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, Jabra_VideoTransitionStyle>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            Jabra_VideoTransitionStyle style;
            Jabra_ReturnCode retCode = Jabra_GetVideoTransitionStyle(deviceId, &style);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return style;
        },
        Napi::Number::New
    );
}

Napi::Value napi_GetEthernetIPv4Status(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, IPv4Status>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            IPv4Status status;
            Jabra_ReturnCode retCode = Jabra_GetEthernetIPv4Status(deviceId, &status);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return status;
        },
        [](const Napi::Env& env, const IPv4Status& status) {
            auto jsStatus = Napi::Object::New(env);
            auto jsIP = Napi::Object::New(env);
            auto jsSubnetMask = Napi::Object::New(env);

            jsIP.Set("octet1", Napi::Number::New(env, status.IP.octet1));
            jsIP.Set("octet2", Napi::Number::New(env, status.IP.octet2));
            jsIP.Set("octet3", Napi::Number::New(env, status.IP.octet3));
            jsIP.Set("octet4", Napi::Number::New(env, status.IP.octet4));
            jsSubnetMask.Set("octet1", Napi::Number::New(env, status.SubNetMask.octet1));
            jsSubnetMask.Set("octet2", Napi::Number::New(env, status.SubNetMask.octet2));
            jsSubnetMask.Set("octet3", Napi::Number::New(env, status.SubNetMask.octet3));
            jsSubnetMask.Set("octet4", Napi::Number::New(env, status.SubNetMask.octet4));
            jsStatus.Set("interfaceEnabled", Napi::Boolean::New(env, status.InterfaceEnabled));
            jsStatus.Set("DHCPEnabled", Napi::Boolean::New(env, status.DHCPEnabled));
            jsStatus.Set("connectionStatus", Napi::Boolean::New(env, status.ConnectionStatus));
            jsStatus.Set("IP", jsIP);
            jsStatus.Set("subnetMask", jsSubnetMask);
            return jsStatus;
        }
    );
}

Napi::Value napi_GetWLANIPv4Status(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Object, IPv4Status>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            IPv4Status status;
            Jabra_ReturnCode retCode = Jabra_GetWLANIPv4Status(deviceId, &status);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName,
                    retCode);
            }

            return status;
        },
        [](const Napi::Env& env, const IPv4Status& status) {
            auto jsStatus = Napi::Object::New(env);
            auto jsIP = Napi::Object::New(env);
            auto jsSubnetMask = Napi::Object::New(env);

            jsIP.Set("octet1", Napi::Number::New(env, status.IP.octet1));
            jsIP.Set("octet2", Napi::Number::New(env, status.IP.octet2));
            jsIP.Set("octet3", Napi::Number::New(env, status.IP.octet3));
            jsIP.Set("octet4", Napi::Number::New(env, status.IP.octet4));
            jsSubnetMask.Set("octet1", Napi::Number::New(env, status.SubNetMask.octet1));
            jsSubnetMask.Set("octet2", Napi::Number::New(env, status.SubNetMask.octet2));
            jsSubnetMask.Set("octet3", Napi::Number::New(env, status.SubNetMask.octet3));
            jsSubnetMask.Set("octet4", Napi::Number::New(env, status.SubNetMask.octet4));
            jsStatus.Set("interfaceEnabled", Napi::Boolean::New(env, status.InterfaceEnabled));
            jsStatus.Set("DHCPEnabled", Napi::Boolean::New(env, status.DHCPEnabled));
            jsStatus.Set("connectionStatus", Napi::Boolean::New(env, status.ConnectionStatus));
            jsStatus.Set("IP", jsIP);
            jsStatus.Set("subnetMask", jsSubnetMask);
            return jsStatus;
        }
    );
}

Napi::Value napi_GetUSBState(const Napi::CallbackInfo& info) {
    const char * const functionName = __func__;

    return util::SimpleDeviceAsyncFunction<Napi::Number, USB_CONNECTION_SPEED>(
        functionName, info,
        [functionName](unsigned short deviceId) {
            USB_CONNECTION_SPEED speed{};
            Jabra_ReturnCode retCode = Jabra_GetUSBState(deviceId, &speed);

            if (retCode != Jabra_ReturnCode::Return_Ok) {
                util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
            }
            return speed;
        },
        Napi::Number::New
    );
}
