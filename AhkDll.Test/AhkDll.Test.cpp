#include "pch.h"
#include <thread>
#include <mutex>
#include <queue>
#include "CppUnitTest.h"

#include <IbWinCppLib/WinCppLib.hpp>
#include "../AhkDll/IbAhkSend.hpp"

#pragma comment(lib, "winmm.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AhkDllTest
{
	TEST_CLASS(AhkDllTest)
	{
	public:
		static inline HHOOK hook;
		static inline std::queue<DWORD> input_keys;
		static inline std::mutex mutex;

		TEST_METHOD_INITIALIZE(TestInit)
		{
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

			int error = IbAhkSendInit();
			Assert::AreEqual(0, error);
		}

		static LRESULT CALLBACK LowLevelKeyboardProc(
			_In_ int    nCode,
			_In_ WPARAM wParam,
			_In_ LPARAM lParam
		) {
			if (nCode == HC_ACTION) {
				KBDLLHOOKSTRUCT* p = ib::Addr(lParam);
				{
					std::lock_guard lock(mutex);
					input_keys.push(p->vkCode);
				}
			}
			return CallNextHookEx(hook, nCode, wParam, lParam);
		}

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

		TEST_METHOD(MeasureKeyboardLatency) {
			Measure measure;

			measure.begin();
			{
				LARGE_INTEGER t1, t2;
				QueryPerformanceCounter(&t1);

				INPUT input[2];

				input[0].type = INPUT_KEYBOARD;
				input[0].ki = {};
				input[0].ki.wVk = VK_F12;

				input[1].type = INPUT_KEYBOARD;
				input[1].ki = {};
				input[1].ki.wVk = VK_F12;
				input[1].ki.dwFlags = KEYEVENTF_KEYUP;

				IbAhkSendInput(2, input, sizeof INPUT);
			}
			uint64_t t1 = measure.end();

			measure.begin();
			while (input_keys.size() < 2)
				_mm_pause();
			uint64_t t2 = measure.end();  //0.8~4ms
			{
				std::lock_guard lock(mutex);
				Assert::AreEqual<DWORD>(VK_F12, input_keys.back());
				input_keys.pop();
				Assert::AreEqual<DWORD>(VK_F12, input_keys.back());
				input_keys.pop();
			}

			Logger::WriteMessage((std::to_wstring(t1) + L"\n").c_str());
			Logger::WriteMessage((std::to_wstring(t2) + L"\n").c_str());
		}
		
		TEST_METHOD(MeasureMouseMove) {
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

		TEST_METHOD_CLEANUP(TestDestroy) {
			IbAhkSendDestroy();
			UnhookWindowsHookEx(hook);
		}
	};
}