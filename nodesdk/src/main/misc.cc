#include "misc.h"

// https://www.linuxquestions.org/questions/programming-9/c-bin-array-to-hex-string-and-back-4175560352/
std::string ToHex(const char* buffer, size_t size){
	std::stringstream str;
	str.setf(std::ios_base::hex, std::ios::basefield);
	str.setf(std::ios_base::uppercase);
	str.fill('0');
	for(size_t i=0; i<size; ++i){
		str << std::setw(2) << (unsigned short)(unsigned char)buffer[i];
	}
	return str.str();
}

Napi::Value napi_GetPanics(const Napi::CallbackInfo& info) {
  const char * const functionName = __func__;
  return util::SimpleDeviceAsyncFunction<Napi::Array, std::vector<std::string>>(functionName, info, 
    [functionName](unsigned short deviceId) {
      std::vector<std::string> v = { };
      Jabra_PanicListType *panics;
      panics = Jabra_GetPanics(deviceId);
      if (panics != NULL)
      {
        for (unsigned int i = 0; i < panics->entriesNo; i++)
        {
          uint8_t *panicCode;
          panicCode = panics->panicList[i].panicCode;
          std::string hexstring = ToHex((char*)panicCode, 25); // To replace magic number with sizeof.
          v.push_back(hexstring);
        }
        Jabra_FreePanicListType(panics);
      }
      return v;
    }, 
    [](const Napi::Env& env, const std::vector<std::string> cppResult) { 
        Napi::Array arr = Napi::Array::New(env);
        uint32_t i = 0;
        for(auto const& item: cppResult) {
          Napi::String s = Napi::String::New(env, item);
          arr[i++] = s;
        }
        return arr;
      }
  );
}