#pragma once
#include <IbWinCppLib/WinCppLib.hpp>

#include "Base.hpp"

namespace Send::Type::Internal {
    class DD final : virtual public Base, public VirtualKeyStates {
        KeyboardModifiers modifiers;
        std::mutex keyboard_mutex;

        HMODULE dd;
        int (*DD_btn)(int btn);
        int (*DD_whl)(int whl);
        int (*DD_mov)(int x, int y);
        int (*DD_movR)(int dx, int dy);

        int (*DD_todc)(int vk);
        int (*DD_key)(int keycode, int flag);
        //int (*DD_str)(char* str);
    public:
        DD() : VirtualKeyStates(modifiers, keyboard_mutex) {}

        Error create(const wchar_t* dd_lib) {
            if (dd_lib == nullptr) {
                if (create(L"DD94687.64.dll") == Error::Success
                    || create(L"DD64.dll") == Error::Success
                    || create(L"DDHID64.dll") == Error::Success)
                    return Error::Success;
                else
                    return Error::LibraryNotFound;  
            } else {
                dd = LoadLibraryW(dd_lib);
                if (!dd)
                    return Error::LibraryLoadFailed;

                DD_btn = ib::Addr(GetProcAddress(dd, "DD_btn"));
                if (!DD_btn) {
                    FreeLibrary(dd);
                    return Error::LibraryLoadFailed;
                }
                DD_whl = ib::Addr(GetProcAddress(dd, "DD_whl"));
                DD_mov = ib::Addr(GetProcAddress(dd, "DD_mov"));
                DD_movR = ib::Addr(GetProcAddress(dd, "DD_movR"));

                DD_todc = ib::Addr(GetProcAddress(dd, "DD_todc"));
                DD_key = ib::Addr(GetProcAddress(dd, "DD_key"));
                //DD_str = ib::Addr(GetProcAddress(dd, "DD_str"));

                if (!(DD_whl && DD_mov && DD_movR && DD_todc && DD_key /*&& DD_str*/)) {
                    FreeLibrary(dd);
                    return Error::LibraryLoadFailed;
                }

                if (DD_btn(0) != 1) {
                    FreeLibrary(dd);
                    return Error::LibraryError;
                }

                return Error::Success;
            }
        }

        void destroy() override {
            FreeLibrary(dd);
        }

        bool send_mouse_input(const MOUSEINPUT& mi) override {
            //#TODO: MOUSEEVENTF_MOVE_NOCOALESCE, MOUSEEVENTF_VIRTUALDESK
            if (mi.dwFlags & MOUSEEVENTF_MOVE) {
                POINT move{ mi.dx, mi.dy };
                if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
                    mouse_absolute_to_screen(move);
                    if constexpr (debug)
                        DebugOStream() << L"DD_mov: (" << move.x << L", " << move.y << L")\n";
                    DD_mov(move.x, move.y);
                } else {
                    if constexpr (debug)
                        DebugOStream() << L"DD_movR: (" << move.x << L", " << move.y << L")\n";
                    DD_movR(move.x, move.y);
                }
            }

            if (mi.dwFlags & 0x7E) {
                DWORD keys[] = { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP };
                for (DWORD key : keys)
                    if (mi.dwFlags & key) {
                        if constexpr (debug)
                            DebugOStream() << L"DD_btn: " << (key >> 1) << L"\n";
                        DD_btn(key >> 1);
                    }
            }
            if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
                bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
                switch (mi.mouseData) {
                case XBUTTON1: DD_btn(down ? 64 : 128); break;
                case XBUTTON2: DD_btn(down ? 256 : 512); break;
                }
            } else if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
                int32_t v = std::bit_cast<int32_t>(mi.mouseData);
                int sign = v > 0 ? 1 : -1;
                if constexpr (debug)
                    DebugOStream() << L"DD_whl: " << v << L"\n";
                for (DWORD i = 0; i < abs(v); i++)
                    DD_whl(sign);
            }

            return true;  //#TODO
        }

        bool send_keyboard_input(const KEYBDINPUT& ki) override {
            std::lock_guard lock(keyboard_mutex);

            bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);
            if (is_modifier(ki.wVk))
                set_modifier_state(ki.wVk, keydown);
            
            int dd_code = DD_todc(ki.wVk);
            if (dd_code == -1) {
                // (some version) DD do not support left/right modifiers
                if (ki.wVk == VK_LSHIFT || ki.wVk == VK_RSHIFT)
                    dd_code = DD_todc(VK_SHIFT);
                else if (ki.wVk == VK_LCONTROL || ki.wVk == VK_RCONTROL)
                    dd_code = DD_todc(VK_CONTROL);
                else if (ki.wVk == VK_LMENU || ki.wVk == VK_RMENU)
                    dd_code = DD_todc(VK_MENU);

                if (dd_code == -1) {
                    if constexpr (debug)
                        DebugOStream() << L"DD_todc: unsupported key " << ki.wVk << L"\n";
                    return false;
                }
                else {
                    if constexpr (debug)
                        DebugOStream() << L"DD_todc: left/right modifier key (" << ki.wVk << ") is unsupported" << L"\n";
                }
            }
            if constexpr (debug)
                DebugOStream() << L"DD_key: " << dd_code << L" (vk" << ki.wVk << L"), " << (keydown ? 1 : 2) << L")\n";
            DD_key(dd_code, keydown ? 1 : 2);  //#TODO: result?

            return true;
        }

#pragma warning(suppress : 4250)  //'class1' : inherits 'class2::member' via dominance
    };
}