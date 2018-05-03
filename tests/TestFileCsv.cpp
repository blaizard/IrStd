#include "../Test.hpp"
#include "../IrStd.hpp"

class FileCsvTest : public IrStd::Test
{
public:
	FileCsvTest()
			: m_path("irstd_file_csv_test.csv")
	{
	}

	void SetUp()
	{
		IrStd::Test::SetUp();
		IrStd::FileSystem::remove(m_path);
	}

	void TearDown()
	{
		IrStd::FileSystem::remove(m_path);
		IrStd::Test::TearDown();
	}

	const std::string m_path;
};

TEST_F(FileCsvTest, testReadWrite)
{
	// Create the file
	{
		IrStd::FileSystem::FileCsv file(m_path);

		// Write dummy data
		file.write(1, "test", -1);
		file.write(2, "test", -2);
		file.write(3, "test", -3);
	}

	std::string entry;
	// Go to the end of the file
	for (size_t bufferSize = 12; bufferSize < 34; ++bufferSize)
	{
		IrStd::FileSystem::FileCsv file(m_path, bufferSize);
		file.seekEnd();
		ASSERT_TRUE(file.read(entry));
		ASSERT_TRUE(validateOutput(entry, "3;test;-3;", RegexMatch::MATCH_ALL));
		ASSERT_TRUE(file.read(entry));
		ASSERT_TRUE(validateOutput(entry, "2;test;-2;", RegexMatch::MATCH_ALL));
		ASSERT_TRUE(file.read(entry));
		ASSERT_TRUE(validateOutput(entry, "1;test;-1;", RegexMatch::MATCH_ALL));
		ASSERT_TRUE(!file.read(entry));
	}
}

TEST_F(FileCsvTest, testReadWriteEmpty)
{
	// Create the file
	{
		IrStd::FileSystem::FileCsv file(m_path);

		// Write dummy data
		file.write("");
		file.write("");
		file.write("");
	}

	std::string entry;
	// Go to the end of the file
	for (size_t bufferSize = 3; bufferSize < 8; ++bufferSize)
	{
		IrStd::FileSystem::FileCsv file(m_path, bufferSize);
		file.seekEnd();
		ASSERT_TRUE(file.read(entry));
		ASSERT_TRUE(validateOutput(entry, ";", RegexMatch::MATCH_ALL));
		ASSERT_TRUE(file.read(entry));
		ASSERT_TRUE(validateOutput(entry, ";", RegexMatch::MATCH_ALL));
		ASSERT_TRUE(file.read(entry));
		ASSERT_TRUE(validateOutput(entry, ";", RegexMatch::MATCH_ALL));
		ASSERT_TRUE(!file.read(entry));
	}
}
