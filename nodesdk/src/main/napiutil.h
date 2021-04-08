#pragma once

#include <iostream>
#include <memory>
#include <thread>
#include <vector>

// Node lib headers:
#include <napi.h>

// Jabra lib headers:
#include "Common.h"
#include "JabraDeviceConfig.h"

// Own stuff:
#include "logger.h"

// -----------------------------------------Helper Macros ------------------------------------------------

/**
 * Helper macro that make sure a native function starting with "napi_" prefix is visiable in node
 * without the "napi_" prefix. Call this in module initialization.
 */
#define EXPORTS_SET(name) exports.Set(Napi::String::New(env, #name), Napi::Function::New(env, napi_##name));

// ----------------------------------------- Util --------------------------------------------------------

namespace util {

/**
 * Retrieve integer from n-api object or default value if it does not exist
 */ 
inline int32_t getObjInt32OrDefault(Napi::Object& obj, const char * name, int32_t defaultValue) {
    if (obj.Has(name)) {
        return obj.Get(name).As<Napi::Number>().Int32Value();
    } else {
        return defaultValue;
    }
}

/**
 * Retrieve bool value from n-api object or default value if it does not exist
 */ 
inline bool getObjBooleanOrDefault(Napi::Object& obj, const char * name, bool defaultValue) {
    if (obj.Has(name)) {
        return obj.Get(name).As<Napi::Boolean>().ToBoolean();
    } else {
        return defaultValue;
    }
}

/**
 * Retrieve string from n-api object or default value if it does not exist
 */
inline const std::string getObjStringOrDefault(Napi::Object& obj, const char * name, const std::string defaultValue) {
    if (obj.Has(name)) {
        return obj.Get(name).As<Napi::String>().ToString();
    } else {
        return defaultValue;
    }
}

/**
 * Retrieve enum value from n-api object or default value if it does not exist
 */
template<typename T>
const T getObjEnumValueOrDefault(Napi::Object& obj, const char * name, const T defaultValue) {
    if (obj.Has(name)) {
        return (T)obj.Get(name).As<Napi::Number>().Int32Value();
    } else {
        return defaultValue;
    }
}

/**
 * Should be thrown by api work functions when a jabra function fails with a specific error code.
 * 
 * Nb. the code throwing this exception should be placed in a worker or a wrapper function that
 * catches the exception and handles it.
 */
class JabraReturnCodeException : public std::runtime_error
{
    private:
    const std::string callerFunctionName;
    const Jabra_ReturnCode jabraApiReturnCode;

    static inline std::string generateString(const char * callerFunctionName, const Jabra_ReturnCode jabraApiReturnCode) {
        return std::string(callerFunctionName) + " got Jabra_SDK error " + std::to_string(jabraApiReturnCode);
    }

    public:
  	explicit JabraReturnCodeException(const char * callerFunctionName, const Jabra_ReturnCode jabraApiReturnCode)
		:  std::runtime_error(generateString(callerFunctionName, jabraApiReturnCode)), callerFunctionName(callerFunctionName), jabraApiReturnCode(jabraApiReturnCode) {}

    Jabra_ReturnCode getJabraApiReturnCode() const {
        return jabraApiReturnCode;
    }
    
    const std::string& getCallerFunctionName() const {
        return callerFunctionName;
    }
    
    static void LogAndThrow(const char * callerFunctionName, const Jabra_ReturnCode jabraApiReturnCode) {
        LOG_ERROR_(LOGINSTANCE) << generateString(callerFunctionName, jabraApiReturnCode);
        throw JabraReturnCodeException(callerFunctionName, jabraApiReturnCode);
    }
};

/**
 * Should be thrown by api work functions when a jabra function fails without an error code.
 * 
 * Nb. the code throwing this exception should be placed in a worker or a wrapper function that
 * catches the exception and handles it.
 */
class JabraException : public std::runtime_error
{
    private:
    const std::string callerFunctionName;
    const std::string reason;

    static inline std::string generateString(const char * callerFunctionName, const std::string& reason) {
        if (reason.length() > 0) {
            return std::string(callerFunctionName) + " failed with reason " + reason;
        } else {
            return std::string(callerFunctionName) + " failed.";
        }
    }

    public:
    explicit JabraException(const char * callerFunctionName, const std::string& reason = "")
		:  std::runtime_error(generateString(callerFunctionName, reason)), callerFunctionName(callerFunctionName), reason(reason) {}
    
    const std::string& getCallerFunctionName() const {
        return callerFunctionName;
    }

    const std::string& getReason() const {
        return reason;
    }
    
    static void LogAndThrow(const char * callerFunctionName, const std::string& reason = "") {
        LOG_ERROR_(LOGINSTANCE) << generateString(callerFunctionName, reason);
        throw JabraException(callerFunctionName, reason);
    }
};

/**
* Used by verifyArguments´etc. to describe the type of javascript paramter to a native function.
*/
enum FormalParameterType
{
    _VOID, // Prefixed with "_" to avoid clash with Windows.h macro.
    BOOLEAN,
    NUMBER,
    // BIGINT,
    STRING,
    SYMBOL,
    ARRAYBUFFER,
    TYPEDARRAY,
    OBJECT,
    ARRAY,
    FUNCTION,
    PROMISE,
    DATAVIEW,
    BUFFER,
    EXTERNAL,
    OBJECT_OR_STRING
};

/** 
 * Helper that checks if arguments of correct type exists - throws napi exception and returns false if not.
 **/
bool verifyArguments(const char * const callerFunctionName, const Napi::CallbackInfo &info, std::initializer_list<FormalParameterType> expectedFormalParameterTypes);

// --- Async helpers ------------------------------------------------------------------------------------------------

/**
 * Async worker utility that can run any Jabra (lambda) work function asynchronously.
 * 
 * This is the central worker that (almost all) n-api implementations of Jabra functions 
 * should use to make sure code is non-blocking. This utility may be used directly for
 * complex cases or indirectly thorugh high-level helpers like f.x. SimpleDeviceAsyncFunction 
 * for simple cases.
 * 
 * The sole exception where this worker should NOT be used, is for handling Jabra c-callbacks
 * in init and eventhandlers!
 * 
 * Nb. Based on Napi::AsyncWorker that self-destorys (no explicit delete required)
 */
template <typename JabraWorkReturnType, typename NapiReturnType>
class JAsyncWorker : public Napi::AsyncWorker
{
  private:
    Jabra_ReturnCode errorCode;
    JabraWorkReturnType jabraResult;
    const char * const callerFunctionName;
    const std::function<JabraWorkReturnType()> jabraWorkFunc;
    const std::function<NapiReturnType(const Napi::Env& env, const JabraWorkReturnType& jabraData)> jabraToNapiMapperFunc;
    const std::function<void(JabraWorkReturnType& jabraData)> jabraCleanupFunc;

  public:
    /**
     * Construct a new worker:
     * 
     * @callerFunctionName Thread-invariant name of function calling this worker used for documentation (should generally be called with __func__).
     * @javascriptResultCallback The javascript function to call back with the final result.
     * @jabraWorkFunc The async code (jabra sdk call) return a C data type (must NOT use any javascript / napi code or types).
     * @jabraToNapiMapperFunc Synchronous code converting the C data type to a javascript napi type.
     * @jabraCleanupFunc Synchronous code doing cleanup. 
     */
    JAsyncWorker(const char * const callerFunctionName, 
                 const Napi::Function &javascriptResultCallback, 
                 const std::function<JabraWorkReturnType()>& jabraWorkFunc,
                 const std::function<NapiReturnType(const Napi::Env& env, const JabraWorkReturnType& jabraData)>& jabraToNapiMapperFunc,
                 const std::function<void(JabraWorkReturnType& jabraData)>& jabraCleanupFunc = [](JabraWorkReturnType& jabraData) {}
                ) : Napi::AsyncWorker(javascriptResultCallback), errorCode(Jabra_ReturnCode::Return_Ok), jabraResult(), callerFunctionName(callerFunctionName), jabraWorkFunc(jabraWorkFunc), jabraToNapiMapperFunc(jabraToNapiMapperFunc), jabraCleanupFunc(jabraCleanupFunc) {}
    JAsyncWorker(const JAsyncWorker&) = delete;
    ~JAsyncWorker() {}

    void okError(const Napi::Env& env, const std::string& errorMsg, bool duringJsCallback) {
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        try {
            if (!duringJsCallback) {
                Callback().Call({ Napi::String::New(env, errorMsg), env.Undefined() });
            }
        } catch (const std::exception &e) {
            LOG_ERROR_(LOGINSTANCE) << "Failed calling error callback with details " + std::string(e.what());
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE) << "Failed calling error callback";
        }
    }

    void executeError(const std::string& errorMsg, const Jabra_ReturnCode _errorCode = Jabra_ReturnCode::Return_Ok) {
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        SetError(errorMsg);
        errorCode = _errorCode;
    }

    // Executed inside the worker-thread.
    // It is not safe to access JS engine data structure
    // here, so everything we need for input and output
    // should go on `this`.
    void Execute()
    {
        try
        {
            LOG_DEBUG_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " started async function call";
            jabraResult = jabraWorkFunc();
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " finished async function call";      
        }
        catch (const JabraReturnCodeException &e)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(e.what());
            executeError(errorMsg, e.getJabraApiReturnCode());
        }
        catch (const JabraException &e)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(e.what());
            executeError(errorMsg);
        }
        catch (const std::exception &e)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(callerFunctionName) + " -> " + e.what();
            executeError(errorMsg);
        }
        catch (...)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(callerFunctionName) + " -> unknown failure";
            executeError(errorMsg);
        }
    }

    void cleanup() {
        try {
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " started cleanup.";
            jabraCleanupFunc(jabraResult);
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " completed (and finished cleanup).";
        } catch (const std::exception &e) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker cleanup failure with details " + std::string(e.what());
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker cleanup failure";
        }
    }

    // Executed when the async work is complete
    // this function will be run inside the main event loop
    // so it is safe to use JS engine data again
    void OnOK()
    {
        Napi::Env env = Env();
        Napi::HandleScope scope(env);
        NapiReturnType napiResult;
        bool callBackError = false;

        try {
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " started mapping.";
            napiResult = jabraToNapiMapperFunc(env, jabraResult);
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " finished mapping.";            
            
            callBackError = true;
            // TODO: Should Receiver().Value() be passed as first arg ?
            Callback().Call({ env.Undefined(), napiResult });
            callBackError = false;
        }
        catch (const JabraReturnCodeException &e)
        {
            const std::string errorMsg = "JAsyncWorker ok failure: " + std::string(e.what());
            okError(env, errorMsg, callBackError);
        }
        catch (const JabraException &e)
        {
            const std::string errorMsg = "JAsyncWorker ok failure: " + std::string(e.what());
            okError(env, errorMsg, callBackError);
        }
        catch (const std::exception &e)
        {
            const std::string errorMsg = "JAsyncWorker ok failure: " + std::string(callerFunctionName) + " -> " + e.what();
            okError(env, errorMsg, callBackError);
        }
        catch (...)
        {
            const std::string errorMsg = "JAsyncWorker ok failure: " + std::string(callerFunctionName) + " -> unknown failure";
            okError(env, errorMsg, callBackError);
        }

        cleanup();
    }

    // Executed when the async work fails.
    // this function will be run inside the main event loop
    // so it is safe to use JS engine data again
    void OnError(const Napi::Error& e) 
    {
        try {
            Napi::Env env = e.Env();

            // Ugly code here: Unfortunately "e" is const, so we need to cast it away to modify the error:
            Napi::Error& mutableError = const_cast<Napi::Error&>(e);
            if (errorCode != Jabra_ReturnCode::Return_Ok) {
                mutableError.Set(Napi::String::New(env, "code"), (Napi::Number::New(env, (int)errorCode)));
            }

            Callback().Call(Receiver().Value(), std::initializer_list<napi_value>{ mutableError.Value() });
        } catch (const std::exception &e) {
            LOG_ERROR_(LOGINSTANCE) << "Failed calling error callback with details " + std::string(e.what());
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE) << "Failed calling error callback";
        }

        cleanup();
    }
};

/**
 * Async worker utility specilization that can run any Jabra (lambda) work procedure asynchronously.
 * 
 * Use this version when nothing should be returned from the async work other than a notification that
 * it is completed (by calling the callback).
 * 
 * See full template of JAsyncWorker template for additional information !
 */
template <>
class JAsyncWorker<void,void> : public Napi::AsyncWorker
{
  private:
    typedef Napi::AsyncWorker base;

    Jabra_ReturnCode errorCode;
    const char * const callerFunctionName;
    const std::function<void()> jabraWorkFunc;
    const std::function<void()> jabraCleanupFunc;

  public:
    /**
     * Create a new worker.
     * @callerFunctionName Thread-invariant name of function calling this worker used for documentation (should generally be called with __func__).
     * @javascriptResultCallback The javascript function to call back with the final result.
     * @jabraWorkFunc The async code (jabra sdk call) return a C data type (must NOT use any javascript / napi code or types).
     * @jabraCleanupFunc Synchronous code doing cleanup.
     */
    JAsyncWorker(const char * const callerFunctionName, 
                 const Napi::Function &javascriptResultCallback, 
                 const std::function<void()>& jabraWorkFunc,
                 const std::function<void()>& jabraCleanupFunc = [](){}
                ) : Napi::AsyncWorker(javascriptResultCallback), errorCode(Jabra_ReturnCode::Return_Ok), callerFunctionName(callerFunctionName), jabraWorkFunc(jabraWorkFunc), jabraCleanupFunc(jabraCleanupFunc) {}
    JAsyncWorker(const JAsyncWorker&) = delete;
    ~JAsyncWorker() {}

    void executeError(const std::string& errorMsg, const Jabra_ReturnCode _errorCode = Jabra_ReturnCode::Return_Ok) {
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        SetError(errorMsg);
        errorCode = _errorCode;
    }

    // Executed inside the worker-thread.
    // It is not safe to access JS engine data structure
    // here, so everything we need for input and output
    // should go on `this`.
    void Execute()
    {
        try
        {
            LOG_DEBUG_(LOGINSTANCE) << callerFunctionName << " started async prodcedure call";
            jabraWorkFunc();
            LOG_VERBOSE_(LOGINSTANCE) << callerFunctionName << " finished async procedure call";
        }
        catch (const JabraReturnCodeException &e)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(e.what());
            executeError(errorMsg, e.getJabraApiReturnCode());
        }
        catch (const JabraException &e)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(e.what());
            executeError(errorMsg);
        }
        catch (const std::exception &e)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(callerFunctionName) + " -> " + e.what();
            executeError(errorMsg);
        }
        catch (...)
        {
            const std::string errorMsg = "JAsyncWorker execute failure: " + std::string(callerFunctionName) + " -> unknown failure";
            executeError(errorMsg);
        }
    }

    void cleanup() {
        try {
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " started cleanup.";
            jabraCleanupFunc();
            LOG_VERBOSE_(LOGINSTANCE) << "JAsyncWorker: " << callerFunctionName << " completed (and finished cleanup).";
        } catch (const std::exception &e) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker cleanup failure with details " + std::string(e.what());
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker cleanup failure";
        }
    }

    // Executed when the async work is complete
    // this function will be run inside the main event loop
    // so it is safe to use JS engine data again
    void OnOK()
    {
        Napi::Env env = Env();
        Napi::HandleScope scope(env);

        try {
            Callback().Call({ env.Undefined(), env.Undefined() });
        } catch (const std::exception &e) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker ok callback failure with details " + std::string(e.what());
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker ok callback failure";
        }

        cleanup();
    }

    // Executed when the async work fails.
    // this function will be run inside the main event loop
    // so it is safe to use JS engine data again
    void OnError(const Napi::Error& e) 
    {
        try {
            Napi::Env env = e.Env();

            // Ugly code here: Unfortunately "e" is const, so we need to cast it away to modify the error:
            Napi::Error& mutableError = const_cast<Napi::Error&>(e);
            if (errorCode != Jabra_ReturnCode::Return_Ok) {
                mutableError.Set(Napi::String::New(env, "code"), (Napi::Number::New(env, (int)errorCode)));
            }

            Callback().Call(Receiver().Value(), std::initializer_list<napi_value>{ mutableError.Value() });
        } catch (const std::exception &e) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker error callback failure with details " + std::string(e.what());
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE) << "JAsyncWorker error callback failure";
        }

        cleanup();
    }
};

/** 
* Does all the skeleton work for a simple call to a async jabra call without arguments returning
* a specific node type by a callback. The specific jabraWorkFunc function should do the actual async work, while jabraToNapiMapperFunc 
* should convert the managed c++ result to a napi type that can be passed to the callback.
*
* @callerFunctionName Thread-invariant name of function calling this code used for documentation (should generally be called with __func__).
* @info The javascript n-api function parameter informaton.
* @jabraWorkFunc The async code (jabra sdk call) return a C data type (must NOT use any javascript / napi code or types).
* @jabraToNapiMapperFunc Synchronous code converting the C data type to a javascript napi type.
* @jabraCleanupFunc Synchronous code doing cleanup. 
**/
template <typename NapiReturnType, typename cppReturnType>
Napi::Value SimpleAsyncFunction(const char * const callerFunctionName,
                                      const Napi::CallbackInfo &info, 
                                      const std::function<cppReturnType ()>& jabraWorkFunc,
                                      const std::function<NapiReturnType(const Napi::Env& env, const cppReturnType& jabraData)>& jabraToNapiMapperFunc,
                                      const std::function<void(cppReturnType& jabraData)>& jabraCleanupFunc = [](cppReturnType& jabraData) {}
                                     )
{
    Napi::Env env = info.Env();

    if (util::verifyArguments(callerFunctionName, info, { util::FUNCTION }))
    {
        Napi::Function javascriptResultCallback = info[0].As<Napi::Function>();

        auto *const worker = new util::JAsyncWorker<cppReturnType, NapiReturnType>
              (callerFunctionName, 
               javascriptResultCallback, 
               jabraWorkFunc,
               jabraToNapiMapperFunc,
               jabraCleanupFunc
              );

        worker->Queue();
    }

    return env.Undefined();
};


/** 
* Does all the skeleton work for a simple call to a async jabra call taking a deviceid as sole argument and returning
* a specific node type by a callback. The specific jabraWorkFunc function should do the actual async work, while jabraToNapiMapperFunc 
* should convert the managed c++ result to a napi type that can be passed to the callback.
*
* @callerFunctionName Thread-invariant name of function calling this code used for documentation (should generally be called with __func__).
* @info The javascript n-api function parameter informaton.
* @jabraWorkFunc The async code (jabra sdk call) return a C data type (must NOT use any javascript / napi code or types).
* @jabraToNapiMapperFunc Synchronous code converting the C data type to a javascript napi type.
* @jabraCleanupFunc Synchronous code doing cleanup. 
**/
template <typename NapiReturnType, typename cppReturnType>
Napi::Value SimpleDeviceAsyncFunction(const char * const callerFunctionName,
                                      const Napi::CallbackInfo &info, 
                                      const std::function<cppReturnType (unsigned short)>& jabraWorkFunc,
                                      const std::function<NapiReturnType(const Napi::Env& env, const cppReturnType& jabraData)>& jabraToNapiMapperFunc,
                                      const std::function<void(cppReturnType& jabraData)>& jabraCleanupFunc = [](cppReturnType& jabraData) {}
                                     )
{
    Napi::Env env = info.Env();

    if (util::verifyArguments(callerFunctionName, info, {util::NUMBER, util::FUNCTION}))
    {
        const unsigned short deviceId = info[0].As<Napi::Number>().Int32Value();
        Napi::Function javascriptResultCallback = info[1].As<Napi::Function>();

        auto *const worker = new util::JAsyncWorker<cppReturnType, NapiReturnType>
              (callerFunctionName, 
               javascriptResultCallback, 
               std::bind(jabraWorkFunc, deviceId),
               jabraToNapiMapperFunc,
               jabraCleanupFunc
              );

        worker->Queue();
    }

    return env.Undefined();
};


/** 
* Does all the skeleton work for a simple call to a async jabra setter taking a deviceid and a boolean as arguments with no result.
* The specific jabraWorkFunc function should do the actual async work
*
* @callerFunctionName Thread-invariant name of function calling this code used for documentation (should generally be called with __func__).
* @info The javascript n-api function parameter informaton.
* @jabraWorkFunc The async code (jabra sdk call) return a C data type (must NOT use any javascript / napi code or types).
* @jabraCleanupFunc Synchronous code doing cleanup. 
**/
inline Napi::Value SimpleDeviceAsyncBoolSetter(const char * const callerFunctionName,
                                               const Napi::CallbackInfo &info, 
                                               const std::function<void (unsigned short, bool enable)>& jabraWorkFunc,
                                               const std::function<void()>& jabraCleanupFunc = []() {}
                                              )
{
    Napi::Env env = info.Env();

    if (util::verifyArguments(callerFunctionName, info, {util::NUMBER, util::BOOLEAN, util::FUNCTION}))
    {
        const unsigned short deviceId = info[0].As<Napi::Number>().Int32Value();
        const bool enable = info[1].As<Napi::Boolean>().ToBoolean();
        Napi::Function javascriptResultCallback = info[2].As<Napi::Function>();

        auto *const worker = new util::JAsyncWorker<void, void>
              (callerFunctionName, 
               javascriptResultCallback, 
               std::bind(jabraWorkFunc, deviceId, enable),
               jabraCleanupFunc
              );

        worker->Queue();
    }

    return env.Undefined();
};



// Sync helpers ------------------------------------------------------------------------------------------

/**
 * Calls sync function returing values directly and throwing errors as JS exceptions (no callbacks).
 * 
 * @callerFunctionName Thread-invariant name of function calling this code used for documentation (should generally be called with __func__).
 * @info The javascript n-api function parameter informaton.
 * @func The sync code (jabra sdk call) return a n-api value of type T.
 */
template <typename T> 
T JSyncWrapper(const char * const callerFunctionName, const Napi::CallbackInfo& info, const std::function<T (const char * const callerFunctionName, const Napi::CallbackInfo&)> func) {
    try
    {
        LOG_DEBUG_(LOGINSTANCE) << "JSyncWrapper: " << callerFunctionName << " started sync function call.";
        auto result = func(callerFunctionName, info);
        LOG_VERBOSE_(LOGINSTANCE) << "JSyncWrapper: " << callerFunctionName << " completed sync function call.";

        return result;
    }
    catch (const Napi::Error& e) {
        const std::string errorMsg = "JSyncWrapper execute failure: " + std::string(e.what());
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        throw; // Rethrow napi exceptions as they are handled.
    }
    catch (const JabraReturnCodeException &e)
    {
        const std::string errorMsg = "JSyncWrapper execute failure: " + std::string(e.what());
        LOG_ERROR_(LOGINSTANCE) << errorMsg;

        Napi::Env env = info.Env();
        Napi::Error error = Napi::Error::New(env, errorMsg);

        Jabra_ReturnCode errorCode = e.getJabraApiReturnCode();
        if (errorCode != Jabra_ReturnCode::Return_Ok) {
          error.Set(Napi::String::New(env, "code"), (Napi::Number::New(env, (int)errorCode)));
        }

        error.ThrowAsJavaScriptException();
    }
    catch (const JabraException &e)
    {
        const std::string errorMsg = "JSyncWrapper execute failure: " + std::string(e.what());
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
    }
    catch (const std::exception &e)
    {
        const std::string errorMsg = "JSyncWrapper execute failure : " + std::string(callerFunctionName) + " -> " + e.what();
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
    }
    catch (...)
    {
        const std::string errorMsg = "JSyncWrapper execute failure : " + std::string(callerFunctionName) + " -> unknown error";
        LOG_ERROR_(LOGINSTANCE) << errorMsg;
        Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
    }

    return info.Env().Undefined(); // dummy
};


/**
 * Create a C-string suitable for storing in a settings object from a std:string 
 */
char * newCString(const std::string& src);

/**
 * Create a C-string suitable for storing in a settings object from a napi string/null object.
 **/
char * newCString(const Napi::Value& src);

/**
 * Encode a std::string to UTF-8.
 *
 * @param[in]   str         The string to be encoded.
 * @param[in]   callerName  The name of the caller function. Used only for
 *                          logging purposes in case of errors.
 * @param[in]   charset     The encoding of str.
 * @return  `str` encoded in UTF-8.
 */
std::string toUtf8(const std::string& str, const char* const callerName,
    const std::string& charset = "");

} // namespace util

// -------------------------------------------------------------------------------------------------------
