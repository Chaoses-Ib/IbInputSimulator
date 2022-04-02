#pragma once
#include "base.hpp"

namespace Send::Type::Internal {
    class SendInput final : virtual public Base {
        decltype(&::SendInput) *SendInput_true;
    public:
        Error create(decltype(&::SendInput)* SendInput_true) {
            this->SendInput_true = SendInput_true;
            return Error::Success;
        }

        void destroy() override {}

        uint32_t send_input(const INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, const_cast<INPUT*>(inputs), sizeof INPUT);
        }


        uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, const_cast<INPUT*>(inputs), sizeof INPUT);
        }

        bool send_mouse_input(const MOUSEINPUT& mi) override {
            INPUT input;
            input.type = INPUT_MOUSE;
            input.mi = mi;
            return (*SendInput_true)(1, &input, sizeof INPUT);
        }


        uint32_t send_keyboard_input(const INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, const_cast<INPUT*>(inputs), sizeof INPUT);
        }

        bool send_keyboard_input(const KEYBDINPUT& ki) override {
            INPUT input;
            input.type = INPUT_KEYBOARD;
            input.ki = ki;
            return (*SendInput_true)(1, &input, sizeof INPUT);
        }
    };
}