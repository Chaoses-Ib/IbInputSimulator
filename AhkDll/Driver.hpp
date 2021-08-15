#pragma once
#include <string>
#include <Windows.h>
#include <winternl.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "common.hpp"

#pragma comment(lib, "ntdll.lib")

extern "C" {
    constexpr NTSTATUS STATUS_SUCCESS = 0x00000000;
    constexpr NTSTATUS STATUS_MORE_ENTRIES = 0x00000105;
    constexpr NTSTATUS STATUS_BUFFER_TOO_SMALL = 0xC0000023;
    constexpr ACCESS_MASK DIRECTORY_QUERY = 0x0001;  //or DIRECTORY_TRAVERSE?

    NTSTATUS WINAPI NtOpenDirectoryObject(
        _Out_ PHANDLE            DirectoryHandle,
        _In_  ACCESS_MASK        DesiredAccess,
        _In_  POBJECT_ATTRIBUTES ObjectAttributes
    );

    typedef struct _OBJECT_DIRECTORY_INFORMATION {
        UNICODE_STRING Name;
        UNICODE_STRING TypeName;
    } OBJECT_DIRECTORY_INFORMATION, * POBJECT_DIRECTORY_INFORMATION;

    NTSTATUS WINAPI NtQueryDirectoryObject(
        _In_      HANDLE  DirectoryHandle,
        _Out_opt_ PVOID   Buffer,
        _In_      ULONG   Length,
        _In_      BOOLEAN ReturnSingleEntry,
        _In_      BOOLEAN RestartScan,
        _Inout_   PULONG  Context,
        _Out_opt_ PULONG  ReturnLength
    );
}

class LogiDriver {
    HANDLE device;
    bool has_acceleration;

public:
    enum class Error {
        Success,
        DeviceNotFound,
        DeviceOpeningFailed,
        LogiSettingsNotFound
    };

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
            return Error::DeviceOpeningFailed;

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
        using namespace std::literals;

        std::wstring result{};
        HANDLE dir_handle;

        OBJECT_ATTRIBUTES obj_attr;
        UNICODE_STRING obj_name;  //or RTL_CONSTANT_STRING
        RtlInitUnicodeString(&obj_name, LR"(\GLOBAL??)");
        InitializeObjectAttributes(&obj_attr, &obj_name, 0, NULL, NULL);

        if (NT_SUCCESS(NtOpenDirectoryObject(&dir_handle, DIRECTORY_QUERY, &obj_attr))) {
            union {
                ib::Byte buf[2048];  //#TODO
                OBJECT_DIRECTORY_INFORMATION info[1];
            };
            ULONG context;

#pragma warning(suppress : 6001)  //Warning C6001: Using uninitialized memory 'context'.
            NTSTATUS status = NtQueryDirectoryObject(dir_handle, buf, sizeof buf, false, true, &context, NULL);
            while (NT_SUCCESS(status)) {  //STATUS_SUCCESS, STATUS_MORE_ENTRIES
                bool found = false;
                for (ULONG i = 0; info[i].Name.Buffer; i++) {
                    //LGS:
                    //ROOT#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
                    //ROOT#SYSTEM#0001#{5bada891-842b-4296-a496-68ae931aa16c}
                    //Root#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
                    //Root#SYSTEM#0001#{5bada891-842b-4296-a496-68ae931aa16c}

                    //G HUB:
                    //ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}
                    //ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015} ?
                    //ROOT#SYSTEM#0001#{dfbedcdb-2148-416d-9e4d-cecc2424128c}

                    std::wstring_view sv{ info[i].Name.Buffer, info[i].Name.Length / sizeof(wchar_t) };
                    if ((
                        sv.starts_with(L"ROOT#SYSTEM#"sv)
                        || sv.starts_with(L"Root#SYSTEM#"sv)
                        ) && (
                        sv.ends_with(L"#{1abc05c0-c378-41b9-9cef-df1aba82b015}"sv)
                        || sv.ends_with(L"#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}"sv)
                        || sv.ends_with(L"#{dfbedcdb-2148-416d-9e4d-cecc2424128c}"sv)
                        || sv.ends_with(L"#{5bada891-842b-4296-a496-68ae931aa16c}"sv)
                        )
                    ){
                        result = LR"(\??\)" + std::wstring(sv);
                        found = true;
                        break;
                    }
                }
                if (found || status != STATUS_MORE_ENTRIES)
                    break;
                status = NtQueryDirectoryObject(dir_handle, buf, sizeof buf, false, false, &context, NULL);
            }

            CloseHandle(dir_handle);
        }

        return result;
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
    void report_mouse(MouseReport report) const {
        constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
        DWORD bytes_returned;

        if (has_acceleration && (report.x || report.y)) {
            MouseReport report11 = report;
            report.x = report.y = 1;
            DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report11, sizeof MouseReport, nullptr, 0, &bytes_returned, nullptr);
            /*
            report11.x = -report11.x;
            report11.y = -report11.y;
            */

            report.x = compensate_lgs_acceleration(report.x);
            report.y = compensate_lgs_acceleration(report.y);
        }
        
        if constexpr (debug) {
            bool success = DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof MouseReport, nullptr, 0, &bytes_returned, nullptr);
            DWORD error = GetLastError();
            DebugOStream() << L"report_mouse: " << report.button_byte << L", " << report.x << L", " << report.y << L". "
                << success << L", " << error << std::endl;
            return;
        }
        DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof MouseReport, nullptr, 0, &bytes_returned, nullptr);
    }

    struct KeyboardModifier {
        bool LCtrl : 1;
        bool LShift : 1;
        bool LAlt : 1;
        bool LGui : 1;
        bool RCtrl : 1;
        bool RShift : 1;
        bool RAlt : 1;
        bool RGui : 1;
    };

    struct KeyboardReport {
        union {
            KeyboardModifier modifier;
            Byte modifier_byte;
        };
        Byte reserved;
        Byte keys[6];
    private:
        void assert_size() {
            static_assert(sizeof KeyboardReport == 8);
        }
    };

    void report_keyboard(KeyboardReport report) const {
        DWORD bytes_returned;
        if constexpr (debug) {
            bool success = DeviceIoControl(device, 0x2A200C, &report, sizeof KeyboardReport, nullptr, 0, &bytes_returned, nullptr);
            DWORD error = GetLastError();
            DebugOStream() << L"report_keyboard: " << report.modifier_byte << L", " << report.keys[0] << L", " << report.keys[1] << L". "
                << success << L", " << error << std::endl;
            return;
        }
        DeviceIoControl(device, 0x2A200C, &report, sizeof KeyboardReport, nullptr, 0, &bytes_returned, nullptr);
    }
    
    static constexpr uint8_t keyboard_vk_to_usage(uint8_t vkCode) {
        //#TODO: fully test

        switch (vkCode) {
        case 0x00: return 0x0000;
        case 0x01: return 0x0000;  //VK_LBUTTON
        case 0x02: return 0x0000;  //VK_RBUTTON
        case 0x03: return 0x9B;  //VK_CANCEL
        case 0x04: return 0x0000;  //VK_MBUTTON
        case 0x05: return 0x0000;  //VK_XBUTTON1
        case 0x06: return 0x0000;  //VK_XBUTTON2
        case 0x07: return 0x00;  //reserved
        case 0x08: return 0x2A;  //VK_BACK
        case 0x09: return 0x2B;  //VK_TAB
        case 0x0A: return 0x00;  //reserved
        case 0x0B: return 0x00;  //reserved
        case 0x0C: return 0x0000;  //VK_CLEAR
        case 0x0D: return 0x28;  //VK_RETURN
        case 0x0E: return 0x00;  //reserved
        case 0x0F: return 0x00;  //reserved
        case 0x10: return 0x0000;  //VK_SHIFT
        case 0x11: return 0x0000;  //VK_CONTROL
        case 0x12: return 0x0000;  //VK_MENU
        case 0x13: return 0x48;  //VK_PAUSE
        case 0x14: return 0x39;  //VK_CAPITAL
        case 0x15: return 0x0000;  //VK_KANA|VK_HANGEUL|VK_HANGUL
        case 0x16: return 0x00;  //unassigned
        case 0x17: return 0x0000;  //VK_JUNJA
        case 0x18: return 0x0000;  //VK_FINAL
        case 0x19: return 0x0000;  //VK_HANJA|VK_KANJI
        case 0x1A: return 0x00;  //unassigned
        case 0x1B: return 0x29;  //VK_ESCAPE
        case 0x1C: return 0x0000;  //VK_CONVERT
        case 0x1D: return 0x0000;  //VK_NONCONVERT
        case 0x1E: return 0x0000;  //VK_ACCEPT
        case 0x1F: return 0x0000;  //VK_MODECHANGE
        case 0x20: return 0x2C;  //VK_SPACE
        case 0x21: return 0x4B;  //VK_PRIOR
        case 0x22: return 0x4E;  //VK_NEXT
        case 0x23: return 0x4D;  //VK_END
        case 0x24: return 0x4A;  //VK_HOME
        case 0x25: return 0x50;  //VK_LEFT
        case 0x26: return 0x52;  //VK_UP
        case 0x27: return 0x4F;  //VK_RIGHT
        case 0x28: return 0x51;  //VK_DOWN
        case 0x29: return 0x0000;  //VK_SELECT
        case 0x2A: return 0x0000;  //VK_PRINT
        case 0x2B: return 0x0000;  //VK_EXECUTE
        case 0x2C: return 0x46;  //VK_SNAPSHOT
        case 0x2D: return 0x49;  //VK_INSERT
        case 0x2E: return 0x4C;  //VK_DELETE
        case 0x2F: return 0x0000;  //VK_HELP
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40: return 0x00;  //unassigned
        case 0x5B: return 0xE3;  //VK_LWIN
        case 0x5C: return 0xE7;  //VK_RWIN
        case 0x5D: return 0x65;  //VK_APPS
        case 0x5E: return 0x00;  //reserved
        case 0x5F: return 0x0000;  //VK_SLEEP

        case 0x6A: return 0x55;  //VK_MULTIPLY
        case 0x6B: return 0x57;  //VK_ADD
        case 0x6C: return 0x0000;  //VK_SEPARATOR
        case 0x6D: return 0x56;  //VK_SUBTRACT
        case 0x6E: return 0x63;  //VK_DECIMAL
        case 0x6F: return 0x54;  //VK_DIVIDE

        case 0x88: return 0x0000;  //VK_NAVIGATION_VIEW
        case 0x89: return 0x0000;  //VK_NAVIGATION_MENU
        case 0x8A: return 0x0000;  //VK_NAVIGATION_UP
        case 0x8B: return 0x0000;  //VK_NAVIGATION_DOWN
        case 0x8C: return 0x0000;  //VK_NAVIGATION_LEFT
        case 0x8D: return 0x0000;  //VK_NAVIGATION_RIGHT
        case 0x8E: return 0x0000;  //VK_NAVIGATION_ACCEPT
        case 0x8F: return 0x0000;  //VK_NAVIGATION_CANCEL
        case 0x90: return 0x53;  //VK_NUMLOCK
        case 0x91: return 0x47;  //VK_SCROLL
        case 0x92: return 0x0000;  //VK_OEM_NEC_EQUAL|VK_OEM_FJ_JISHO
        case 0x93: return 0x0000;  //VK_OEM_FJ_MASSHOU
        case 0x94: return 0x0000;  //VK_OEM_FJ_TOUROKU
        case 0x95: return 0x0000;  //VK_OEM_FJ_LOYA
        case 0x96: return 0x0000;  //VK_OEM_FJ_ROYA
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F: return 0x00;  //unassigned
        case 0xA0: return 0xE1;  //VK_LSHIFT
        case 0xA1: return 0xE5;  //VK_RSHIFT
        case 0xA2: return 0xE0;  //VK_LCONTROL
        case 0xA3: return 0xE4;  //VK_RCONTROL
        case 0xA4: return 0xE2;  //VK_LMENU
        case 0xA5: return 0xE6;  //VK_RMENU
        case 0xA6: return 0x0000;  //VK_BROWSER_BACK
        case 0xA7: return 0x0000;  //VK_BROWSER_FORWARD
        case 0xA8: return 0x0000;  //VK_BROWSER_REFRESH
        case 0xA9: return 0x0000;  //VK_BROWSER_STOP
        case 0xAA: return 0x0000;  //VK_BROWSER_SEARCH
        case 0xAB: return 0x0000;  //VK_BROWSER_FAVORITES
        case 0xAC: return 0x0000;  //VK_BROWSER_HOME
        case 0xAD: return 0x0000;  //VK_VOLUME_MUTE
        case 0xAE: return 0x0000;  //VK_VOLUME_DOWN
        case 0xAF: return 0x0000;  //VK_VOLUME_UP
        case 0xB0: return 0x0000;  //VK_MEDIA_NEXT_TRACK
        case 0xB1: return 0x0000;  //VK_MEDIA_PREV_TRACK
        case 0xB2: return 0x0000;  //VK_MEDIA_STOP
        case 0xB3: return 0x0000;  //VK_MEDIA_PLAY_PAUSE
        case 0xB4: return 0x0000;  //VK_LAUNCH_MAIL
        case 0xB5: return 0x0000;  //VK_LAUNCH_MEDIA_SELECT
        case 0xB6: return 0x0000;  //VK_LAUNCH_APP1
        case 0xB7: return 0x0000;  //VK_LAUNCH_APP2
        case 0xB8:
        case 0xB9: return 0x00;  //reserved
        case 0xBA: return 0x33;  //VK_OEM_1   // ';:' for US
        case 0xBB: return 0x2E;  //VK_OEM_PLUS   // '+' any country
        case 0xBC: return 0x36;  //VK_OEM_COMMA   // ',' any country
        case 0xBD: return 0x2D;  //VK_OEM_MINUS   // '-' any country
        case 0xBE: return 0x37;  //VK_OEM_PERIOD   // '.' any country
        case 0xBF: return 0x38;  //VK_OEM_2   // '/?' for US
        case 0xC0: return 0x35;  //VK_OEM_3   // '`~' for US
        case 0xC1: return 0x00;  //reserved
        case 0xC2: return 0x00;  //reserved
        case 0xC3: return 0x00;  //VK_GAMEPAD_A // reserved
        case 0xC4: return 0x00;  //VK_GAMEPAD_B // reserved
        case 0xC5: return 0x00;  //VK_GAMEPAD_X // reserved
        case 0xC6: return 0x00;  //VK_GAMEPAD_Y // reserved
        case 0xC7: return 0x00;  //VK_GAMEPAD_RIGHT_SHOULDER // reserved
        case 0xC8: return 0x00;  //VK_GAMEPAD_LEFT_SHOULDER // reserved
        case 0xC9: return 0x00;  //VK_GAMEPAD_LEFT_TRIGGER // reserved
        case 0xCA: return 0x00;  //VK_GAMEPAD_RIGHT_TRIGGER // reserved
        case 0xCB: return 0x00;  //VK_GAMEPAD_DPAD_UP // reserved
        case 0xCC: return 0x00;  //VK_GAMEPAD_DPAD_DOWN // reserved
        case 0xCD: return 0x00;  //VK_GAMEPAD_DPAD_LEFT // reserved
        case 0xCE: return 0x00;  //VK_GAMEPAD_DPAD_RIGHT // reserved
        case 0xCF: return 0x00;  //VK_GAMEPAD_MENU // reserved
        case 0xD0: return 0x00;  //VK_GAMEPAD_VIEW // reserved
        case 0xD1: return 0x00;  //VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON // reserved
        case 0xD2: return 0x00;  //VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON // reserved
        case 0xD3: return 0x00;  //VK_GAMEPAD_LEFT_THUMBSTICK_UP // reserved
        case 0xD4: return 0x00;  //VK_GAMEPAD_LEFT_THUMBSTICK_DOWN // reserved
        case 0xD5: return 0x00;  //VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT // reserved
        case 0xD6: return 0x00;  //VK_GAMEPAD_LEFT_THUMBSTICK_LEFT // reserved
        case 0xD7: return 0x00;  //VK_GAMEPAD_RIGHT_THUMBSTICK_UP // reserved
        case 0xD8: return 0x00;  //VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN // reserved
        case 0xD9: return 0x00;  //VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT // reserved
        case 0xDA: return 0x00;  //VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT // reserved
        case 0xDB: return 0x2F;  //VK_OEM_4  //  '[{' for US
        case 0xDC: return 0x31;  //VK_OEM_5  //  '\|' for US
        case 0xDD: return 0x30;  //VK_OEM_6  //  ']}' for US
        case 0xDE: return 0x34;  //VK_OEM_7  //  ''"' for US
        case 0xDF: return 0x0000;  //VK_OEM_8
        case 0xE0: return 0x0000;  //reserved
        case 0xE1: return 0x0000;  //VK_OEM_AX  //  'AX' key on Japanese AX kbd
        case 0xE2: return 0x64;  //VK_OEM_102  //  "<>" or "\|" on RT 102-key kbd.
        case 0xE3: return 0x0000;  //VK_ICO_HELP  //  Help key on ICO
        case 0xE4: return 0x0000;  //VK_ICO_00  //  00 key on ICO
        case 0xE5: return 0x0000;  //VK_PROCESSKEY
        case 0xE6: return 0x0000;  //VK_ICO_CLEAR
        case 0xE7: return 0x0000;  //VK_PACKET
        case 0xE8: return 0x0000;  //unassigned
        case 0xE9: return 0x0000;  //VK_OEM_RESET
        case 0xEA: return 0x0000;  //VK_OEM_JUMP
        case 0xEB: return 0x0000;  //VK_OEM_PA1
        case 0xEC: return 0x0000;  //VK_OEM_PA2
        case 0xED: return 0x0000;  //VK_OEM_PA3
        case 0xEE: return 0x0000;  //VK_OEM_WSCTRL
        case 0xEF: return 0x0000;  //VK_OEM_CUSEL
        case 0xF0: return 0x0000;  //VK_OEM_ATTN
        case 0xF1: return 0x0000;  //VK_OEM_FINISH
        case 0xF2: return 0x0000;  //VK_OEM_COPY
        case 0xF3: return 0x0000;  //VK_OEM_AUTO
        case 0xF4: return 0x0000;  //VK_OEM_ENLW
        case 0xF5: return 0x0000;  //VK_OEM_BACKTAB
        case 0xF6: return 0x0000;  //VK_ATTN
        case 0xF7: return 0x0000;  //VK_CRSEL
        case 0xF8: return 0x0000;  //VK_EXSEL
        case 0xF9: return 0x0000;  //VK_EREOF
        case 0xFA: return 0x0000;  //VK_PLAY
        case 0xFB: return 0x0000;  //VK_ZOOM
        case 0xFC: return 0x0000;  //VK_NONAME
        case 0xFD: return 0x0000;  //VK_PA1
        case 0xFE: return 0x0000;  //VK_OEM_CLEAR
        case 0xFF: return 0x0000;
        default:
            if ('A' <= vkCode and vkCode <= 'Z')
                return 0x04 + vkCode - 'A';
            else if ('0' <= vkCode and vkCode <= '9')
                return vkCode == '0' ? 0x27 : 0x1E + vkCode - '1';
            else if (VK_NUMPAD0 <= vkCode and vkCode <= VK_NUMPAD9)
                return vkCode == VK_NUMPAD0 ? 0x62 : 0x59 + vkCode - VK_NUMPAD1;
            else if (VK_F1 <= vkCode and vkCode <= VK_F24)
                return vkCode <= VK_F12
                ? 0x3A + vkCode - VK_F1
                : 0x68 + vkCode - VK_F13;
        }
    }
};

class DriverSendInput {
    const LogiDriver& driver;

public:
    DriverSendInput(const LogiDriver& driver) : driver(driver) {}

    void send_input(INPUT inputs[], uint32_t n) {
        for (uint32_t i = 0; i < n; i++) {
            DWORD type = inputs[i].type;

            uint32_t j = i + 1;
            while (j < n && inputs[j].type == type)
                j++;

            switch (type) {
            case INPUT_KEYBOARD:
                send_keyboard_input(inputs + i, j - i);
                break;
            case INPUT_MOUSE:
                send_mouse_input(inputs + i, j - i);
                break;
            }
        }
    }

private:
    LogiDriver::MouseReport mouse_report{};

public:
    void send_mouse_input(INPUT inputs[], uint32_t n) {
        for (uint32_t i = 0; i < n; i++) {
            MOUSEINPUT& mi = inputs[i].mi;
            if constexpr (debug)
                DebugOStream() << L"send_mouse_input: " << mi.dwFlags << L", " << mi.dx << L", " << mi.dy << std::endl;

            //#TODO: move and then click, or click and then move? former?

            //#TODO: MOUSEEVENTF_MOVE_NOCOALESCE, MOUSEEVENTF_VIRTUALDESK
            if (mi.dwFlags & MOUSEEVENTF_MOVE || mi.dwFlags & MOUSEEVENTF_MOVE_NOCOALESCE) {
                if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
                    static POINT screen;  //#TODO: may change
                    if (!screen.x) {
                        screen.x = GetSystemMetrics(SM_CXSCREEN);
                        screen.y = GetSystemMetrics(SM_CYSCREEN);
                    }
                    // mi.dx = round(x / screen.x * 65536)
                    mi.dx = mi.dx * screen.x / 65536;
                    mi.dy = mi.dy * screen.y / 65536;

                    POINT point;
                    GetCursorPos(&point);
                    if constexpr (debug)
                        DebugOStream() << L"MOUSEEVENTF_ABSOLUTE: (" << point.x << L", " << point.y << L") -> (" << mi.dx << L", " << mi.dy << L")" << std::endl;
                    mi.dx -= point.x;
                    mi.dy -= point.y;
                }

                while (abs(mi.dx) > 127 || abs(mi.dy) > 127) {
                    if (abs(mi.dx) > 127) {
                        mouse_report.x = mi.dx > 0 ? 127 : -127;
                        mi.dx -= mouse_report.x;
                    }
                    else {
                        mouse_report.x = 0;
                    }

                    if (abs(mi.dy) > 127) {
                        mouse_report.y = mi.dy > 0 ? 127 : -127;
                        mi.dy -= mouse_report.y;
                    }
                    else {
                        mouse_report.y = 0;
                    }

                    driver.report_mouse(mouse_report);
                }

                mouse_report.x = (uint8_t)mi.dx;
                mouse_report.y = (uint8_t)mi.dy;
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
            if (mi.dwFlags & MOUSEEVENTF_XDOWN || mi.dwFlags & MOUSEEVENTF_XUP) {
                bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
                switch (mi.mouseData) {
                case XBUTTON1: mouse_report.button.XButton1 = down; break;
                case XBUTTON2: mouse_report.button.XButton2 = down; break;
                }
            }
            
            driver.report_mouse(mouse_report);
        }
    }

private:
    LogiDriver::KeyboardReport keyboard_report{};

public:
    SHORT get_key_state(int vKey, decltype(GetAsyncKeyState) fallback) {
        switch (vKey) {
#define CODE_GENERATE(vk, member)  case vk: return keyboard_report.modifier.##member << 15;

            CODE_GENERATE(VK_LCONTROL, LCtrl)
            CODE_GENERATE(VK_RCONTROL, RCtrl)
            CODE_GENERATE(VK_LSHIFT, LShift)
            CODE_GENERATE(VK_RSHIFT, RShift)
            CODE_GENERATE(VK_LMENU, LAlt)
            CODE_GENERATE(VK_RMENU, RAlt)
            CODE_GENERATE(VK_LWIN, LGui)
            CODE_GENERATE(VK_RWIN, RGui)
#undef CODE_GENERATE
        default:
            return fallback(vKey);
        }
    }

    void sync_key_states() {
        //#TODO: GetKeyboardState() ?
        //static bool states[256];  //down := true
#define CODE_GENERATE(vk, member)  keyboard_report.modifier.##member = GetAsyncKeyState(vk) & 0x8000;

        CODE_GENERATE(VK_LCONTROL, LCtrl)
        CODE_GENERATE(VK_RCONTROL, RCtrl)
        CODE_GENERATE(VK_LSHIFT, LShift)
        CODE_GENERATE(VK_RSHIFT, RShift)
        CODE_GENERATE(VK_LMENU, LAlt)
        CODE_GENERATE(VK_RMENU, RAlt)
        CODE_GENERATE(VK_LWIN, LGui)
        CODE_GENERATE(VK_RWIN, RGui)
#undef CODE_GENERATE
    }

    void send_keyboard_input(INPUT inputs[], uint32_t n) {
        for (uint32_t i = 0; i < n; i++) {
            bool keydown = !(inputs[i].ki.dwFlags & KEYEVENTF_KEYUP);
            switch (inputs[i].ki.wVk) {

#define CODE_GENERATE(vk, member)  \
            case vk:  \
                keyboard_report.modifier.##member = keydown;  \
                break;

            CODE_GENERATE(VK_LCONTROL, LCtrl)
            CODE_GENERATE(VK_RCONTROL, RCtrl)
            CODE_GENERATE(VK_LSHIFT, LShift)
            CODE_GENERATE(VK_RSHIFT, RShift)
            CODE_GENERATE(VK_LMENU, LAlt)
            CODE_GENERATE(VK_RMENU, RAlt)
            CODE_GENERATE(VK_LWIN, LGui)
            CODE_GENERATE(VK_RWIN, RGui)
#undef CODE_GENERATE

            default:
                uint8_t usage = driver.keyboard_vk_to_usage((uint8_t)inputs[i].ki.wVk);;
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

            driver.report_keyboard(keyboard_report);
        }
    }
};