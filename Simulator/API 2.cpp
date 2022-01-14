#include "pch.h"
#include <bit>
#include "IbInputSimulator.hpp"
using namespace Send;


DLLAPI VOID WINAPI IbSend_mouse_event(
    _In_ DWORD dwFlags,
    _In_ DWORD dx,
    _In_ DWORD dy,
    _In_ DWORD dwData,
    _In_ ULONG_PTR dwExtraInfo
) {
    INPUT input{
        .type = INPUT_MOUSE,
        .mi {
            .dx = std::bit_cast<LONG>(dx),
            .dy = std::bit_cast<LONG>(dy),
            .mouseData = dwData,
            .dwFlags = dwFlags,
            .time = 0,
            .dwExtraInfo = dwExtraInfo
        }
    };
    IbSendInput(1, &input, sizeof INPUT);
}

DLLAPI VOID WINAPI IbSend_keybd_event(
    _In_ BYTE bVk,
    _In_ BYTE bScan,
    _In_ DWORD dwFlags,
    _In_ ULONG_PTR dwExtraInfo
) {
    INPUT input{
        .type = INPUT_KEYBOARD,
        .ki {
            .wVk = bVk,
            .wScan = bScan,
            .dwFlags = dwFlags,
            .time = 0,
            .dwExtraInfo = dwExtraInfo
        }
    };
    IbSendInput(1, &input, sizeof INPUT);
}