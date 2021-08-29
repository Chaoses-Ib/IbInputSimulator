#pragma once
#include <Windows.h>
#include <stdint.h>

#ifdef IB_AHKSEND_DLLEXPORT
#define DLLAPI extern "C" __declspec(dllexport)
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif

namespace Send {
    enum class Error : uint32_t {
        Success,
        InvalidArgument,
        LibraryNotFound,
        LibraryLoadFailed,
        LibraryError,
        DeviceCreateFailed,
        DeviceNotFound,
        DeviceOpenFailed
    };

    enum class SendType : uint32_t {
        AnyDriver,
        SendInput,
        Logitech,
        DD
    };

    using InitFlags = const uint32_t;
    struct Init {
        using T = InitFlags;
    };

    enum class HookCode : uint32_t {
        Off,
        On,
        InitOnly,
        Destroy
    };
}

DLLAPI Send::Error __stdcall IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument);
DLLAPI void __stdcall IbSendDestroy();

DLLAPI void __stdcall IbSendSyncKeyStates();

#pragma region API 1

DLLAPI UINT WINAPI IbSendInput(
    _In_ UINT cInputs,                    // number of input in the array
    _In_reads_(cInputs) LPINPUT pInputs,  // array of inputs
    _In_ int cbSize                       // sizeof(INPUT)
);

DLLAPI void __stdcall IbSendInputHook(Send::HookCode code);

#pragma endregion


#pragma region API 2

DLLAPI VOID WINAPI IbSend_mouse_event(
    _In_ DWORD dwFlags,         //MOUSEEVENTF_
    _In_ DWORD dx,
    _In_ DWORD dy,
    _In_ DWORD dwData,
    _In_ ULONG_PTR dwExtraInfo
);

DLLAPI VOID WINAPI IbSend_keybd_event(
    _In_ BYTE bVk,
    _In_ BYTE bScan,
    _In_ DWORD dwFlags,         //KEYEVENTF_
    _In_ ULONG_PTR dwExtraInfo
);

#pragma endregion