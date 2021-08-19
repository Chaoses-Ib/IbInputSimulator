#pragma once
#include "../common.hpp"
#include <mutex>
#include <IbWinCppLib/WinCppLib.hpp>

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

        POINT screen;
        void mouse_absolute_to_screen(POINT& absolute) const {
            // absolute.x = round(x / screen.x * 65536)
            absolute.x = absolute.x * screen.x / 65536;
            absolute.y = absolute.y * screen.y / 65536;
        }

        static void mouse_screen_to_relative(POINT& screen_point) {
            POINT point;
            GetCursorPos(&point);
            if constexpr (debug)
                ib::DebugOStream() << L"mouse_screen_to_relative: cursor (" << point.x << L", " << point.y << L") to point (" << screen_point.x << L", " << screen_point.y << L")\n";
            screen_point.x -= point.x;
            screen_point.y -= point.y;
        }
    public:
        void create_base(decltype(&::GetAsyncKeyState)* get_key_state_fallback) {
            this->get_key_state_fallback = get_key_state_fallback;

            screen.x = GetSystemMetrics(SM_CXSCREEN);  //#TODO: may change
            screen.y = GetSystemMetrics(SM_CYSCREEN);
        }

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
            }

            return count;
        }

        virtual uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) = 0;
        virtual uint32_t send_keyboard_input(const INPUT inputs[], uint32_t n) = 0;
        
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

    class VirtualKeyStates : virtual public Base {
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
}