#include "deviceconstants.h"
#include "Common.h"
#include <map>
#include <memory>
#include <utility>
#include <mutex>

/*
Every deviceID is associated with exactly one Jabra_Constants* and zero to multiple Jabra_Const.
When the device is removed, the map entry is removed, freeing the associated Jabra_Constants* and in turn, removing all Jabra_Const so they can no longer be indexed.
*/
typedef std::unique_ptr<Jabra_Constants, std::function<void(Jabra_Constants*)>> ConstantsPtr;
static std::map<int /*deviceID*/, std::pair<ConstantsPtr /*Jabra_Constants*/, std::vector<Jabra_Const>>> ConstantsMap;
static std::mutex ConstantsMutex;

int32_t FindJabraConst(const unsigned short deviceId, const Jabra_Const val)
{
    if (!val) return -1;

    std::lock_guard<std::mutex> lock(ConstantsMutex);
    if (ConstantsMap.count(deviceId) == 0)
        return -1;

    auto& ConstPtrs = ConstantsMap[deviceId].second;

    if (std::find(ConstPtrs.cbegin(), ConstPtrs.cend(), val) == ConstPtrs.cend())
        ConstPtrs.push_back(val);
     
     // We know for sure that it is present in the vector at this point
    const auto idx = std::find(ConstPtrs.cbegin(), ConstPtrs.cend(), val) - ConstPtrs.cbegin();
    return static_cast<int32_t>(idx);
}

Jabra_Const GetCachedJabraConst(const unsigned short deviceId, const int32_t idx)
{
    if (idx == -1) return nullptr;

    std::lock_guard<std::mutex> lock(ConstantsMutex);
    if (ConstantsMap.count(deviceId) == 0)
        return nullptr;

    auto& ConstPtrs = ConstantsMap[deviceId].second;

    if (static_cast<int32_t>(ConstPtrs.size()) - 1 < idx)
        return nullptr;

    return ConstPtrs[idx];
}

// GetConstSync(deviceId: number, key: string): number | undefined;
Napi::Value napi_GetConstSync(const Napi::CallbackInfo& info)
{
    if (!util::verifyArguments(__func__, info, {util::NUMBER, util::STRING}))
        return info.Env().Undefined();

    const unsigned short deviceId = static_cast<unsigned short>(info[0].As<Napi::Number>().Uint32Value());
    const std::string key = info[1].As<Napi::String>();

    Jabra_Const val = nullptr;
    {
        std::lock_guard<std::mutex> lock(ConstantsMutex);
        // If map does not contain an element for this device, we create a new Jabra_Constants* and insert it
        if (ConstantsMap.count(deviceId) == 0)
        {
            // We create a unique_ptr with a custom deleter so it will be freed automatically when deleted from the map
            ConstantsPtr ptr(Jabra_GetConstants(deviceId), Jabra_ReleaseConst);
            if (!ptr) return info.Env().Undefined();

            ConstantsMap.emplace(deviceId, std::make_pair(std::move(ptr), std::vector<Jabra_Const>()));
        }
        auto& Constants = ConstantsMap[deviceId];
        
        // Jabra_GetConst does not store/forward/free the provided root pointer so we can safely provide a raw ptr
        val = Jabra_GetConst(Constants.first.get(), key.c_str());
        if (!val) return info.Env().Undefined();
    }
    return Napi::Number::New(info.Env(), FindJabraConst(deviceId, val));
}

void freeConstants(unsigned short deviceId)
{
    std::lock_guard<std::mutex> lock(ConstantsMutex);
    ConstantsMap.erase(deviceId);
}

void freeConstants()
{
    std::lock_guard<std::mutex> lock(ConstantsMutex);
    ConstantsMap.clear();
}

// GetConstStringSync(deviceId: number, refKey: number): string | undefined;
Napi::Value napi_GetConstStringSync(const Napi::CallbackInfo& info)
{
    if (!util::verifyArguments(__func__, info, {util::NUMBER, util::NUMBER}))
        return info.Env().Undefined();

    const unsigned short deviceId = static_cast<unsigned short>(info[0].As<Napi::Number>().Uint32Value());
    const uint32_t refKey = info[1].As<Napi::Number>().Uint32Value();

    Jabra_Const val = GetCachedJabraConst(deviceId, refKey);
    if (!val || !Jabra_IsString(val))
        return info.Env().Undefined();
    
    return Napi::String::New(info.Env(), std::string(Jabra_AsString(val)));
}

// GetConstIntegerSync(deviceId: number, refKey: number): number | undefined;
Napi::Value napi_GetConstIntegerSync(const Napi::CallbackInfo& info)
{
    if (!util::verifyArguments(__func__, info, {util::NUMBER, util::NUMBER}))
        return info.Env().Undefined();

    const unsigned short deviceId = static_cast<unsigned short>(info[0].As<Napi::Number>().Uint32Value());
    const uint32_t refKey = info[1].As<Napi::Number>().Uint32Value();

    Jabra_Const val = GetCachedJabraConst(deviceId, refKey);
    if (!val || !Jabra_IsInt(val))
        return info.Env().Undefined();
    
    return Napi::Number::New(info.Env(), Jabra_AsInt(val));
}

// GetConstBooleanSync(deviceId: number, refKey: number): boolean | undefined;
Napi::Value napi_GetConstBooleanSync(const Napi::CallbackInfo& info)
{
    if (!util::verifyArguments(__func__, info, {util::NUMBER, util::NUMBER}))
        return info.Env().Undefined();

    const unsigned short deviceId = static_cast<unsigned short>(info[0].As<Napi::Number>().Uint32Value());
    const uint32_t refKey = info[1].As<Napi::Number>().Uint32Value();

    Jabra_Const val = GetCachedJabraConst(deviceId, refKey);
    if (!val || !Jabra_IsBool(val))
        return info.Env().Undefined();
    
    return Napi::Boolean::New(info.Env(), Jabra_AsBool(val));
}

// GetConstFieldSync(deviceId: number, refKey: number, id : string): number | undefined;
Napi::Value napi_GetConstFieldSync(const Napi::CallbackInfo& info)
{
    if (!util::verifyArguments(__func__, info, {util::NUMBER, util::NUMBER, util::STRING}))
        return info.Env().Undefined();

    const unsigned short deviceId = static_cast<unsigned short>(info[0].As<Napi::Number>().Uint32Value());
    const uint32_t refKey = info[1].As<Napi::Number>().Uint32Value();
    const std::string id = info[2].As<Napi::String>();

    auto node = GetCachedJabraConst(deviceId, refKey);
    if (!node) return info.Env().Undefined();
    
    auto val = Jabra_GetField(node, id.c_str());
    if (!val) return info.Env().Undefined();

    return Napi::Number::New(info.Env(), FindJabraConst(deviceId, val));
}

// GetConstListSync(deviceId: number, refKey: number, idx : number): number | undefined;
Napi::Value napi_GetConstListSync(const Napi::CallbackInfo& info)
{
    if (!util::verifyArguments(__func__, info, {util::NUMBER, util::NUMBER, util::NUMBER}))
        return info.Env().Undefined();

    const unsigned short deviceId = static_cast<unsigned short>(info[0].As<Napi::Number>().Uint32Value());
    const uint32_t refKey = info[1].As<Napi::Number>().Uint32Value();
    const int32_t idx = info[2].As<Napi::Number>().Int32Value();

    auto node = GetCachedJabraConst(deviceId, refKey);
    if (!node) return info.Env().Undefined();
    
    auto val = Jabra_ListElement(node, idx);
    if (!val) return info.Env().Undefined();

    return Napi::Number::New(info.Env(), FindJabraConst(deviceId, val));
}
