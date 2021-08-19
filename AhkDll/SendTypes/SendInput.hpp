#pragma once
#include "Base.hpp"

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
            return (*SendInput_true)(n, (INPUT*)inputs, sizeof INPUT);
        }

        uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, (INPUT*)inputs, sizeof INPUT);
        }

        uint32_t send_keyboard_input(const INPUT inputs[], uint32_t n) override {
            return (*SendInput_true)(n, (INPUT*)inputs, sizeof INPUT);
        }
    };
}