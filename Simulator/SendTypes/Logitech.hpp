#pragma once
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "../common.hpp"
#include "Base.hpp"
#include "Usb.hpp"

namespace Send::Type::Internal {
    class LogitechDriver {
        HANDLE device;
        bool has_acceleration;

    public:
        Error create() {
            std::wstring device_name = find_device();
            if (device_name.empty())
                return Error::DeviceNotFound;

            device = CreateFileW(
                device_name.c_str(),
                GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                OPEN_EXISTING, 0, NULL
            );
            if (device == INVALID_HANDLE_VALUE)
                return Error::DeviceOpenFailed;

            int acceleration = get_has_acceleration();
            /* #TODO: G HUB
            if (acceleration < 0) {
                CloseHandle(device);
                return Error::LogiSettingsNotFound;
            }
            */
            has_acceleration = acceleration == 1;

            return Error::Success;
        }

        void destroy() {
            CloseHandle(device);
        }

    private:
        int get_has_acceleration() {
            //#TODO: G HUB

            using namespace rapidjson;
            //#TODO: 20 warnings... maybe getline is better? or disable warnings for rapidjson?

            //fopen doesn't support environment variables
            wchar_t path[MAX_PATH];
            ExpandEnvironmentStringsW(LR"(%LOCALAPPDATA%\Logitech\Logitech Gaming Software\settings.json)", path, (DWORD)std::size(path));
            FILE* file = _wfsopen(path, L"rb", _SH_DENYNO);
            if (!file) {
                return -1;
            }
            char* buf = new char[16 * 1024];
            FileReadStream is(file, buf, sizeof(buf));

            int result;
            Document doc;
            doc.ParseStream(is);

            //pointer.hasAcceleration
            //#TODO: FindMember
            if (doc.HasMember("pointer")) {
                auto& pointer = doc["pointer"];
                if (pointer.HasMember("hasAcceleration")) {
                    result = pointer["hasAcceleration"].GetBool();
                }
                else {
                    //hasAcceleration not found
                    result = 0;
                }
            }
            else {
                //pointer not found
                result = 0;
            }

            fclose(file);
            delete[] buf;

            return result;
        }

        std::wstring find_device() {
            return Internal::find_device([](std::wstring_view sv) {
                using namespace std::literals;

                //LGS:
                //ROOT#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
                //ROOT#SYSTEM#0001#{5bada891-842b-4296-a496-68ae931aa16c}
                //Root#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
                //Root#SYSTEM#0001#{5bada891-842b-4296-a496-68ae931aa16c}

                //G HUB:
                //ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}
                //ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015} ?
                //ROOT#SYSTEM#0001#{dfbedcdb-2148-416d-9e4d-cecc2424128c}

                return (sv.starts_with(L"ROOT#SYSTEM#"sv)
                     || sv.starts_with(L"Root#SYSTEM#"sv))
                     &&
                       (sv.ends_with(L"#{1abc05c0-c378-41b9-9cef-df1aba82b015}"sv)
                     || sv.ends_with(L"#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}"sv)
                     || sv.ends_with(L"#{dfbedcdb-2148-416d-9e4d-cecc2424128c}"sv)
                     || sv.ends_with(L"#{5bada891-842b-4296-a496-68ae931aa16c}"sv));
            });
        }

    public:
        struct MouseButton {
            bool LButton : 1;
            bool RButton : 1;
            bool MButton : 1;
            bool XButton1 : 1;
            bool XButton2 : 1;
            bool unknown : 3;
        };

        struct MouseReport {
            union {
                MouseButton button;
                Byte button_byte;
            };
            int8_t x;
            int8_t y;
            Byte unknown_W;  //#TODO: Wheel?
            Byte unknown_T;  //#TODO: T?
        private:
            void assert_size() {
                static_assert(sizeof MouseReport == 5);
            }
        };

    private:
        [[deprecated]] static LONG compensate_win_acceleration(LONG x) {
            //#TODO

            static struct {
                int threshold[2];
                int acceleration;
            } mouse;
            static int speed = -1;  //1~20. #TODO: 0 when disabled?
            if (speed == -1) {  //#TODO: may change
                SystemParametersInfoW(SPI_GETMOUSE, 0, &mouse, 0);
                SystemParametersInfoW(SPI_GETMOUSESPEED, 0, &speed, 0);
                if constexpr (debug)
                    DebugOStream() << L"compensate_win_acceleration: "
                    << mouse.acceleration << L", " << mouse.threshold[0] << L"," << mouse.threshold[1]
                    << L", " << speed << std::endl;
                //1, 6,10, 10
            }

            /*
            if (mouse.acceleration) {
                if (x > mouse.threshold[0])
                    x *= 2;
                if (x > mouse.threshold[1] && mouse.acceleration == 2)
                    x *= 2;
            }
            return x * speed / 10;
            */

            return x;
        }

        static int8_t compensate_lgs_acceleration(int8_t x) {
            int8_t abs_x = abs(x);
            int8_t sign = x > 0 ? 1 : -1;

            if (abs_x <= 5)
                return x;
            else if (abs_x <= 10)
                return sign * (abs_x + 1);
            else
                return sign * (int8_t)round(0.6156218196 * abs_x + 4.45777444629);
        }

    public:
        bool report_mouse(MouseReport report, int8_t compensate_switch) const {
            constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
            DWORD bytes_returned;

            if (has_acceleration && (report.x || report.y)) {
                MouseReport report11 = report;
                report11.x = report11.y = compensate_switch;
                DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report11, sizeof MouseReport, nullptr, 0, &bytes_returned, nullptr);

                report.x = compensate_lgs_acceleration(report.x);
                report.y = compensate_lgs_acceleration(report.y);
            }

            if constexpr (debug) {
                bool success = DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof MouseReport, nullptr, 0, &bytes_returned, nullptr);
                DWORD error = GetLastError();
                DebugOStream() << L"report_mouse: " << report.button_byte << L", " << report.x << L", " << report.y << L". "
                    << success << L", " << error << std::endl;
                return success;
            }
            return DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof MouseReport, nullptr, 0, &bytes_returned, nullptr);
        }

        struct KeyboardReport {
            union {
                KeyboardModifiers modifiers;
                Byte modifiers_byte;
            };
            Byte reserved;
            Byte keys[6];
        private:
            void assert_size() {
                static_assert(sizeof KeyboardReport == 8);
            }
        };

        bool report_keyboard(KeyboardReport report) const {
            DWORD bytes_returned;
            if constexpr (debug) {
                bool success = DeviceIoControl(device, 0x2A200C, &report, sizeof KeyboardReport, nullptr, 0, &bytes_returned, nullptr);
                DWORD error = GetLastError();
                DebugOStream() << L"report_keyboard: " << report.modifiers_byte << L", " << report.keys[0] << L", " << report.keys[1] << L". "
                    << success << L", " << error << std::endl;
                return success;
            }
            return DeviceIoControl(device, 0x2A200C, &report, sizeof KeyboardReport, nullptr, 0, &bytes_returned, nullptr);
        }
    };

    class Logitech final : virtual public Base, public VirtualKeyStates {
        LogitechDriver driver;
    public:
        Logitech() : VirtualKeyStates(keyboard_report.modifiers, keyboard_mutex) {}

        Error create() {
            return driver.create();
        }

        void destroy() override {
            driver.destroy();
        }

    private:
        LogitechDriver::MouseReport mouse_report{};
        uint8_t compensate_switch = -1;
        std::mutex mouse_mutex;

    public:
        uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) override {
            update_screen_resolution();
            return Base::send_mouse_input(inputs, n);
        }

        bool send_mouse_input(const MOUSEINPUT& mi) override {
            std::lock_guard lock(mouse_mutex);
            
            if constexpr (debug)
                DebugOStream() << L"send_mouse_input: " << mi.dwFlags << L", " << mi.dx << L", " << mi.dy << std::endl;

            //#TODO: move and then click, or click and then move? former?

            //#TODO: MOUSEEVENTF_MOVE_NOCOALESCE, MOUSEEVENTF_VIRTUALDESK
            if (mi.dwFlags & MOUSEEVENTF_MOVE) {
                POINT move { mi.dx, mi.dy };
                if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
                    mouse_absolute_to_screen(move);
                    mouse_screen_to_relative(move);
                }

                while (abs(move.x) > 127 || abs(move.y) > 127) {
                    if (abs(move.x) > 127) {
                        mouse_report.x = move.x > 0 ? 127 : -127;
                        move.x -= mouse_report.x;
                    }
                    else {
                        mouse_report.x = 0;
                    }

                    if (abs(move.y) > 127) {
                        mouse_report.y = move.y > 0 ? 127 : -127;
                        move.y -= mouse_report.y;
                    }
                    else {
                        mouse_report.y = 0;
                    }

                    driver.report_mouse(mouse_report, compensate_switch = -compensate_switch);
                }

                mouse_report.x = (uint8_t)move.x;
                mouse_report.y = (uint8_t)move.y;
            } else {
                mouse_report.x = 0;
                mouse_report.y = 0;
            }

#define CODE_GENERATE(down, up, member)  \
            if (mi.dwFlags & down || mi.dwFlags & up)  \
                mouse_report.button.##member = mi.dwFlags & down;

            CODE_GENERATE(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, LButton)  //#TODO: may be switched?
            CODE_GENERATE(MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, RButton)
            CODE_GENERATE(MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, MButton)
#undef CODE_GENERATE
            if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
                bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
                switch (mi.mouseData) {
                case XBUTTON1: mouse_report.button.XButton1 = down; break;
                case XBUTTON2: mouse_report.button.XButton2 = down; break;
                }
            }
        
            return driver.report_mouse(mouse_report, compensate_switch = -compensate_switch);
        }

    private:
        LogitechDriver::KeyboardReport keyboard_report{};
        std::mutex keyboard_mutex;
    public:
        bool send_keyboard_input(const KEYBDINPUT& ki) override {
            std::lock_guard lock(keyboard_mutex);

            bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);
            if (is_modifier(ki.wVk)) {
                set_modifier_state(ki.wVk, keydown);
            }
            else {
                uint8_t usage = Usb::keyboard_vk_to_usage((uint8_t)ki.wVk);;
                if (keydown) {
                    for (int i = 0; i < 6; i++) {
                        if (keyboard_report.keys[i] == 0) {
                            keyboard_report.keys[i] = usage;
                            break;
                        }
                    }
                    //full
                }
                else {
                    for (int i = 0; i < 6; i++) {
                        if (keyboard_report.keys[i] == usage) {
                            keyboard_report.keys[i] = 0;
                            //#TODO: move to left?
                            break;
                        }
                    }
                }
            }

            return driver.report_keyboard(keyboard_report);
        }
#pragma warning(suppress : 4250)  //'class1' : inherits 'class2::member' via dominance
    };
}