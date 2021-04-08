#include "settings.h"

#include <string.h>
#include <limits.h>
#include <stdint.h>

/**
 * For freeing a DeviceSettings that we created ourselves.
 * In this case we can't safely use Jabra_FreeDeviceSettings
 * as we don't have binary compatability for c++ delete.
 * 
 * The current code is copied from Jabra_FreeDeviceSettings source.
 */
static void Custom_FreeDeviceSettings(DeviceSettings* setting) {
	if (setting != nullptr) {
		if (setting->settingInfo != nullptr) {

			SettingInfo *lstStngInfo = setting->settingInfo;
			for (unsigned int i = 0; i < setting->settingCount; i++) {
				delete[] lstStngInfo->guid;
				delete[] lstStngInfo->name;
				delete[] lstStngInfo->helpText;
				delete[](char *)lstStngInfo->currValue;
				//List
				if (lstStngInfo->listKeyValue != nullptr) {
					ListKeyValue *lstKeyVal = lstStngInfo->listKeyValue;
					for (int i = 0; i < lstStngInfo->listSize; i++) {
						delete[](char *)lstKeyVal->value;
						//dependents
						if (lstStngInfo->isDepedentsetting) {
							for (int depIdx = 0; depIdx < lstKeyVal->dependentcount; depIdx++) {
								delete[](lstKeyVal->dependents + depIdx)->GUID;
							}
							delete[] lstKeyVal->dependents;
						}
						lstKeyVal++;
					}
				}
				delete[] lstStngInfo->listKeyValue;

				//validation rule
				if (lstStngInfo->validationRule != nullptr) {
					delete[] lstStngInfo->validationRule->regExp;
					delete[] lstStngInfo->validationRule->errorMessage;
				}
				delete lstStngInfo->validationRule;
				delete[] lstStngInfo->groupName;
				delete[] lstStngInfo->groupHelpText;
				delete[](char *)lstStngInfo->dependentDefaultValue;
				lstStngInfo++;
			}
		}
		delete[] setting->settingInfo;
	}
	delete setting;
}

/**
 * Utility for representing a native deviceSettings structure as a string (for logging purposes).
 */
static std::string toString(DeviceSettings * src) {
  std::ostringstream buffer; 

  buffer << "{ ";
  buffer << "\"errStatus\": " << src->errStatus << ", ";
  buffer << "\"settingInfo\": [ ";

  for (unsigned int i=0; i<src->settingCount; ++i) {
    SettingInfo& settingSrc = src->settingInfo[i];
    if (i>0)  {
        buffer << ",";
    }

    buffer << "{ ";
    buffer << "\"guid\": \"" << (settingSrc.guid ? settingSrc.guid : "") << "\", ";
    buffer << "\"name\": \"" << (settingSrc.name ? settingSrc.name : "") << "\", ";
    buffer << "\"helpText\": \"" << (settingSrc.helpText ? settingSrc.helpText : "") << "\", ";
    buffer << "\"isValidationSupport\": " << (settingSrc.isValidationSupport ? "true" : "false") << ", ";

    ValidationRule *validationRuleSrc = settingSrc.validationRule;
    if (validationRuleSrc) {
       buffer << "\"validationRule\": { ";
       buffer << "\"minLength\": " << validationRuleSrc->minLength << ", ";
       buffer << "\"maxLength\": " << validationRuleSrc->maxLength << ", ";
       buffer << "\"errorMessage\": \"" << (validationRuleSrc->errorMessage ? validationRuleSrc->errorMessage : "") << "\", ";
       buffer << "\"regExp\": \"" << (validationRuleSrc->regExp ? validationRuleSrc->regExp : "") << "\" " ;
       buffer << "}, " << std::endl;
    }


    buffer << "\"isDeviceRestart\": " << (settingSrc.isDeviceRestart ? "true" : "false") << ", ";
    buffer << "\"isSettingProtected\": " << (settingSrc.isSettingProtected ? "true" : "false") << ", ";    
    buffer << "\"isSettingProtectionEnabled\": " << (settingSrc.isSettingProtectionEnabled ? "true" : "false") << ", ";
    buffer << "\"isWirelessConnect\": " << (settingSrc.isWirelessConnect ? "true" : "false") << ", ";
    buffer << "\"cntrlType\": " << settingSrc.cntrlType << ", ";
    
    if (settingSrc.currValue) {
      if (settingSrc.settingDataType == DataType::settingByte) {
        buffer << "\"currValue\": " << std::to_string(*((uint8_t *)settingSrc.currValue)) << ", ";
      } else if (settingSrc.settingDataType == DataType::settingString) {
        buffer << "\"currValue\": \"" << (char *)settingSrc.currValue << "\", ";
      } else {
        buffer << "\"currValue\": " << "ERROR: unexpected datatype " << settingSrc.currValue << ", ";
      }
    }

    buffer << "\"settingDataType\": " << settingSrc.settingDataType << ", ";
    buffer << "\"groupName\": \"" << (settingSrc.groupName ? settingSrc.groupName : "") << "\", ";
    buffer << "\"groupHelpText\": \"" << (settingSrc.groupHelpText ? settingSrc.groupHelpText : "") << "\", ";
    buffer << "\"isDepedentsetting\": " << (settingSrc.isDepedentsetting ? "true" : "false") << ", ";

    buffer << "\"isPCsetting\": " << (settingSrc.isPCsetting ? "true" : "false") << ", ";
    buffer << "\"isChildDeviceSetting\": " << (settingSrc.isChildDeviceSetting ? "true" : "false") << ", ";
    
    if (settingSrc.dependentDefaultValue) {
      if (settingSrc.settingDataType == DataType::settingByte) {
        buffer << "\"dependentDefaultValue\": " << std::to_string(*((uint8_t *)settingSrc.dependentDefaultValue)) << ", ";
      } else if (settingSrc.settingDataType == DataType::settingString) {
        buffer << "\"dependentDefaultValue\": \"" << (char *)settingSrc.dependentDefaultValue << "\", ";
      } else {
        buffer << "\"dependentDefaultValue\": " << "ERROR: unexpected datatype " << settingSrc.settingDataType << ", ";
      }
    }

    buffer << "\"listSize\": " << settingSrc.listSize << ", ";
    buffer << "\"listKeyValue\": [ ";
    for (int j=0; j< settingSrc.listSize; ++j) {
      if (j>0)  {
        buffer << ",";
      }

      buffer << "{ ";

      ListKeyValue& listKeyValueSrc = settingSrc.listKeyValue[j];

      buffer << "\"key\": " << listKeyValueSrc.key << ", ";
 
      if (listKeyValueSrc.value) {
        buffer << "\"value\": \"" <<  (char *)listKeyValueSrc.value << "\", ";
      }

      buffer << "\"dependentcount\": " << listKeyValueSrc.dependentcount << ", ";
      buffer << "\"dependents\": [ ";
      for (int k=0; k< listKeyValueSrc.dependentcount; ++k) {
        if (k>0)  {
           buffer << ",";
        }

        DependencySetting& dependencySettingSrc = listKeyValueSrc.dependents[k];
        buffer << "{ ";
        buffer << "\"GUID\": \"" << (dependencySettingSrc.GUID ? dependencySettingSrc.GUID : "") << "\", ";
        buffer << "\"enableFlag\": " << (dependencySettingSrc.enableFlag ? "true" : "false") << " ";
        buffer << "}";
      }
      buffer << "]";

      buffer << "}";
    }
    buffer << "]";

    buffer << "}";
  }

  buffer << "]"; // settingInfo

  buffer << "}"; // top level object

  return buffer.str();
}

/**
 * Convert a napi device settings object to a native sdk DeviceSettings object (the reverse of toNodeType).
 * 
 * Nb. Use Custom_FreeDeviceSettings to free memory allocated by this function.
 */
static DeviceSettings *toCType(const unsigned short deviceId, Napi::Object src) {
  DeviceSettings * result = new DeviceSettings();

  result->errStatus = util::getObjEnumValueOrDefault(src, "errStatus", Jabra_ErrorStatus::NoError);

  Napi::Array settingInfo = src.Get("settingInfo").As<Napi::Array>();
  if (settingInfo.IsArray()) {
    result->settingCount = util::getObjInt32OrDefault(src, "settingsCount", settingInfo.Length());
    result->settingInfo = new SettingInfo[result->settingCount];

    for (unsigned int i=0; i<result->settingCount; ++i) {
      SettingInfo& settingDst = result->settingInfo[i];
      Napi::Object settingSrc = settingInfo.Get(i).ToObject();

      settingDst.guid = util::newCString(settingSrc.Get("guid"));
      settingDst.name = util::newCString(settingSrc.Get("name"));
      settingDst.helpText = util::newCString(settingSrc.Get("helpText"));
      settingDst.isValidationSupport = util::newCString(settingSrc.Get("isValidationSupport"));

      Napi::Value validationRuleSrc = settingSrc.Get("validationRule");
      if (validationRuleSrc.IsObject()) {
        Napi::Object validationRuleSrcObj = validationRuleSrc.As<Napi::Object>();
        settingDst.validationRule = new ValidationRule();
        settingDst.validationRule->errorMessage = util::newCString(validationRuleSrcObj.Get("errorMessage"));
        settingDst.validationRule->maxLength = util::getObjInt32OrDefault(validationRuleSrcObj, "maxLength", INT_MAX);
        settingDst.validationRule->minLength = util::getObjInt32OrDefault(validationRuleSrcObj, "minLength", 0);
        settingDst.validationRule->regExp = util::newCString(validationRuleSrcObj.Get("regExp"));
      } else {
        settingDst.validationRule = nullptr;
      }

      settingDst.isDeviceRestart = util::getObjBooleanOrDefault(settingSrc, "isDeviceRestart", false);
      settingDst.isSettingProtected = util::getObjBooleanOrDefault(settingSrc, "isSettingProtected", false);
      settingDst.isSettingProtectionEnabled = util::getObjBooleanOrDefault(settingSrc, "isSettingProtectionEnabled", false);
      settingDst.isWirelessConnect = util::getObjBooleanOrDefault(settingSrc, "isWirelessConnect", false);
      settingDst.cntrlType = util::getObjEnumValueOrDefault<ControlType>(settingSrc, "cntrlType", ControlType::cntrlUnknown);
      settingDst.settingDataType = util::getObjEnumValueOrDefault<DataType>(settingSrc, "settingDataType", DataType::settingByte);

      if (settingSrc.Has("currValue")) {
        if (settingDst.settingDataType == DataType::settingByte) {
          settingDst.currValue = new char[1] { (char)(uint8_t)util::getObjInt32OrDefault(settingSrc, "currValue", 0) };
        } else if (settingDst.settingDataType == DataType::settingString) {
          settingDst.currValue = util::newCString(settingSrc.Get("currValue"));
        } else {         
          LOG_ERROR_(LOGINSTANCE) << "Device " << deviceId << " has unexpected settingDataType " << settingDst.currValue << " for settings GUID " << settingDst.guid;
          settingDst.currValue = nullptr;
        }
      } else {
        settingDst.currValue = nullptr;
      }

      settingDst.groupName = util::newCString(settingSrc.Get("groupName"));
      settingDst.groupHelpText = util::newCString(settingSrc.Get("groupHelpText"));
      settingDst.isDepedentsetting = util::getObjBooleanOrDefault(settingSrc, "isDepedentsetting", false);

      settingDst.isPCsetting = util::getObjBooleanOrDefault(settingSrc, "isPCsetting", false);
      settingDst.isChildDeviceSetting = util::getObjBooleanOrDefault(settingSrc, "isChildDeviceSetting", false);

      if (settingSrc.Has("dependentDefaultValue")) {
        if (settingDst.settingDataType == DataType::settingByte) {
          settingDst.dependentDefaultValue = new char[1] { (char)(uint8_t)util::getObjInt32OrDefault(settingSrc, "dependentDefaultValue", 0) };
        } else if (settingDst.settingDataType == DataType::settingString) {
          settingDst.dependentDefaultValue = util::newCString(settingSrc.Get("dependentDefaultValue"));
        } else {         
          LOG_ERROR_(LOGINSTANCE) << "Device " << deviceId << " has unexpected settingDataType " << settingDst.currValue << " for settings GUID " << settingDst.guid;
          settingDst.dependentDefaultValue = nullptr;
        }
      } else {
        settingDst.dependentDefaultValue = nullptr;
      }

       Napi::Array listKeyValueAry = settingSrc.Get("listKeyValue").As<Napi::Array>();
       if (listKeyValueAry.IsArray()) {
          settingDst.listSize = util::getObjInt32OrDefault(settingSrc, "listSize", listKeyValueAry.Length());
          settingDst.listKeyValue = new ListKeyValue[settingDst.listSize];

          for (int j=0; j<settingDst.listSize; ++j) {
           ListKeyValue& listKeyValueDst = settingDst.listKeyValue[j];
           Napi::Object listKeyValueSrcObj = listKeyValueAry.Get(j).As<Napi::Object>();

           listKeyValueDst.key = util::getObjInt32OrDefault(listKeyValueSrcObj, "key", 0);
           listKeyValueDst.value = util::newCString(listKeyValueSrcObj.Get("value"));

           Napi::Array dependentsSrc = listKeyValueSrcObj.Get("dependents").As<Napi::Array>();
           if (dependentsSrc.IsArray()) {
              listKeyValueDst.dependentcount = util::getObjInt32OrDefault(listKeyValueSrcObj, "dependentcount", dependentsSrc.Length());
              listKeyValueDst.dependents = new DependencySetting[listKeyValueDst.dependentcount];

              for (int k=0; k<listKeyValueDst.dependentcount; ++k) {
                  DependencySetting& dependencySettingDst = listKeyValueDst.dependents[k];
                  Napi::Object dependencySettingSrc = dependentsSrc.Get(k).As<Napi::Object>();

                  dependencySettingDst.GUID = util::newCString(dependencySettingSrc.Get("GUID"));
                  dependencySettingDst.enableFlag = util::getObjBooleanOrDefault(dependencySettingSrc, "enableFlag", false);
              }
           } else {
            listKeyValueDst.dependentcount = 0;
            listKeyValueDst.dependents = nullptr;
           }
          }
       } else {
         settingDst.listSize = 0;
         settingDst.listKeyValue = nullptr;
       }
    }
  } else {
    result->settingCount = 0;
    result->settingInfo = nullptr;
  }

  return result;
}

/**
* Copy a native sdk DeviceSettings object into an empty napi device settings object (the reverse of toCType).
*/
static void toNodeType(const unsigned short deviceId, DeviceSettings *src, Napi::Object& dest) {
  Napi::Env env = dest.Env();
  
  Napi::Array settings = Napi::Array::New(env, src->settingCount);
  for (unsigned int i=0; i<src->settingCount; ++i) {
    SettingInfo& settingSrc = src->settingInfo[i];

    Napi::Object settingDst = Napi::Object::New(env);

    settingDst.Set(Napi::String::New(env, "guid"), Napi::String::New(env, settingSrc.guid ? settingSrc.guid : ""));
    settingDst.Set(Napi::String::New(env, "name"), Napi::String::New(env, settingSrc.name ? settingSrc.name : ""));
    settingDst.Set(Napi::String::New(env, "helpText"), Napi::String::New(env, settingSrc.helpText ? settingSrc.helpText : ""));
    settingDst.Set(Napi::String::New(env, "isValidationSupport"), Napi::Boolean::New(env, settingSrc.isValidationSupport));

    ValidationRule *validationRuleSrc = settingSrc.validationRule;
    if (validationRuleSrc) {
       Napi::Object validationRuleDst = Napi::Object::New(env);

       validationRuleDst.Set(Napi::String::New(env, "minLength"), Napi::Number::New(env, validationRuleSrc->minLength));
       validationRuleDst.Set(Napi::String::New(env, "maxLength"), Napi::Number::New(env, validationRuleSrc->maxLength));

       validationRuleDst.Set(Napi::String::New(env, "errorMessage"), Napi::String::New(env, validationRuleSrc->errorMessage ? validationRuleSrc->errorMessage : ""));
       validationRuleDst.Set(Napi::String::New(env, "regExp"), Napi::String::New(env, validationRuleSrc->regExp ? validationRuleSrc->regExp : ""));

       settingDst.Set(Napi::String::New(env, "validationRule"), validationRuleDst);
    }

    settingDst.Set(Napi::String::New(env, "isDeviceRestart"), Napi::Boolean::New(env, settingSrc.isDeviceRestart));
    settingDst.Set(Napi::String::New(env, "isSettingProtected"), Napi::Boolean::New(env, settingSrc.isSettingProtected));
    settingDst.Set(Napi::String::New(env, "isSettingProtectionEnabled"), Napi::Boolean::New(env, settingSrc.isSettingProtectionEnabled));
    settingDst.Set(Napi::String::New(env, "isWirelessConnect"), Napi::Boolean::New(env, settingSrc.isWirelessConnect));
    settingDst.Set(Napi::String::New(env, "cntrlType"), Napi::Number::New(env, settingSrc.cntrlType));
    settingDst.Set(Napi::String::New(env, "settingDataType"), Napi::Number::New(env, settingSrc.settingDataType));

    if (settingSrc.currValue) {
      if (settingSrc.settingDataType == DataType::settingByte) {
        settingDst.Set(Napi::String::New(env, "currValue"), Napi::Number::New(env, *((uint8_t *)settingSrc.currValue)));
      } else if (settingSrc.settingDataType == DataType::settingString) {
        settingDst.Set(Napi::String::New(env, "currValue"), Napi::String::New(env, (char *)settingSrc.currValue));
      } else {
        LOG_ERROR_(LOGINSTANCE) << "Device " << deviceId << " has unexpected settingDataType " << settingSrc.currValue << " for settings GUID " << settingSrc.guid;
      }
    }

    settingDst.Set(Napi::String::New(env, "groupName"), Napi::String::New(env, settingSrc.groupName ? settingSrc.groupName : ""));
    settingDst.Set(Napi::String::New(env, "groupHelpText"), Napi::String::New(env, settingSrc.groupHelpText ? settingSrc.groupHelpText : ""));
    settingDst.Set(Napi::String::New(env, "isDepedentsetting"), Napi::Boolean::New(env, settingSrc.isDepedentsetting));

    settingDst.Set(Napi::String::New(env, "isPCsetting"), Napi::Boolean::New(env, settingSrc.isPCsetting));
    settingDst.Set(Napi::String::New(env, "isChildDeviceSetting"), Napi::Boolean::New(env, settingSrc.isChildDeviceSetting));
    
    if (settingSrc.dependentDefaultValue) {
      if (settingSrc.settingDataType == DataType::settingByte) {
        settingDst.Set(Napi::String::New(env, "dependentDefaultValue"), Napi::Number::New(env, *((uint8_t *)settingSrc.dependentDefaultValue)));
      } else if (settingSrc.settingDataType == DataType::settingString) {
        settingDst.Set(Napi::String::New(env, "dependentDefaultValue"), Napi::String::New(env, (char *)settingSrc.dependentDefaultValue));
      } else {
        LOG_ERROR_(LOGINSTANCE) << "Device " << deviceId << " has unexpected settingDataType " << settingSrc.settingDataType << " for settings GUID " << settingSrc.guid;
      }
    }

    Napi::Array keyValueList = Napi::Array::New(env, settingSrc.listSize);
    settingDst.Set(Napi::String::New(env, "listSize"), Napi::Number::New(env, settingSrc.listSize));
    for (int j=0; j< settingSrc.listSize; ++j) {
      ListKeyValue& listKeyValueSrc = settingSrc.listKeyValue[j];

      Napi::Object listKeyValueDst = Napi::Object::New(env);
      listKeyValueDst.Set(Napi::String::New(env, "key"), Napi::Number::New(env, listKeyValueSrc.key));
 
      if (listKeyValueSrc.value) {
        listKeyValueDst.Set(Napi::String::New(env, "value"), Napi::String::New(env, (char *)listKeyValueSrc.value));
      }

      listKeyValueDst.Set(Napi::String::New(env, "dependentcount"), Napi::Number::New(env, listKeyValueSrc.dependentcount));

      Napi::Array dependenciesList = Napi::Array::New(env, listKeyValueSrc.dependentcount);
      for (int k=0; k< listKeyValueSrc.dependentcount; ++k) {
        DependencySetting& dependencySettingSrc = listKeyValueSrc.dependents[k];

        Napi::Object dependencySettingDst = Napi::Object::New(env);
        dependencySettingDst.Set(Napi::String::New(env, "GUID"), Napi::String::New(env, dependencySettingSrc.GUID ? dependencySettingSrc.GUID : ""));
        dependencySettingDst.Set(Napi::String::New(env, "enableFlag"), Napi::Boolean::New(env, dependencySettingSrc.enableFlag));

        dependenciesList.Set(k, dependencySettingDst);
      }

      listKeyValueDst.Set(Napi::String::New(env, "dependents"), dependenciesList);

      keyValueList.Set(j, listKeyValueDst);
    }

    settingDst.Set(Napi::String::New(env, "listKeyValue"), keyValueList);

    settings.Set(i, settingDst);
  }

  dest.Set(Napi::String::New(env, "errStatus"), Napi::Number::New(env, src->errStatus));
  dest.Set(Napi::String::New(env, "settingInfo"), settings);
}

Napi::Value napi_GetSettingsNoFilter(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<DeviceSettings *, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId]() -> DeviceSettings * { 
        DeviceSettings * const rawSetttings = Jabra_GetSettingsNoFilter(deviceId);

        if (!rawSetttings) {
          util::JabraException::LogAndThrow(functionName, "null returned");
        } else {
            IF_LOG_(LOGINSTANCE, plog::verbose) {
              LOG_VERBOSE_(LOGINSTANCE) << "napi_GetSettingsNoFilter got raw object : '" << toString(rawSetttings) << "'";
            }
        }

        return rawSetttings;
      }, [deviceId](const Napi::Env& env, DeviceSettings * const rawSetttings) {  
          Napi::Object napiResult = Napi::Object::New(env);
          if (rawSetttings) {
            toNodeType(deviceId, rawSetttings, napiResult);
          }
          return napiResult;
      }, [](DeviceSettings * rawSetttings) {
          if (rawSetttings) {
            Jabra_FreeDeviceSettings(rawSetttings);
          }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetSettings(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<DeviceSettings *, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId]() -> DeviceSettings * { 
        DeviceSettings * const rawSetttings = Jabra_GetSettings(deviceId);

        if (!rawSetttings) {
          util::JabraException::LogAndThrow(functionName, "null returned");
        } else {
            IF_LOG_(LOGINSTANCE, plog::verbose) {
              LOG_VERBOSE_(LOGINSTANCE) << "napi_GetSetting got raw object : '" << toString(rawSetttings) << "'";
            }
        }

        return rawSetttings;
      }, [deviceId](const Napi::Env& env, DeviceSettings * const rawSetttings) {  
          Napi::Object napiResult = Napi::Object::New(env);
          if (rawSetttings) {
            toNodeType(deviceId, rawSetttings, napiResult);
          }
          return napiResult;
      }, [](DeviceSettings * rawSetttings) {
          if (rawSetttings) {
            Jabra_FreeDeviceSettings(rawSetttings);
          }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_GetSetting(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::STRING, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    const std::string guid = info[1].As<Napi::String>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    (new util::JAsyncWorker<DeviceSettings *, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId, guid]() -> DeviceSettings * { 
        DeviceSettings * const rawSetttings = Jabra_GetSetting(deviceId, guid.c_str());

        if (!rawSetttings) {
          util::JabraException::LogAndThrow(functionName, "null returned");
        } else {
            IF_LOG_(LOGINSTANCE, plog::verbose) {
              LOG_VERBOSE_(LOGINSTANCE) << "napi_GetSetting got raw object : '" << toString(rawSetttings) << "'";
            }
        }
      
        return rawSetttings;
      }, [deviceId](const Napi::Env& env, DeviceSettings * const rawSetttings) {  
          Napi::Object napiResult = Napi::Object::New(env);
          if (rawSetttings) {
            toNodeType(deviceId, rawSetttings, napiResult);
          }
          return napiResult;
      }, [](DeviceSettings * rawSetttings) {
          if (rawSetttings) {
            // Created by Jabra SDK so we can release it using browser SDK.
            Jabra_FreeDeviceSettings(rawSetttings);
          }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_SetSettings(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::OBJECT, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Object settings = info[1].As<Napi::Object>();
    Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

    DeviceSettings * const rawDeviceSettings = toCType(deviceId, settings);
    IF_LOG_(LOGINSTANCE, plog::verbose) {
      LOG_VERBOSE_(LOGINSTANCE) << "napi_SetSettings translated settings input argument into raw object : '" << toString(rawDeviceSettings) << "'";
    }

    (new util::JAsyncWorker<void, void>(
      functionName,
      javascriptResultCallback,
      [functionName, deviceId, rawDeviceSettings](){
        Jabra_ReturnCode retv;
        if ((retv = Jabra_SetSettings(deviceId, rawDeviceSettings)) != Return_Ok) {
          util::JabraReturnCodeException::LogAndThrow(functionName, retv);
        }
      }, [rawDeviceSettings]() {
        if (rawDeviceSettings) {
          Custom_FreeDeviceSettings(rawDeviceSettings);
        }
      }
    ))->Queue();
  }

  return env.Undefined();
}

Napi::Value napi_FactoryReset(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Value, Jabra_ReturnCode>(functionName, info, [functionName](unsigned short deviceId) {
    Jabra_ReturnCode retv;        
    if ((retv = Jabra_FactoryReset(deviceId)) != Return_Ok) {
      util::JabraReturnCodeException::LogAndThrow(functionName, retv);
    }
    return retv;
  }, [](const Napi::Env& env, Jabra_ReturnCode cppResult) { return env.Undefined(); });
}

Napi::Value napi_IsSettingProtectionEnabled(const Napi::CallbackInfo& info) {
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsSettingProtectionEnabled(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) { return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsUploadImageSupported(const Napi::CallbackInfo& info) {
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsUploadImageSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) { return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsUploadRingtoneSupported(const Napi::CallbackInfo& info) {
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsUploadRingtoneSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) { return Napi::Boolean::New(env, cppResult); });
}

Napi::Value napi_IsFactoryResetSupported(const Napi::CallbackInfo& info) {
  return util::SimpleDeviceAsyncFunction<Napi::Boolean, bool>(__func__, info, [](unsigned short deviceId) {
    bool retv = Jabra_IsFactoryResetSupported(deviceId);
    return retv;
  }, [](const Napi::Env& env, bool cppResult) { return Napi::Boolean::New(env, cppResult); });
}

/**
* Copy a native sdk FailedSettings object into an empty napi Failed settings object (the reverse of toCType).
*/
static void toNodeType(const unsigned short deviceId, FailedSettings *src, Napi::Array& settingNames) {
  Napi::Env env = settingNames.Env();
  if(src!=nullptr)
  settingNames = Napi::Array::New(env, src->count);
  for (unsigned int i=0; i<src->count; ++i) {
    char* FailedSettingsSrc = src->settingNames[i];
    settingNames.Set(i, FailedSettingsSrc);
  }
}

Napi::Value napi_GetFailedSettingNames(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  Napi::Env env = info.Env();

  if (util::verifyArguments(functionName, info, {util::NUMBER, util::FUNCTION})) {
    const unsigned short deviceId = (unsigned short)(info[0].As<Napi::Number>().Int32Value());
    Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

    (new util::JAsyncWorker<FailedSettings *, Napi::Object>(
      functionName, 
      javascriptResultCallback,
      [functionName, deviceId]() -> FailedSettings * { 
        FailedSettings * const rawSetttings = Jabra_GetFailedSettingNames(deviceId);
        return rawSetttings;
      }, [deviceId](const Napi::Env& env, FailedSettings * const rawSetttings) {  
          Napi::Array napiResult = Napi::Array::New(env);
          if (rawSetttings != nullptr) {
            toNodeType(deviceId, rawSetttings, napiResult);
          }
          return napiResult;
      }, [](FailedSettings * rawSetttings) {
          if (rawSetttings != nullptr) {
            Jabra_FreeFailedSettings(rawSetttings);
          }
      }
    ))->Queue();
  }

  return env.Undefined();
}
