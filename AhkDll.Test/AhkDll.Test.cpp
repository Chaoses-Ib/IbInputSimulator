#include "pch.h"
#include <thread>
#include <mutex>
#include <queue>
#include <fmt/core.h>
#include "CppUnitTest.h"

#include <IbWinCppLib/WinCppLib.hpp>
#include "../AhkDll/IbAhkSend.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class Measure {
	LARGE_INTEGER t;
	double frequency_ns;
public:
	Measure() {
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		frequency_ns = frequency.QuadPart / 1'000'000'000.0;
	}

	void begin() {
		QueryPerformanceCounter(&t);
	}

	uint64_t end() {
		LARGE_INTEGER t_;
		QueryPerformanceCounter(&t_);
		return uint64_t((t_.QuadPart - t.QuadPart) / frequency_ns);
	}
};

void send_init(Send::SendType type, Send::InitFlags flags = 0, void* argument = nullptr) {
	//cannot be put in TEST_MODULE_INITIALIZE for unknown reason
	Send::Error error = IbAhkSendInit(type, flags, argument);
	Assert::AreEqual((uint32_t)Send::Error::Success, (uint32_t)error);
}

class KeyboardTestBase
{
public:
	static inline HHOOK hook;
	static inline bool capture;
	static inline std::queue<DWORD> input_keys;
	static inline std::mutex mutex;

	KeyboardTestBase() {
		capture = false;
		std::thread t([] {
			hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0);
			Assert::AreNotEqual<void*>(NULL, hook);

			MSG messages;
			while (GetMessageW(&messages, NULL, 0, 0))
			{
				TranslateMessage(&messages);
				DispatchMessageW(&messages);
			}
			});
		t.detach();
	}

	~KeyboardTestBase() {
		IbAhkSendDestroy();
		UnhookWindowsHookEx(hook);
	}

	static LRESULT CALLBACK LowLevelKeyboardProc(
		_In_ int    nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
	) {
		if (capture && nCode == HC_ACTION) {
			KBDLLHOOKSTRUCT* p = ib::Addr(lParam);
			{
				std::lock_guard lock(mutex);
				input_keys.push(p->vkCode);
			}
		}
		return CallNextHookEx(hook, nCode, wParam, lParam);
	}

	void MeasureKeyboardLatency() {
		Measure measure;

		measure.begin();
		{
			INPUT input[2];

			input[0].type = INPUT_KEYBOARD;
			input[0].ki = {};
			input[0].ki.wVk = VK_F12;

			input[1].type = INPUT_KEYBOARD;
			input[1].ki = {};
			input[1].ki.wVk = VK_F12;
			input[1].ki.dwFlags = KEYEVENTF_KEYUP;

			capture = true;
			IbAhkSendInput(2, input, sizeof INPUT);
		}
		uint64_t t1 = measure.end();

		measure.begin();
		{
			while (input_keys.size() < 2)
				_mm_pause();
			capture = false;
		}
		uint64_t t2 = measure.end();  //0.8~4ms

		{
			std::lock_guard lock(mutex);
			Assert::AreEqual<DWORD>(VK_F12, input_keys.back());
			input_keys.pop();
			Assert::AreEqual<DWORD>(VK_F12, input_keys.back());
			input_keys.pop();
		}

		Logger::WriteMessage(fmt::format("Duration: {}ns\n", t1).c_str());
		Logger::WriteMessage(fmt::format("Latency: {}ns\n", t2).c_str());
	}
};

#define CODE_GENERATE_KEYBOARDTEST(type)  \
	TEST_CLASS(KeyboardTest), private KeyboardTestBase {  \
		using base = KeyboardTestBase;  \
	public:  \
		KeyboardTest() {  \
			send_init(Send::SendType::type);  \
		}  \
		  \
		TEST_METHOD(MeasureKeyboardLatency) {  \
			base::MeasureKeyboardLatency();  \
		}  \
	};

class MouseTestBase {
public:
	MouseTestBase() {}

	~MouseTestBase() {
		IbAhkSendDestroy();
	}

	void MeasureMouseMoveDuration() {
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi = {};
		input.mi.dwFlags = MOUSEEVENTF_MOVE_NOCOALESCE;
		for (size_t i = 0; i < 10000 / 200; i++) {
			input.mi.dx = input.mi.dy = 1;
			for (size_t i = 0; i < 100; i++)
				IbAhkSendInput(1, &input, sizeof INPUT);  //≈3us

			input.mi.dx = input.mi.dy = -1;
			for (size_t i = 0; i < 100; i++)
				IbAhkSendInput(1, &input, sizeof INPUT);
		}
	}

	void MeasureMouseMovement() {
		POINT p1, p2, d1, d2;
		GetCursorPos(&p1);

		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi = {};
		input.mi.dwFlags = MOUSEEVENTF_MOVE_NOCOALESCE;
		input.mi.dx = input.mi.dy = 100;
		IbAhkSendInput(1, &input, sizeof INPUT);

		GetCursorPos(&p2);
		d1 = { p2.x - p1.x, p2.y - p1.y };

		Sleep(10);

		GetCursorPos(&p2);
		d2 = { p2.x - p1.x, p2.y - p1.y };

		Logger::WriteMessage(fmt::format("0ms: ({}, {})\n", d1.x, d1.y).c_str());
		Logger::WriteMessage(fmt::format("10ms: ({}, {})\n", d2.x, d2.y).c_str());
	}
};

#define CODE_GENERATE_MOUSETEST(type)  \
	TEST_CLASS(MouseTest), private MouseTestBase {  \
		using base = MouseTestBase;  \
	public:  \
		MouseTest() {  \
			send_init(Send::SendType::type);  \
		}  \
		  \
		TEST_METHOD(MeasureMouseMoveDuration) {  \
			base::MeasureMouseMoveDuration();  \
		}  \
		  \
		TEST_METHOD(MeasureMouseMovement) {  \
			base::MeasureMouseMovement();  \
		}  \
	};

namespace AnyDriverTest
{
	CODE_GENERATE_KEYBOARDTEST(AnyDriver)
	CODE_GENERATE_MOUSETEST(AnyDriver)
}

namespace LogitechTest
{
	CODE_GENERATE_KEYBOARDTEST(Logitech)
	CODE_GENERATE_MOUSETEST(Logitech)
}