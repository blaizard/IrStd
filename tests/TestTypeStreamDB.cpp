#include "../Test.hpp"
#include "../IrStd.hpp"

class TypeStreamDBTest : public IrStd::Test
{
};

// ---- TypeStreamDBTest::testSimple ------------------------------------------

class TestEntry
{
public:
	static void write(IrStd::FileSystem::FileCsv& stream, const IrStd::Type::Timestamp timestamp, const TestEntry& test)
	{
		stream.write(static_cast<uint64_t>(timestamp), test.m_data1, test.m_data2);
	}

	bool m_data1;
	int m_data2;
};

class Cache : public IrStd::Type::StreamDBEntry<bool, int>
{
public:
	Cache() = default;
	Cache(const TestEntry& entry, Context& context)
	{
		set<0>(entry.m_data1, context);
		set<1>(entry.m_data2, context);
	}

	Cache(const std::string& entryStr, IrStd::Type::Timestamp& timestamp, Context& context)
	{
		std::cout << "[" << entryStr << "]" << std::endl;

		std::stringstream entryStream(entryStr);
		std::string cell;

		std::getline(entryStream, cell, ';');
		timestamp = IrStd::Type::Timestamp::fromString(cell.c_str());

		std::getline(entryStream, cell, ';');
		set<0>((cell == "0") ? false : true, context);

		std::getline(entryStream, cell, ';');
		set<1>(IrStd::Type::Numeric<int>::fromString(cell.c_str()), context);
	}
};

TEST_F(TypeStreamDBTest, testSimple)
{
	std::cout << sizeof(Cache) << "  " << sizeof(Cache::Context) << std::endl;

	IrStd::Type::StreamDB<TestEntry, Cache> db("test.csv");

	TestEntry entry{true, 12};
	db.push(IrStd::Type::Timestamp::now(), entry);

	TestEntry entry2{false, 13};
	db.push(IrStd::Type::Timestamp::now(), entry2);

	TestEntry entry3{true, 10};
	db.push(IrStd::Type::Timestamp::now(), entry3);

	std::cout << db.get<1>().getMin() << std::endl;
	std::cout << db.get<1>().getMax() << std::endl;

	ASSERT_TRUE(1);
}
