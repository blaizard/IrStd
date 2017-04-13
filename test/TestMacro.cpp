#include "TestBase.hpp"

#include "../IrStd/Utils.hpp"

#define TEST_MACRO_SUM0() 0
#define TEST_MACRO_SUM1(a) a
#define TEST_MACRO_SUM2(a, b) a + b
#define TEST_MACRO_SUM3(a, b, c) a + b + c

#define TEST_MACRO_SUM(...) IRSTD_GET_MACRO(TEST_MACRO_SUM, __VA_ARGS__)(__VA_ARGS__)

namespace Test
{
	class MacroTest : public Base
	{
	};

	TEST_F(MacroTest, testIsEmpty) {
		// IRSTD_IS_EMPTY
		ASSERT_TRUE(IRSTD_IS_EMPTY() == 1);
		ASSERT_TRUE(IRSTD_IS_EMPTY(a) == 0);
		ASSERT_TRUE(IRSTD_IS_EMPTY(a, b, c) == 0);
		ASSERT_TRUE(IRSTD_IS_EMPTY("a, b") == 0);
		ASSERT_TRUE(IRSTD_IS_EMPTY(mido-qwd,l;cajwejr""''->test) == 0);

		// IRSTD_IS_EMPTY_X
		ASSERT_TRUE(IRSTD_IS_EMPTY_X(5, 9) == 5);
		ASSERT_TRUE(IRSTD_IS_EMPTY_X(78, -1, a) == -1);
	}

	TEST_F(MacroTest, testNArgs) {
		// IRSTD_NARGS
		ASSERT_TRUE(IRSTD_NARGS() == 0);
		ASSERT_TRUE(IRSTD_NARGS(a) == 1);
		ASSERT_TRUE(IRSTD_NARGS(a, b) == 2);
		ASSERT_TRUE(IRSTD_NARGS(a, b, c) == 3);
		ASSERT_TRUE(IRSTD_NARGS("a, b") == 1);
	}

	TEST_F(MacroTest, testGetMacro) {
		// IRSTD_GET_MACRO
		ASSERT_TRUE(TEST_MACRO_SUM(1, 2, 3) == 6);
		ASSERT_TRUE(TEST_MACRO_SUM(1, 2) == 3);
		ASSERT_TRUE(TEST_MACRO_SUM(1) == 1);
		ASSERT_TRUE(TEST_MACRO_SUM() == 0);
	}
}
