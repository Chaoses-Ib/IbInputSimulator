#pragma once
#include "base.hpp"
#include "SendInput.hpp"
#include "Logitech.hpp"
#include "Razer.hpp"
#include "DD.hpp"
#include "MouClassInputInjection.hpp"

namespace Send
{
    namespace Type
    {
        using Internal::Base;
        using Internal::SendInput;
        using Internal::Logitech;
        using Internal::LogitechGHubNew;
        using Internal::Razer;
        using Internal::DD;
        using Internal::MouClassInputInjection;
    }
}