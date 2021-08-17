#pragma once
#include "../IbAhkSend.hpp"

namespace Send::Type {
    class Base {
    protected:
        decltype(&::GetAsyncKeyState) *get_key_state_fallback;
    public:
        void base_create(decltype(&::GetAsyncKeyState)* get_key_state_fallback) {
            this->get_key_state_fallback = get_key_state_fallback;
        }

        virtual Error create() {
            return Error::Success;
        }

        virtual void destroy() {}

        virtual uint32_t send_input(INPUT inputs[], uint32_t n) {
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

        virtual uint32_t send_mouse_input(INPUT inputs[], uint32_t n) = 0;

        virtual SHORT get_key_state(int vKey) {
            return (*get_key_state_fallback)(vKey);
        }

        virtual void sync_key_states() {}

        virtual uint32_t send_keyboard_input(INPUT inputs[], uint32_t n) = 0;
    };
}