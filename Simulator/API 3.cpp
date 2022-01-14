#include "pch.h"
#include <bit>
#include <cassert>
#include "IbInputSimulator.hpp"
using namespace Send;


DLLAPI bool __stdcall IbSendMouseMove(uint32_t x, uint32_t y, Send::MoveMode mode) {
    INPUT input {
        .type = INPUT_MOUSE,
        .mi {
            .dx = std::bit_cast<LONG>(x),
            .dy = std::bit_cast<LONG>(y),
            .mouseData = 0,
            .dwFlags = [mode]() -> DWORD {
                switch (mode) {
                    case MoveMode::Absolute: return MOUSEEVENTF_ABSOLUTE;
                    case MoveMode::Relative: return MOUSEEVENTF_MOVE;
                    default: assert(false);
                }
            }(),
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    return IbSendInput(1, &input, sizeof INPUT);
}

DLLAPI bool __stdcall IbSendMouseClick(Send::MouseButton button) {
    INPUT inputs[2];
    inputs[0] = inputs[1] = {
        .type = INPUT_MOUSE,
        .mi {
            .dx = 0,
            .dy = 0,
            .mouseData = 0,
            .time = 0,
            .dwExtraInfo = 0
        }
    };

    switch (button) {
        case MouseButton::Left:
            inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
            return IbSendInput(2, inputs, sizeof INPUT) == 2;
        case MouseButton::Right:
            inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            return IbSendInput(2, inputs, sizeof INPUT) == 2;
        case MouseButton::Middle:
            inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            return IbSendInput(2, inputs, sizeof INPUT) == 2;
        case MouseButton::XButton1:
            inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
            inputs[0].mi.mouseData = inputs[1].mi.mouseData = XBUTTON1;
            return IbSendInput(2, inputs, sizeof INPUT) == 2;
        case MouseButton::XButton2:
            inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
            inputs[0].mi.mouseData = inputs[1].mi.mouseData = XBUTTON2;
            return IbSendInput(2, inputs, sizeof INPUT) == 2;
        default:
            inputs[0].mi.dwFlags = static_cast<DWORD>(button);
            return IbSendInput(1, inputs, sizeof INPUT);
    }
}

DLLAPI bool __stdcall IbSendMouseWheel(int32_t movement) {
    INPUT input{
        .type = INPUT_MOUSE,
        .mi {
            .dx = 0,
            .dy = 0,
            .mouseData = std::bit_cast<DWORD>(movement),
            .dwFlags = MOUSEEVENTF_WHEEL,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    return IbSendInput(1, &input, sizeof INPUT);
}

DLLAPI bool __stdcall IbSendKeybdDown(uint16_t vk) {
    INPUT input{
        .type = INPUT_KEYBOARD,
        .ki {
            .wVk = vk,
            .wScan = 0,
            .dwFlags = 0,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    return IbSendInput(1, &input, sizeof INPUT);
}

DLLAPI bool __stdcall IbSendKeybdUp(uint16_t vk) {
    INPUT input{
        .type = INPUT_KEYBOARD,
        .ki {
            .wVk = vk,
            .wScan = 0,
            .dwFlags = KEYEVENTF_KEYUP,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    return IbSendInput(1, &input, sizeof INPUT);
}

DLLAPI bool __stdcall IbSendKeybdDownUp(uint16_t vk, Send::KeyboardModifiers modifiers) {
    constexpr INPUT blank_input = INPUT{ .type = INPUT_KEYBOARD, .ki { 0, 0, 0, 0, 0 }};

    INPUT inputs[18];  //720 bytes
    uint32_t i = 0;

#define CODE_GENERATE(vk, member)  \
    if (modifiers.member) {  \
        inputs[i] = blank_input;  \
        inputs[i].ki.wVk = vk;  \
        i++;  \
    }

    CODE_GENERATE(VK_LCONTROL, LCtrl)
    CODE_GENERATE(VK_RCONTROL, RCtrl)
    CODE_GENERATE(VK_LSHIFT, LShift)
    CODE_GENERATE(VK_RSHIFT, RShift)
    CODE_GENERATE(VK_LMENU, LAlt)
    CODE_GENERATE(VK_RMENU, RAlt)
    CODE_GENERATE(VK_LWIN, LWin)
    CODE_GENERATE(VK_RWIN, RWin)
#undef CODE_GENERATE
    inputs[i] = blank_input;
    inputs[i].ki.wVk = vk;
    i++;

    inputs[i] = inputs[i-1];
    inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
    i++;
#define CODE_GENERATE(vk, member)  \
    if (modifiers.member) {  \
        inputs[i] = blank_input;  \
        inputs[i].ki.wVk = vk;  \
        inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;  \
        i++;  \
    }

    CODE_GENERATE(VK_RWIN, RWin)
    CODE_GENERATE(VK_LWIN, LWin)
    CODE_GENERATE(VK_RMENU, RAlt)
    CODE_GENERATE(VK_LMENU, LAlt)
    CODE_GENERATE(VK_RSHIFT, RShift)
    CODE_GENERATE(VK_LSHIFT, LShift)
    CODE_GENERATE(VK_RCONTROL, RCtrl)
    CODE_GENERATE(VK_LCONTROL, LCtrl)
#undef CODE_GENERATE

    return IbSendInput(i, inputs, sizeof INPUT) == i;
}