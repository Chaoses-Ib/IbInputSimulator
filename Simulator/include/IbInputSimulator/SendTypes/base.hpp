#pragma once
#include "../common.hpp"
#include <mutex>
#include <IbWinCpp/WinCpp.hpp>

#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

extern "C" {
    constexpr NTSTATUS STATUS_SUCCESS = 0x00000000;
    constexpr NTSTATUS STATUS_MORE_ENTRIES = 0x00000105;
    constexpr NTSTATUS STATUS_BUFFER_TOO_SMALL = 0xC0000023;
    constexpr ACCESS_MASK DIRECTORY_QUERY = 0x0001;

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

namespace Send::Type::Internal {
    class Base {
    protected:
        decltype(&::GetAsyncKeyState)* get_key_state_fallback;

        static bool is_modifier(int vKey) {
            int mods[] = { VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN };
            for (int mod : mods)
                if (mod == vKey)
                    return true;
            return false;
        }

        void mouse_absolute_to_screen(POINT& absolute) const {
            const static int mainScreenWidth = GetSystemMetrics(SM_CXSCREEN);
            const static int mainScreenHeight = GetSystemMetrics(SM_CYSCREEN);

            // the overhead of WM_DISPLAYCHANGE is a bit high

            absolute.x = MulDiv(absolute.x, mainScreenWidth, 65536);
            absolute.y = MulDiv(absolute.y, mainScreenHeight, 65536);
        }

        void mouse_virtual_desk_absolute_to_screen(POINT& absolute) const {
            const static int virtualDeskWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            const static int virtualDeskHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

            absolute.x = MulDiv(absolute.x, virtualDeskWidth, 65536); 
            absolute.y = MulDiv(absolute.y, virtualDeskHeight, 65536);
        }

        // need to call update_screen_resolution first
        static void mouse_screen_to_relative(POINT& screen_point) {
            POINT point;
            GetCursorPos(&point);
            if constexpr (debug)
                DebugOStream() << L"mouse_screen_to_relative: cursor (" << point.x << L", " << point.y << L") to point (" << screen_point.x << L", " << screen_point.y << L")\n";
            screen_point.x -= point.x;
            screen_point.y -= point.y;
        }

    public:
        void create_base(decltype(&::GetAsyncKeyState)* get_key_state_fallback) {
            this->get_key_state_fallback = get_key_state_fallback;
        }

        virtual ~Base() = default;
        virtual void destroy() = 0;

        virtual uint32_t send_input(const INPUT inputs[], uint32_t n) {
            uint32_t count = 0;

            for (uint32_t i = 0; i < n; i++) {
                DWORD type = inputs[i].type;

                uint32_t j = i + 1;
                while (j < n && inputs[j].type == type)
                    j++;

                switch (type) {
                case INPUT_KEYBOARD:
                    count += send_keyboard_input(inputs + i, j - i);
                    break;
                case INPUT_MOUSE:
                    count += send_mouse_input(inputs + i, j - i);
                    break;
                }

                i = j;
            }

            return count;
        }

        virtual uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) {
            uint32_t count = 0;
            for (uint32_t i = 0; i < n; i++)
                count += send_mouse_input(inputs[i].mi);
            return count;
        }

        virtual bool send_mouse_input(const MOUSEINPUT& mi) = 0;

        virtual uint32_t send_keyboard_input(const INPUT inputs[], uint32_t n) {
            uint32_t count = 0;
            for (uint32_t i = 0; i < n; i++)
                count += send_keyboard_input(inputs[i].ki);
            return count;
        }
        
        virtual bool send_keyboard_input(const KEYBDINPUT& ki) = 0;
        
        virtual SHORT get_key_state(int vKey) {
            return (*get_key_state_fallback)(vKey);
        }

        virtual void sync_key_states() {}
    };

    struct KeyboardModifiers {
        bool LCtrl : 1;
        bool LShift : 1;
        bool LAlt : 1;
        bool LGui : 1;
        bool RCtrl : 1;
        bool RShift : 1;
        bool RAlt : 1;
        bool RGui : 1;
    };

    class VirtualKeyStates : public Base {
        KeyboardModifiers& modifiers;
        std::mutex& mutex;

    protected:
        VirtualKeyStates(KeyboardModifiers& modifiers, std::mutex& mutex) : modifiers(modifiers), mutex(mutex) {}

        void set_modifier_state(int vKey, bool keydown) {
            switch (vKey) {
#define CODE_GENERATE(vk, member)  \
                case vk:  \
                    modifiers.##member = keydown;  \
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
            }
        }

    public:
        SHORT get_key_state(int vKey) override {
            switch (vKey) {
#define CODE_GENERATE(vk, member)  case vk: return modifiers.##member << 15;

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
                return (*get_key_state_fallback)(vKey);
            }
        }

        void sync_key_states() override {
            std::lock_guard lock(mutex);

            //#TODO: GetKeyboardState() ?
            //static bool states[256];  //down := true
#define CODE_GENERATE(vk, member)  modifiers.##member = (*get_key_state_fallback)(vk) & 0x8000;

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
    };

    inline std::wstring find_device(std::function<bool(std::wstring_view name)> p) {
        std::wstring result{};
        HANDLE dir_handle;

        OBJECT_ATTRIBUTES obj_attr;
        UNICODE_STRING obj_name;  //or RTL_CONSTANT_STRING
        RtlInitUnicodeString(&obj_name, LR"(\GLOBAL??)");
        InitializeObjectAttributes(&obj_attr, &obj_name, 0, NULL, NULL);

        if (NT_SUCCESS(NtOpenDirectoryObject(&dir_handle, DIRECTORY_QUERY, &obj_attr))) {  //or DIRECTORY_TRAVERSE?
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
                    std::wstring_view sv{ info[i].Name.Buffer, info[i].Name.Length / sizeof(wchar_t) };
                    if (p(sv)) {
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
}