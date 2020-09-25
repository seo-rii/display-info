#include <napi.h>
#include <windows.h>
#include <vector>
#include <string>
#include <tuple>

using monitorTuple = std::tuple<unsigned int, std::string, std::tuple<int, int, int>>;

unsigned int i_hash(char *str) {
    unsigned int hash_val = 5381;
    unsigned int c;
    while (c = *str++) {
        hash_val = ((hash_val << 5) + hash_val) + c;
    }
    return hash_val;
}

Napi::Object getMonitorInfo(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    try
    {
        DISPLAY_DEVICE device = {0}, monitor = {0};
        device.cb = sizeof(DISPLAY_DEVICE);
        monitor.cb = sizeof(DISPLAY_DEVICE);
        std::vector<monitorTuple> monitorList;
        for (int displayIndex = 0;; ++displayIndex) {
            monitorTuple monitorInfo;
            if (!EnumDisplayDevices(NULL, displayIndex, &device, EDD_GET_DEVICE_INTERFACE_NAME))
                break;

            char szDeviceName[32]={0};
            lstrcpy(szDeviceName, device.DeviceName);

            for(int monitorIndex=0;;++monitorIndex) {
                DEVMODE devmode = {0};
                devmode.dmSize = sizeof(DEVMODE);
                devmode.dmDriverExtra = 0;
                if (!EnumDisplayDevices(szDeviceName, monitorIndex, &monitor, 0)) break;
                if (!EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &devmode)) continue;

                std::get<0>(monitorInfo) = i_hash(monitor.DeviceName);
                std::get<1>(monitorInfo) = std::string(monitor.DeviceString);
                std::get<2>(monitorInfo) = std::make_tuple(devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmDisplayFrequency);

                /*
                for (int modes = 0;; ++modes) {
                    if (!EnumDisplaySettings(device.DeviceName, modes, &devmode)) break;
                    //printf("%d %d %d\n",devmode.dmPelsWidth,devmode.dmPelsHeight,devmode.dmDisplayFrequency);
                }
                */

                monitorList.push_back(monitorInfo);
            }
        }

        Napi::Object returnArray = Napi::Array::New(info.Env());
        for (int i = 0; i < monitorList.size(); ++i)
        {
            Napi::Object ret = Napi::Object::New(env);
            ret.Set("id", Napi::Number::New(env, std::get<0>(monitorList[i])));
            ret.Set("name", Napi::String::New(env, std::get<1>(monitorList[i])));
            ret.Set("refreshRate", Napi::Number::New(env, std::get<2>(std::get<2>(monitorList[i]))));
            Napi::Object monitorSize = Napi::Object::New(env);
            monitorSize.Set("width", Napi::Number::New(env, std::get<0>(std::get<2>(monitorList[i]))));
            monitorSize.Set("height", Napi::Number::New(env, std::get<1>(std::get<2>(monitorList[i]))));
            ret.Set("bound", monitorSize);
            returnArray.Set(i, ret);
        }
        return returnArray;
    }
    catch (const char *ex)
    {
        Napi::Object ret = Napi::Object::New(env);
        return ret;
    }
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "getMonitorInfo"),
                Napi::Function::New(env, getMonitorInfo));
    return exports;
}

NODE_API_MODULE(displayInfo, Init)
