#include "pch.h"
#include "CppUnitTest.h"

#include <IbWinCppLib/WinCppLib.hpp>
#include "../AhkDll/import.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AhkDllTest
{
	TEST_CLASS(AhkDllTest)
	{
	public:
		TEST_METHOD_INITIALIZE(TestInit)
		{
			int error = IbAhkSendInit();
			Assert::AreEqual(0, error);
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
		}
	};
}