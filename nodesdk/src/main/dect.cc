#include "dect.h"

Napi::Value napi_GetConnectedHeadsetNames(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::BOOLEAN, util::OBJECT, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const bool getAssetNames = info[1].As<Napi::Boolean>();
    Napi::Object whichNamesToRead = info[2].As<Napi::Object>();
    unsigned int readMask = (unsigned int)util::getObjBooleanOrDefault(whichNamesToRead, "primary", false);
    readMask |= ((unsigned int)util::getObjBooleanOrDefault(whichNamesToRead, "secondary1", false)) << 1;
    readMask |= ((unsigned int)util::getObjBooleanOrDefault(whichNamesToRead, "secondary2", false)) << 2;
    readMask |= ((unsigned int)util::getObjBooleanOrDefault(whichNamesToRead, "secondary3", false)) << 3;
    const Napi::Function javascriptResultCallback = info[3].As<Napi::Function>();

    (new util::JAsyncWorker<std::vector<std::string>, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, getAssetNames, readMask]() {
        std::vector<std::string> headsetNames{};
        char *_priName = nullptr, *_sec1Name = nullptr, *_sec2Name = nullptr, *_sec3Name = nullptr;

        Jabra_ReturnCode retCode = Jabra_GetConnectedHeadsetNames(deviceId, readMask, getAssetNames, &_priName, &_sec1Name, &_sec2Name, &_sec3Name);
        headsetNames.push_back(_priName ? std::string(_priName) : "");
        headsetNames.push_back(_sec1Name ? std::string(_sec1Name) : "");
        headsetNames.push_back(_sec2Name ? std::string(_sec2Name) : "");
        headsetNames.push_back(_sec3Name ? std::string(_sec3Name) : "");
        Jabra_FreeString(_priName);
        Jabra_FreeString(_sec1Name);
        Jabra_FreeString(_sec2Name);
        Jabra_FreeString(_sec3Name);

        if (retCode != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
        }
        return headsetNames;
      },
      [](const Napi::Env& env, std::vector<std::string> headsetNames) {
        Napi::Object jNames = Napi::Object::New(env);

        jNames.Set("primary", Napi::String::New(env, headsetNames[0]));
        jNames.Set("secondary1", Napi::String::New(env, headsetNames[1]));
        jNames.Set("secondary2", Napi::String::New(env, headsetNames[2]));
        jNames.Set("secondary3", Napi::String::New(env, headsetNames[3]));
        return jNames;
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_TriggerDECTPairing(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const unsigned short devicePairingType = (unsigned short)(info[1].As<Napi::Number>().Int32Value());
    const Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, devicePairingType]() {
        const DectHeadset pairingType = (DectHeadset)devicePairingType == DectHeadset::PRIMARY_HS ? DectHeadset::PRIMARY_HS : DectHeadset::SECONDARY_HS;
        Jabra_ReturnCode retCode = Jabra_DectPair(deviceId, pairingType);

        if (retCode != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_TriggerDECTSecurePairing(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId]() {
        Jabra_ReturnCode retCode = Jabra_DectPairSecure(deviceId);

        if (retCode != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetDECTPairingKey(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Number, uint32_t>(
    functionName, info,
      [functionName](unsigned short deviceId) {
          uint32_t pairingKey;
          Jabra_ReturnCode retCode = Jabra_GetDectPairKey(deviceId, &pairingKey);
          if (retCode != Return_Ok) {
              util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
          }
          return pairingKey;
      }, [](const Napi::Env& env, const uint32_t& cppResult) {
          auto pairingKey = Napi::Number::New(env, cppResult);
          return pairingKey;
      });
}

Napi::Value napi_SetDECTPairingKey(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const uint32_t pairingKey = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<void, void>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, pairingKey]() {
        Jabra_ReturnCode retCode = Jabra_SetDectPairKey(deviceId, pairingKey);

        if (retCode != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retCode);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}
