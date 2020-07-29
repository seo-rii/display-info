#include <napi.h>
#include <windows.h>

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    int *Count = (int*)dwData;
    (*Count)++;
    return TRUE;
}

Napi::Object getMonitorCount(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::Object ret = Napi::Object::New(env);


    int Count = 0;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count);
    ret.Set("monitor_count", Napi::Number::New(env, Count));
    return ret;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "getMonitorCount"),
                Napi::Function::New(env, getMonitorCount));
    return exports;
}

NODE_API_MODULE(displayInfo, Init)