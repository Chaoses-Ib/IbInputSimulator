#pragma once
#include <Windows.h>

extern "C" __declspec(dllimport) int __stdcall IbAhkSendInit();
extern "C" __declspec(dllimport) void __stdcall IbAhkSendDestroy();
extern "C" __declspec(dllimport) bool __stdcall IbAhkSendInputHookBegin();
extern "C" __declspec(dllimport) void __stdcall IbAhkSendInputHookEnd();
extern "C" __declspec(dllimport) UINT WINAPI IbAhkSendInput(UINT cInputs, LPINPUT pInputs, int cbSize);