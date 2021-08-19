#include "pch.h"
#include "IbAhkSend.hpp"
using namespace Send;

#include "SendTypes/Types.hpp"

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

static std::unique_ptr<Type::Base> send;

class SendInputHook {
public:
    static inline decltype(SendInput)* SendInput_real = SendInput;
    static UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
        if (!hook)
            return SendInput_real(cInputs, pInputs, cbSize);

        return send->send_input(pInputs, cInputs);
    }

    //#TODO: only needed when two or more AHK processes exist?
    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real = GetAsyncKeyState;
    static SHORT WINAPI GetAsyncKeyState_detour(int vKey) {
        if (!hook)
            return GetAsyncKeyState_real(vKey);

        if constexpr (debug)
            DebugOStream() << L"GetAsyncKeyState: " << vKey << ", " << send->get_key_state(vKey) << std::endl;
        return send->get_key_state(vKey);
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


DLLAPI Send::Error __stdcall IbAhkSendInit(SendType type, InitFlags flags, void* argument) {
    if (type == SendType::AnyDriver) {
        Error error = IbAhkSendInit(SendType::Logitech, flags, nullptr);
        if (error == Error::Success) return Error::Success;

        error = IbAhkSendInit(SendType::DD, flags, nullptr);
        if (error == Error::Success) return Error::Success;

        return Error::DeviceNotFound;
    }
    else {
        switch (type) {
        case SendType::SendInput:
            {
                auto type = std::make_unique<Type::SendInput>();
                type->create_base(&SendInputHook::GetAsyncKeyState_real);
                Error error = type->create(&SendInputHook::SendInput_real);
                if (error != Error::Success)
                    return error;
                send = std::move(type);
            }
            break;
        case SendType::Logitech:
            {
                auto type = std::make_unique<Type::Logitech>();
                type->create_base(&SendInputHook::GetAsyncKeyState_real);
                Error error = type->create();
                if (error != Error::Success)
                    return error;
                send = std::move(type);
            }
            break;
        case SendType::DD:
            {
                auto type = std::make_unique<Type::DD>();
                type->create_base(&SendInputHook::GetAsyncKeyState_real);
                Error error = type->create(ib::Addr(argument));
                if (error != Error::Success)
                    return error;
                send = std::move(type);
            }
            break;
        default:
            return Error::InvalidArgument;
        }
        return Error::Success;
    }
}

DLLAPI void __stdcall IbAhkSendDestroy() {
    IbAhkSendInputHook(HookCode::Destroy);

    if (!send)
        return;
    send->destroy();
    send.release();
}

DLLAPI void __stdcall IbAhkSendSyncKeyStates() {
    send->sync_key_states();
}

DLLAPI UINT WINAPI IbAhkSendInput(UINT cInputs, LPINPUT pInputs, int cbSize) {
    return send->send_input(pInputs, cInputs);
}