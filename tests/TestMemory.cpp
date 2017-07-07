#include "../Test.hpp"
#include "../IrStd.hpp"

class MemoryTest : public IrStd::Test
{
};

TEST_F(MemoryTest, testNewDelete)
{
#if IRSTD_IS_DEBUG
	{
		const size_t nbNew = IrStd::Memory::getInstance().getStatNbNew();
		const size_t nbDelete = IrStd::Memory::getInstance().getStatNbDelete();
		int* pInt = new int;
		ASSERT_TRUE(nbNew + 1 == IrStd::Memory::getInstance().getStatNbNew()) << "nbNew="
				<< nbNew << ", getStatNbNew()=" << IrStd::Memory::getInstance().getStatNbNew();
		*pInt = 12;
		delete pInt;
		ASSERT_TRUE(nbDelete + 1 == IrStd::Memory::getInstance().getStatNbDelete()) << "nbDelete="
				<< nbDelete << ", getStatNbDelete()=" << IrStd::Memory::getInstance().getStatNbDelete();
	}
#endif

	{
		std::set<size_t> testSet;
		testSet.insert(42);
		ASSERT_TRUE(testSet.find(42) != testSet.end());
	}
}
