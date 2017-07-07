#include "../IrStd.hpp"
#include "../Test.hpp"

int mainIrStd()
{
	// Print some intial information that might be usefull for debugging
	IrStd::Test::print("Platform: " IRSTD_PLATFORM_STRING);
	IrStd::Test::print("Compiler: " IRSTD_COMPILER_STRING);
#if IRSTD_IS_DEBUG
	IrStd::Test::print("Build type: Debug");
#endif
#if IRSTD_IS_RELEASE
	IrStd::Test::print("Build type: Release");
#endif
	return RUN_ALL_TESTS();
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	auto& main = IrStd::Main::getInstance();
	return main.call(mainIrStd);
}
