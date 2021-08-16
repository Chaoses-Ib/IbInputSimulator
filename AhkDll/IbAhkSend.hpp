#pragma once
#include <Windows.h>

#ifdef IB_AHKSEND_DLLEXPORT
#define DLLAPI extern "C" __declspec(dllexport)
#undef IB_AHKSEND_DLLEXPORT
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif

namespace Send {
    enum class HookCode : int {
        Off,
        On,
        InitOnly,
        Destroy
    };
}

DLLAPI int __stdcall IbAhkSendInit();
DLLAPI void __stdcall IbAhkSendDestroy();
DLLAPI void __stdcall IbAhkSendSyncKeyStates();
DLLAPI UINT WINAPI IbAhkSendInput(UINT cInputs, LPINPUT pInputs, int cbSize);
DLLAPI void __stdcall IbAhkSendInputHook(Send::HookCode code);