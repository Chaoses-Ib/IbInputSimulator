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
		TEST_METHOD(TestInit)
		{
			int error = IbAhkSendInit();
			Assert::AreEqual(0, error);
		}
	};
}