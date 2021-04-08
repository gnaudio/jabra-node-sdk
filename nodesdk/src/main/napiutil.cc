#include "napiutil.h"

// OS-specific macros
#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#define WIN32
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

namespace util {
    static inline std::string toString(FormalParameterType type) {
        switch (type) {
            case _VOID: return "void"; break;
            case BOOLEAN: return "boolean"; break;
            case NUMBER: return "number"; break;
            // BIGINT: return "bigint"; break;
            case STRING: return "string"; break;
            case SYMBOL: return "symbol"; break;
            case ARRAYBUFFER: return "arraybuffer"; break;
            case TYPEDARRAY: return "typedarray"; break;
            case OBJECT: return "object"; break;
            case ARRAY: return "array"; break;
            case FUNCTION: return "function"; break;
            case PROMISE: return "promise"; break;
            case DATAVIEW: return"dataview"; break;
            case BUFFER: return "buffer"; break;
            case EXTERNAL: return "external"; break;
            case OBJECT_OR_STRING: return "object | string"; break;
            default: return "???"; break;
        }
    }


    static inline bool verifyValueType(const Napi::Value& value, FormalParameterType type) {
        switch (type) {
            case _VOID: return value.IsNull() || value.IsUndefined(); break;
            case BOOLEAN: return value.IsBoolean(); break;
            case NUMBER: return value.IsNumber(); break;
            // BIGINT: return value.IsBigInt(); break;
            case STRING: return value.IsString(); break;
            case SYMBOL: return value.IsSymbol(); break;
            case ARRAYBUFFER: return value.IsArrayBuffer(); break;
            case TYPEDARRAY: return value.IsTypedArray(); break;
            case OBJECT: return value.IsObject(); break;
            case ARRAY: return value.IsArray(); break;
            case FUNCTION: return value.IsFunction(); break;
            case PROMISE: return value.IsPromise(); break;
            case DATAVIEW: return value.IsDataView(); break;
            case BUFFER: return value.IsBuffer(); break;
            case EXTERNAL: return value.IsExternal(); break;
            case OBJECT_OR_STRING: return value.IsObject() || value.IsString(); break;
            default: throw std::runtime_error(std::string("Unknown enum type value " + std::to_string(type)));
        }
    }
    
    #ifdef WIN32
    /**
     * Return the error message for the latest error.
     *
     * @return  The Windows-formatted error message for the laters error.
     */
    static std::string getErrorMessage() {
        /*
         * Shortly, this function calls FormatMessage with GetLastError() and
         * a bunch of other parameters to get a dynamically allocated error
         * message, which is copied into a std::string and then deallocated.
         */

        DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS;
        DWORD lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        LPTSTR msg = nullptr;

        FormatMessage(flags, NULL, GetLastError(), lang, (LPTSTR) &msg, 0,
            nullptr);

        std::string result = msg;
        LocalFree(msg);

        return result;
    }

    /**
     * Converts a wide string to a UTF-8 multi-byte string.
     *
     * @param   src     The wide string to be converted. Must not be empty.
     *                  The last element must be a null-terminator.
     * @return  `src` converted to a UTF-8 multi-byte string.
     */
    static std::string toMultiByte(const std::vector<WCHAR>& src) {
        /*
         * First we call WideCharToMultiByte with 0 as the char string buffer
         * length (sixth parameter). This returns the length the buffer should
         * have to contain the result of the conversion.
         */
        int srcLength = static_cast<int>(src.size());
        int dstLength = WideCharToMultiByte(CP_UTF8, 0x0,
            (LPWSTR) src.data(), srcLength, nullptr, 0, NULL, NULL);
        if (dstLength <= 0) {
            JabraException::LogAndThrow(__func__, getErrorMessage());
        }

        /*
         * Here we actually perform the conversion, after creating a buffer of
         * the desired length inside an std::vector.
         *
         * We can't use and std::string to hold the buffer, because writing to
         * its .data() member directly violates encapsulation (basically, it
         * messes up with the null-terminator).
         */
        std::vector<char> dst(dstLength);
        bool error = 0 == WideCharToMultiByte(CP_UTF8, 0x0,
            (LPWSTR) src.data(), srcLength, (LPSTR) dst.data(), dstLength,
            NULL, NULL);
        if (error) {
            JabraException::LogAndThrow(__func__, getErrorMessage());
        }

        /*
         * An std::string is being created from the C string filled by
         * WideCharToMultiByte. We use dst.data() instead of the more idiomatic
         * C++ dst.begin() and dst.end() because that would add the
         * null-terminator would as an extra character at the end of the
         * std::string.
         */
        return std::string(dst.data());
    }

    /**
     * Converts a string to a UTF-16 wide string.
     *
     * @param   src     The string to be converted. Must not be empty.
     * @return  `src` converted to a UTF-16 wide string, including the
     *          null-terminator as the last element.
     */
    static std::vector<WCHAR> toWideChar(const std::string& src) {
        /*
         * First we call MultiByteToWideChar with 0 as the wide string buffer
         * length (last parameter). This returns the length the buffer should
         * have to contain the result of the conversion.
         *
         * `src.length() + 1` accounts for the null-terminator, which is
         * included by .data() since C++11.
         *
         * `srcLength` is necessary as a std::string can contain null-terminators
         * before the last character. This makes -1 not an option, since `src`'s
         * content doesn't necessarily end at the first null-terminator.
         */
        int srcLength = static_cast<int>(src.length() + 1);
        int dstLength = MultiByteToWideChar(CP_ACP, 0, src.data(), srcLength,
            nullptr, 0);
        if (dstLength <= 0) {
            JabraException::LogAndThrow(__func__, getErrorMessage());
        }

        /*
         * Here we actually perform the conversion, after creating a buffer of
         * the desired length inside an std::vector.
         */
        std::vector<WCHAR> dst(dstLength);
        bool error = 0 == MultiByteToWideChar(CP_ACP, 0, src.data(), srcLength,
            (LPWSTR) dst.data(), dstLength);
        if (error) {
            JabraException::LogAndThrow(__func__, getErrorMessage());
        }

        return dst;
    }

    #endif

    bool verifyArguments(const char * const functionName, const Napi::CallbackInfo& info, std::initializer_list<FormalParameterType> expectedArgumentTypes) {
        const Napi::Env env = info.Env();

        if (info.Length() != expectedArgumentTypes.size()) {
            const std::string errMsg = "Wrong number of arguments to " + std::string(functionName) + " (got #" + std::to_string(info.Length()) + ",  expected #" + std::to_string( expectedArgumentTypes.size()) + ")";
            LOG_ERROR_(LOGINSTANCE) << errMsg;
            Napi::TypeError::New(env, errMsg).ThrowAsJavaScriptException();
            return false;
        }


        int j = 0;
        for (std::initializer_list<FormalParameterType>::iterator it=expectedArgumentTypes.begin(); it!=expectedArgumentTypes.end(); ++it) {
            if (!verifyValueType(info[j], *it)) {
                const std::string errMsg = "Wrong type of argument to " + std::string(functionName) + " #" +  std::to_string(j) + ". Got value of unexpected " + toString(*it) + " type)";
                LOG_ERROR_(LOGINSTANCE) << errMsg;
                Napi::TypeError::New(env, errMsg).ThrowAsJavaScriptException();
                return false;
            }
            ++j;
        }

        return true;
    }


    /**
     * Create a C-string from a std:string 
     */
    char * newCString(const std::string& src) {      
        char * cpy = new char[src.length() + 1];
        strncpy(cpy, src.c_str(), src.length());
        cpy[src.length()] = 0;

        return cpy;
    }

    /**
     * Create a C-string from a napi string/null object.
     */
    char * newCString(const Napi::Value& src) {
        if (src.IsString()) {
            Napi::String strSrc = src.As<Napi::String>();
            return newCString((std::string)strSrc);
        } else {
            return nullptr;
        }
    }

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
            const std::string& charset) {
        #ifndef WIN32

        // There's no need to do anything on non-Windows platforms.
        return str;

        #else

        /*
         * This function uses Windows APIs to convert a string to UTF-8.
         *
         * It calls MultiByteToWideChar to actually perform the conversion.
         * However, this has the side-effect of tuning the string to a wide
         * string. To solve this, we just call WideCharToMultiByte to turn the
         * wide string back to a multi-byte string.
         *
         * The second argument (charset) is not used right now, but it's kept
         * for future extensions.
         */

        /*
         * This serves the double purpose of excluding some corner cases later
         * and avoiding many unnecessary computations.
         */
        if (str.length() == 0) {
            return str;
        }

        try {
            return toMultiByte(toWideChar(str));
        } catch (const JabraException& e) {
            LOG_ERROR_(LOGINSTANCE)
                << "Error with the Windows API while converting a string to UTF-8: "
                << e.getReason();
            throw;
        } catch (const std::exception& e) {
            LOG_ERROR_(LOGINSTANCE)
                << "Error while converting a string to UTF-8: "
                << std::string(e.what());
            throw;
        } catch (...) {
            LOG_ERROR_(LOGINSTANCE)
                << "Unknown error while converting a string to UTF-8";
            throw;
        }

        #endif
    }
}
