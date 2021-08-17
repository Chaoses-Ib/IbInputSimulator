#include "pch.h"
#include "IbAhkSend.hpp"
using namespace Send;

#include "Driver.hpp"

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

DLLAPI Send::Error __stdcall IbAhkSendInit(SendType type, InitFlags flags, void* argument) {
    if (Error error = driver.create(); error != Error::Success)
        return error;

    send.create(driver);
    return Error::Success;
}

DLLAPI void __stdcall IbAhkSendDestroy() {
    IbAhkSendInputHook(HookCode::Destroy);

    if (!send.has_created())
        return;
    send.destroy();
    driver.destroy();
}

DLLAPI void __stdcall IbAhkSendSyncKeyStates() {
    send->sync_key_states();
}

DLLAPI UINT WINAPI IbAhkSendInput(UINT cInputs, LPINPUT pInputs, int cbSize) {
    send->send_input(pInputs, cInputs);
    return cInputs;  //#TODO
}


class SendInputHook {
    static inline decltype(SendInput)* SendInput_real = SendInput;
    static UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
        if (!hook)
            return SendInput_real(cInputs, pInputs, cbSize);

        send->send_input(pInputs, cInputs);
        return cInputs;
    }

    //#TODO: only needed when two or more AHK processes exist?
    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real = GetAsyncKeyState;
    static SHORT WINAPI GetAsyncKeyState_detour(int vKey) {
        if (!hook)
            return GetAsyncKeyState_real(vKey);

        if constexpr (debug)
            DebugOStream() << L"GetAsyncKeyState: " << vKey << ", " << send->get_key_state(vKey, GetAsyncKeyState_real) << std::endl;
        return send->get_key_state(vKey, GetAsyncKeyState_real);
    }
public:
    static inline bool hook;

    SendInputHook() {
        hook = false;
        IbDetourAttach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
        IbDetourAttach(&SendInput_real, SendInput_detour);
    }

    ~SendInputHook() {
        IbDetourDetach(&SendInput_real, SendInput_detour);
        IbDetourDetach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    }
};
ib::HolderB<SendInputHook> sendinput_hook;

DLLAPI void __stdcall IbAhkSendInputHook(HookCode code) {
    switch (code) {
    case HookCode::InitOnly:
        sendinput_hook.create();
        break;
    case HookCode::Destroy:
        sendinput_hook.destroy();
        break;
    case HookCode::On:
        if (!sendinput_hook.has_created())
            sendinput_hook.create();
        sendinput_hook->hook = true;
        break;
    case HookCode::Off:
        sendinput_hook->hook = false;
        break;
    }
}