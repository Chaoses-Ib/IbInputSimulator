#pragma once
#include "SendType.hpp"

namespace Send::Type {
    class SendInput final : public Base {
        decltype(&::SendInput) *SendInput_true;
    public:
        SendInput(decltype(&::SendInput)* SendInput_true) : SendInput_true(SendInput_true) {}

        uint32_t send_input(INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, inputs, sizeof INPUT);
        }

        uint32_t send_mouse_input(INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, inputs, sizeof INPUT);
        }

        uint32_t send_keyboard_input(INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, inputs, sizeof INPUT);
        }
    };
}