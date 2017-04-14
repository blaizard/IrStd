#include "../Test.hpp"
#include "../Compiler.hpp"

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	// Print some intial information that might be usefull for debugging
	IrStd::Test::print("Compiler: " IRSTD_COMPILER_STRING);
	return RUN_ALL_TESTS();
}
