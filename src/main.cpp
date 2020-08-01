#include <napi.h>
#include <windows.h>
#include <vector>
#include <string>
#include <tuple>

Napi::Object getMonitorInfo(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    try
    {
        DISPLAY_DEVICE device = {0};
        device.cb = sizeof(DISPLAY_DEVICE);
        std::vector<std::tuple<std::string, std::tuple<int, int, int>, std::vector<std::tuple<int, int, int>>>> monitorList;
        for (int index = 0;; ++index)
        {
            std::tuple<std::string, std::tuple<int, int, int>, std::vector<std::tuple<int, int, int>>> monitorInfo;
            if (!EnumDisplayDevices(NULL, index, &device, EDD_GET_DEVICE_INTERFACE_NAME))
                break;
            std::get<0>(monitorInfo) = std::string(device.DeviceName);
            DEVMODE devmode = {0};
            devmode.dmSize = sizeof(DEVMODE);
            devmode.dmDriverExtra = 0;

            if (EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &devmode) == 0)
                continue;

            std::get<1>(monitorInfo) = std::make_tuple(devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmDisplayFrequency);

            /*
            for (int modes = 0;; ++modes)
            {
                if (!EnumDisplaySettings(device.DeviceName, modes, &devmode))
                    break;
                //printf("%d %d %d\n",devmode.dmPelsWidth,devmode.dmPelsHeight,devmode.dmDisplayFrequency);
            }
            */

            monitorList.push_back(monitorInfo);
        }

        Napi::Object returnArray = Napi::Array::New(info.Env());
        for (int i = 0; i < monitorList.size(); ++i)
        {
            Napi::Object ret = Napi::Object::New(env);
            ret.Set("name", Napi::String::New(env, std::get<0>(monitorList[i])));
            ret.Set("refreshRate", Napi::Number::New(env, std::get<2>(std::get<1>(monitorList[i]))));
            Napi::Object monitorSize = Napi::Object::New(env);
            monitorSize.Set("width", Napi::Number::New(env, std::get<0>(std::get<1>(monitorList[i]))));
            monitorSize.Set("height", Napi::Number::New(env, std::get<1>(std::get<1>(monitorList[i]))));
            ret.Set("size", monitorSize);
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