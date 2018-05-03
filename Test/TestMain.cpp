#include "../IrStd.hpp"
#include "../Test.hpp"

#define TEST_VERSION_MAJOR 1
#define TEST_VERSION_MINOR 0

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
	IrStd::Test::print("Version: " + IrStd::Main::getInstance().getVersion());
	return RUN_ALL_TESTS();
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	auto& main = IrStd::Main::getInstance();
	main.setVersion(TEST_VERSION_MAJOR, TEST_VERSION_MINOR);
	return main.call(mainIrStd);
}
