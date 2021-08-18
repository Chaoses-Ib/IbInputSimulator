#include "../AhkDll/IbAhkSend.hpp"
using namespace Send;

#define BOOST_TEST_MODULE AhkDll.Test
#include <boost/test/unit_test.hpp>
#include <fmt/core.h>

#include <thread>
#include <mutex>
#include <future>
#include <queue>
#include <IbWinCppLib/WinCppLib.hpp>


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

template <SendType TypeV>
class InitTest {
public:
	InitTest() {
		Send::Error error = IbAhkSendInit(TypeV, 0, nullptr);
		BOOST_REQUIRE(error == Send::Error::Success);
	}

	~InitTest() {
		IbAhkSendDestroy();
	}
};

template <SendType TypeV>
class KeyboardTest : public InitTest<TypeV>
{
	static inline HHOOK hook;
	static inline bool capture;
	static inline std::queue<DWORD> input_keys;
	static inline std::mutex mutex;

	std::thread t;

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
public:
	KeyboardTest() {
		capture = false;
		std::promise<HHOOK> pro;
		std::future<HHOOK> fut = pro.get_future();
		t = std::thread([](std::promise<HHOOK>&& pro) {
			pro.set_value(SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0));

			MSG messages;
			while (GetMessageW(&messages, NULL, 0, 0))
			{
				TranslateMessage(&messages);
				DispatchMessageW(&messages);
			}
		}, std::move(pro));
		hook = fut.get();
		BOOST_REQUIRE(hook != NULL);
	}

	~KeyboardTest() {
		std::thread::id id = t.get_id();
		PostThreadMessageW(*(DWORD*)(&id), WM_QUIT, 0, 0);
		UnhookWindowsHookEx(hook);
		input_keys = {};

		t.join();
	}

	void TestKeyboardLatency() {
		Measure measure;

		INPUT input[2];

		input[0].type = INPUT_KEYBOARD;
		input[0].ki = {};
		input[0].ki.wVk = VK_F12;

		input[1].type = INPUT_KEYBOARD;
		input[1].ki = {};
		input[1].ki.wVk = VK_F12;
		input[1].ki.dwFlags = KEYEVENTF_KEYUP;

		capture = true;
		measure.begin();
		IbAhkSendInput(2, input, sizeof INPUT);
		uint64_t t1 = measure.end();
		
		measure.begin();
		{
			while (input_keys.size() < 2)
				_mm_pause();
			capture = false;
		}
		uint64_t t2 = measure.end();

		{
			std::lock_guard lock(mutex);
			BOOST_CHECK(input_keys.back() == VK_F12);
			input_keys.pop();
		    BOOST_CHECK(input_keys.back() == VK_F12);
			input_keys.pop();
		}

		BOOST_TEST_MESSAGE(fmt::format("Duration: {}ns\n", t1));
		//SendInput: 1~3ms
		//Logitech: 25~45us
		BOOST_TEST_MESSAGE(fmt::format("Latency: {}ns\n", t2));
		//SendInput: 100ns (0)
		//Logitech: 0.8~4ms
	}
};

template <SendType TypeV>
class MouseTest : public InitTest<TypeV> {
public:
	void TestMouseMove() {
		Measure measure;
		
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi = {};
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;

		measure.begin();
		for (size_t i = 0; i < 10000 / 200; i++) {
			input.mi.dx = input.mi.dy = 1;
			for (size_t i = 0; i < 100; i++)
				IbAhkSendInput(1, &input, sizeof INPUT);

			input.mi.dx = input.mi.dy = -1;
			for (size_t i = 0; i < 100; i++)
				IbAhkSendInput(1, &input, sizeof INPUT);
		}
		uint64_t t = measure.end();
		BOOST_TEST_MESSAGE(fmt::format("Duration: {}ns\n", t / 10000));
		//SendInput: 200~800us
		//Logitech: 3~10us
	}

	void TestMouseMoveLatency() {
		Measure measure;

		POINT p1, p2, d;
		GetCursorPos(&p1);
		
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi = {};
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
		input.mi.dx = input.mi.dy = 100;
		IbAhkSendInput(1, &input, sizeof INPUT);

		measure.begin();
		do {
			GetCursorPos(&p2);
			d = { p2.x - p1.x, p2.y - p1.y };
		} while (!(d.x >= 50 && d.y >= 50));
		uint64_t latency = measure.end();
		BOOST_TEST_MESSAGE(fmt::format("Latency: {}ns\n", latency));
		//SendInput: 3~200us
		//Logitech: 900~4000us (0.9~4ms)
	}

	void TestMouseMoveDistance() {
		POINT p1, p2, d1, d2;
		GetCursorPos(&p1);

		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi = {};
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
		input.mi.dx = input.mi.dy = 100;
		IbAhkSendInput(1, &input, sizeof INPUT);

		GetCursorPos(&p2);
		d1 = { p2.x - p1.x, p2.y - p1.y };

		Sleep(10);

		GetCursorPos(&p2);
		d2 = { p2.x - p1.x, p2.y - p1.y };
		BOOST_CHECK((abs(d2.x - 100) <= 10 && abs(d2.y - 100) <= 100));

		BOOST_TEST_MESSAGE(fmt::format("0ms: ({}, {})\n", d1.x, d1.y));
		BOOST_TEST_MESSAGE(fmt::format("10ms: ({}, {})\n", d2.x, d2.y));
	}
};

#define CODE_GENERATE_TEST_NAME(name, type)  \
BOOST_AUTO_TEST_SUITE(name)  \
    BOOST_FIXTURE_TEST_SUITE(Keyboard, KeyboardTest<SendType::type>)  \
		using base = BOOST_AUTO_TEST_CASE_FIXTURE;  \
		BOOST_AUTO_TEST_CASE(TestKeyboardLatency) {  \
		    base::TestKeyboardLatency();  \
		}  \
    BOOST_AUTO_TEST_SUITE_END()  \
      \
	BOOST_FIXTURE_TEST_SUITE(Mouse, MouseTest<SendType::type>)  \
		using base = BOOST_AUTO_TEST_CASE_FIXTURE;  \
		BOOST_AUTO_TEST_CASE(TestMouseMove) {  \
		    base::TestMouseMove();  \
		}  \
		BOOST_AUTO_TEST_CASE(TestMouseMoveLatency) {  \
		    base::TestMouseMoveLatency();  \
		}  \
		BOOST_AUTO_TEST_CASE(TestMouseMoveDistance) {  \
		    base::TestMouseMoveDistance();  \
		}  \
	BOOST_AUTO_TEST_SUITE_END()  \
BOOST_AUTO_TEST_SUITE_END()
#define CODE_GENERATE_TEST(type) CODE_GENERATE_TEST_NAME(type, type)

CODE_GENERATE_TEST_NAME(SendInput_, SendInput)
CODE_GENERATE_TEST(AnyDriver)
CODE_GENERATE_TEST(Logitech)