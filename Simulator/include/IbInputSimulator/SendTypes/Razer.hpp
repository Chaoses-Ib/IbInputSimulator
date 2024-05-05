#pragma once
#include <ntddkbd.h>
#include <ntddmou.h>
#include "base.hpp"
#include "usb.hpp"

namespace Send::Type::Internal {
    class Razer final : public VirtualKeyStates {
        HANDLE device;

        KeyboardModifiers modifiers;
        std::mutex keyboard_mutex;
    public:
        Razer() : VirtualKeyStates(modifiers, keyboard_mutex) {}

        Error create() {
            std::wstring device_name = find_device([](std::wstring_view sv) {
                using namespace std::literals;

                //RZCONTROL#VID_1532&PID_0306&MI_00#3&1c65d7f8&0#{e3be005d-d130-4910-88ff-09ae02f680e9}
                return sv.starts_with(L"RZCONTROL#"sv) && sv.ends_with(L"#{e3be005d-d130-4910-88ff-09ae02f680e9}"sv);
            });
            if (device_name.empty())
                return Error::DeviceNotFound;

            device = CreateFileW(
                device_name.c_str(),
                GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                OPEN_EXISTING, 0, NULL
            );
            if (device == INVALID_HANDLE_VALUE)
                return Error::DeviceOpenFailed;

            return Error::Success;
        }

        void destroy() override {
            CloseHandle(device);
        }

        struct RzControl {
            uint32_t unk1;
            enum class Type : uint32_t {
                Keyboard = 1,
                Mouse = 2
            } type;
            union {
                //struct {
                //    uint32_t absolute_coord;
                //    struct {
                //        bool LButtonDown : 1;
                //        bool LButtonUp : 1;
                //        bool RButtonDown : 1;
                //        bool RButtonUp : 1;
                //        bool MButtonDown : 1;
                //        bool MButtonUp : 1;
                //        bool XButton1Down : 1;
                //        bool XButton1Up : 1;
                //        bool XButton2Down : 1;
                //        bool XButton2Up : 1;
                //        bool Wheel : 1;
                //        bool HWheel : 1;
                //        uint8_t unk : 4;
                //    private:
                //        void assert_size() {
                //            static_assert(sizeof(*this) == 2);
                //        }
                //    } btn;
                //    int16_t movement;
                //    uint32_t unk1;
                //    int32_t x;
                //    int32_t y;
                //    uint32_t unk2;
                //} mi;

                MOUSE_INPUT_DATA mi;

                //struct {
                //    uint16_t unk1;
                //    int16_t key;
                //    uint16_t action;
                //    uint16_t unk2;
                //    uint32_t unk3;
                //    uint32_t unk4;
                //    uint32_t unk5;
                //    uint32_t unk6;
                //} ki;

                /// The high byte of MakeCode has no effect, extended keys are supported by Flags
                KEYBOARD_INPUT_DATA ki;
            };
        private:
            void assert_size() {
                static_assert(sizeof RzControl == 32);
            }
        };

        bool send_mouse_input(const MOUSEINPUT& mi) override {
            RzControl control{ .type = RzControl::Type::Mouse };

            if (mi.dwFlags & MOUSEEVENTF_MOVE) {
                POINT move{ mi.dx, mi.dy };
                if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
                    control.mi.Flags = MOUSE_MOVE_ABSOLUTE;
                }
                control.mi.LastX = mi.dx;
                control.mi.LastY = mi.dy;
            }

            if (mi.dwFlags & MOUSEEVENTF_LEFTDOWN)
                control.mi.ButtonFlags |= MOUSE_LEFT_BUTTON_DOWN;
            if (mi.dwFlags & MOUSEEVENTF_LEFTUP)
                control.mi.ButtonFlags |= MOUSE_LEFT_BUTTON_UP;
            if (mi.dwFlags & MOUSEEVENTF_RIGHTDOWN)
                control.mi.ButtonFlags |= MOUSE_RIGHT_BUTTON_DOWN;
            if (mi.dwFlags & MOUSEEVENTF_RIGHTUP)
                control.mi.ButtonFlags |= MOUSE_RIGHT_BUTTON_UP;
            if (mi.dwFlags & MOUSEEVENTF_MIDDLEDOWN)
                control.mi.ButtonFlags |= MOUSE_MIDDLE_BUTTON_DOWN;
            if (mi.dwFlags & MOUSEEVENTF_MIDDLEUP)
                control.mi.ButtonFlags |= MOUSE_MIDDLE_BUTTON_UP;

            if (mi.dwFlags & MOUSEEVENTF_XDOWN) {
                switch (mi.mouseData) {
                case XBUTTON1: control.mi.ButtonFlags |= MOUSE_BUTTON_4_DOWN; break;
                case XBUTTON2: control.mi.ButtonFlags |= MOUSE_BUTTON_5_DOWN; break;
                }
            }
            if (mi.dwFlags & MOUSEEVENTF_XUP) {
                switch (mi.mouseData) {
                case XBUTTON1: control.mi.ButtonFlags |= MOUSE_BUTTON_4_UP; break;
                case XBUTTON2: control.mi.ButtonFlags |= MOUSE_BUTTON_5_UP; break;
                }
            }

            if (mi.dwFlags & MOUSEEVENTF_WHEEL || mi.dwFlags & MOUSEEVENTF_HWHEEL) {
                if (mi.dwFlags & MOUSEEVENTF_WHEEL)
                    control.mi.ButtonFlags |= MOUSE_WHEEL;
                else
                    control.mi.ButtonFlags |= MOUSE_HWHEEL;
                control.mi.ButtonData = 120 * std::bit_cast<int32_t>(mi.mouseData);  //#TODO
            }

            if constexpr (debug)
                DebugOStream() << L"send_mouse_input: " << *(uint16_t*)&control.mi.ButtonFlags << L", (" << control.mi.LastX << L", " << control.mi.LastY << L") " << control.mi.Flags << std::endl;

            DWORD bytes_returned;
            return DeviceIoControl(device, 0x88883020, &control, sizeof control, nullptr, 0, &bytes_returned, nullptr);
        }

        bool send_keyboard_input(const KEYBDINPUT& ki) override {
            RzControl control{ .type = RzControl::Type::Keyboard };

            if ((control.ki.MakeCode = keyboard_vk_to_key(ki.wVk)) < 0)
                return false;

            std::lock_guard lock(keyboard_mutex);

            bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);
            if (is_modifier(ki.wVk))
                set_modifier_state(ki.wVk, keydown);

            control.ki.Flags = keydown ? KEY_MAKE : KEY_BREAK;

            // Extended keys
            // TODO: E1
            switch (ki.wVk)
            {
            case VK_SNAPSHOT:
            case VK_INSERT:
            case VK_HOME:
            case VK_PRIOR:
            case VK_DELETE:
            case VK_END:
            case VK_NEXT:
            case VK_RIGHT:
            case VK_LEFT:
            case VK_DOWN:
            case VK_UP:
            case VK_LWIN:
            case VK_RWIN:
                control.ki.Flags |= KEY_E0;
                break;
            default:
                break;
            }

            if constexpr (debug)
                DebugOStream() << L"send_keyboard_input: " << control.ki.MakeCode << ", " << control.ki.Flags << std::endl;

            DWORD bytes_returned;
            return DeviceIoControl(device, 0x88883020, &control, sizeof control, nullptr, 0, &bytes_returned, nullptr);
        }

        int16_t keyboard_vk_to_key(uint8_t vk) {
            if constexpr (debug)
                DebugOStream() << L"keyboard_vk_to_key: " << vk << L" -> " << Usb::keyboard_vk_to_usage(vk) << L" -> " << keyboard_usage_to_key(Usb::keyboard_vk_to_usage(vk)) << std::endl;
            return keyboard_usage_to_key(Usb::keyboard_vk_to_usage(vk));
        }

        /// - For extended keys, only the lowest byte is returned.
        /// - RCtrl and RAlt will be treated as left ones.
        constexpr int16_t keyboard_usage_to_key(uint16_t usage_id) {
            // Extracted from kbddef.dat in Razer Synapse
            //
            // Differences from HidP_TranslateUsagesToI8042ScanCodes() and MapVirtualKey():
            // vk                 | razer | HidP_TranslateUsagesToI8042ScanCodes | MapVirtualKey
            // ---                | ---   | ---                                  | ---
            // VK_PAUSE (0x13)    | 0x1D  | 0x1D                                 | MAPVK_VK_TO_VSC: 0 / MAPVK_VK_TO_VSC_EX: 0x1D
            // VK_SNAPSHOT (0x2C) | 0x37  | 0x2A / 0x37                          | 0x54

            if (usage_id <= 115) {
                constexpr uint16_t table[] = { -1, 255, 252, -3, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 37, 38, 50, 49, 24, 25, 16, 19, 31, 20, 22, 47, 17, 45, 21, 44, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 28, 1, 14, 15, 57, 12, 13, 26, 27, 43, 43, 39, 40, 41, 51, 52, 53, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 87, 88, 55, 70, 29, 82, 71, 73, 83, 79, 81, 77, 75, 80, 72, 69, 53, 55, 74, 78, 28, 79, 80, 81, 75, 76, 77, 71, 72, 73, 82, 83, 86, 93, 94, 89, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 118, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, 94, 95, 99, -3, 126, -3, 115, 112, 125, 121, 123, 92, -3, -3, -3, 242, 241, 120, 119, 118 };
                return table[usage_id];
            } else if (181 <= usage_id && usage_id <= 183) {
                constexpr uint16_t table[] = { 25,16,36 };
                return table[usage_id - 181];
            } else if (usage_id == 205)
                return 34;
            else if (224 <= usage_id && usage_id <= 234) {
                constexpr uint16_t table[] = { 29, 42, 56, 91, 29, 54, 56, 92, -2, 48, 46 };
                return table[usage_id - 224];
            }
            return -1;
        }
    };
}