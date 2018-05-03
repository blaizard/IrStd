#include "../Test.hpp"
#include "../IrStd.hpp"

class MemoryTest : public IrStd::Test
{
};

TEST_F(MemoryTest, testNewDelete)
{
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

	{
		std::set<size_t> testSet;
		testSet.insert(42);
		ASSERT_TRUE(testSet.find(42) != testSet.end());
	}
}

TEST_F(MemoryTest, testVirtualMemory)
{
	const auto total = IrStd::Memory::getInstance().getVirtualMemoryTotal();
	const auto used = IrStd::Memory::getInstance().getVirtualMemoryTotalUsed();
	const auto current = IrStd::Memory::getInstance().getVirtualMemoryCurrent();
	ASSERT_TRUE(total > 1024) << "total=" << total;
	ASSERT_TRUE(used > 1024) << "used=" << used;
	ASSERT_TRUE(current > 1024) << "current=" << current;
	ASSERT_TRUE(total > used) << "total=" << total << ", used=" << used;
	ASSERT_TRUE(used > current) << "used=" << used << ", current=" << current;
	getStdout() << "Virtual Memory: total=" << total << ", used=" << used << ", current=" << current << std::endl;
}

TEST_F(MemoryTest, testRAM)
{
	const auto total = IrStd::Memory::getInstance().getRAMTotal();
	const auto used = IrStd::Memory::getInstance().getRAMTotalUsed();
	const auto current = IrStd::Memory::getInstance().getRAMCurrent();
	ASSERT_TRUE(total > 1024) << "total=" << total;
	ASSERT_TRUE(used > 1024) << "used=" << used;
	ASSERT_TRUE(current > 1024) << "current=" << current;
	ASSERT_TRUE(total > used) << "total=" << total << ", used=" << used;
	ASSERT_TRUE(used > current) << "used=" << used << ", current=" << current;
	getStdout() << "RAM: total=" << total << ", used=" << used << ", current=" << current << std::endl;
}
