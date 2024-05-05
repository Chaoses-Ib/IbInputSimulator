#pragma once
#include "InputSimulator.hpp"
using namespace Send;

#include <detours/detours.h>
#include <IbWinCpp/WinCpp.hpp>
using ib::Byte;

constexpr int debug = ib::debug_runtime;

ib::DebugOStream<> DebugOStream() {
    return { L"IbInputSimulator: " };
}

template<typename T>
LONG IbDetourAttach(_Inout_ T* ppPointer, _In_ T pDetour) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)ppPointer, pDetour);
    return DetourTransactionCommit();
}

template<typename T>
LONG IbDetourDetach(_Inout_ T* ppPointer, _In_ T pDetour) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((void**)ppPointer, pDetour);
    return DetourTransactionCommit();
}