#include "pch.h"
#include "Driver.hpp"

extern "C" __declspec(dllexport) void __stdcall IbAhkSendDestroy();

static decltype(SendInput)* SendInput_real = SendInput;
UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize);
static decltype(GetAsyncKeyState)* GetAsyncKeyState_real = GetAsyncKeyState;
SHORT WINAPI GetAsyncKeyState_detour(int vKey);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        IbAhkSendDestroy();
        break;
    }
    return TRUE;
}

static LogiDriver driver;
static ib::HolderB<DriverSendInput> send;
static bool hook;

extern "C" __declspec(dllexport) int __stdcall IbAhkSendInit() {
    LogiDriver::Error error = driver.create();
    if (error != LogiDriver::Error::Success)
        return (int)error;

    send.create(driver);

    hook = false;
    IbDetourAttach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    IbDetourAttach(&SendInput_real, SendInput_detour);

    return 0;
}

extern "C" __declspec(dllexport) void __stdcall IbAhkSendDestroy() {
    if (!send.has_created())
        return;

    IbDetourDetach(&SendInput_real, SendInput_detour);
    IbDetourDetach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    send.destroy();
    driver.destroy();
}

extern "C" __declspec(dllexport) bool __stdcall IbAhkSendInputHookBegin() {
    if (!send.has_created())
        return false;

    send->sync_key_states();
    hook = true;
    return true;
}
extern "C" __declspec(dllexport) void __stdcall IbAhkSendInputHookEnd() {
    hook = false;
}

UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
    if (!hook)
        return SendInput_real(cInputs, pInputs, cbSize);

    send->send_input(pInputs, cInputs);
    return cInputs;
}

extern "C" __declspec(dllexport) UINT WINAPI IbAhkSendInput(UINT cInputs, LPINPUT pInputs, int cbSize) {
    send->send_input(pInputs, cInputs);
    return cInputs;
}

//#TODO: only needed when two or more AHK processes exist?
SHORT WINAPI GetAsyncKeyState_detour(int vKey) {
    if (!hook)
        return GetAsyncKeyState_real(vKey);

    if constexpr (debug)
        DebugOStream() << L"GetAsyncKeyState: " << vKey << ", " << send->get_key_state(vKey, GetAsyncKeyState_real) << std::endl;
    return send->get_key_state(vKey, GetAsyncKeyState_real);
}