#include "../Test.hpp"
#include "../Logger.hpp"

#include <sstream>
#include <thread>

class LoggerTest : public IrStd::Test
{
protected:
	void SetUp()
	{
		m_loggerStream.str("");
		IrStd::Logger::Stream stream{m_loggerStream, &IrStd::Logger::FormatRaw::getInstance()};
		m_logger = IrStd::Logger{stream};
	}

	IrStd::Logger& getLogger() noexcept
	{
		return m_logger;
	}

	std::string getLoggerStr() const noexcept
	{
		return m_loggerStream.str();
	}

	void loggerClear() noexcept
	{
		m_loggerStream.str("");
	}

	std::stringstream m_loggerStream;
	IrStd::Logger m_logger;
};

// ---- LoggerTest::testSimple ------------------------------------------------

TEST_F(LoggerTest, testSimple) {
	// Simple string output
	{
		loggerClear();
		IRSTD_LOG_X(getLogger(), "test");
		ASSERT_TRUE(getLoggerStr() == "test\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}

	// Composed string output
	{
		loggerClear();
		IRSTD_LOG_X(getLogger(), "test" << 1.5 << "another" << -8);
		ASSERT_TRUE(getLoggerStr() == "test1.5another-8\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}
}

// ---- LoggerTest::testThreadSync --------------------------------------------

static void threadLogSync(const size_t id, IrStd::Logger& logger)
{
	size_t loop = 50;
	while (loop--)
	{
		IRSTD_LOG_X(logger, "[START] This is thread #" << id << " [END]");
	}
}

TEST_F(LoggerTest, testThreadSync) {
	const size_t NB_THREADS = 10;
	std::thread t[NB_THREADS];

	for (int i = 0; i < NB_THREADS; ++i) {
		t[i] = std::thread(threadLogSync, i, std::ref(getLogger()));
	}

	for (int i = 0; i < NB_THREADS; ++i) {
		t[i].join();
	}

	const char* const regexMatch = "(\\[START\\] This is thread #[0-9]+ \\[END\\]" IRSTD_TEST_REGEX_EOL ")+";
	// Expect success
	{
		ASSERT_TRUE(validateOutput(getLoggerStr().c_str(), regexMatch, RegexMatch::MATCH_ALL));
	}

	// Expect failure
	{
		IRSTD_LOG_X(getLogger(), "This should make the test fail");
		ASSERT_TRUE(validateOutput(getLoggerStr().c_str(), regexMatch, RegexMatch::MATCH_ALL, /*expectSuccess*/false));
	}
}

// ---- LoggerTest::testMultiStream -------------------------------------------

TEST_F(LoggerTest, testMultiStream) {
	std::stringstream stream;
	// Add a new stream to the logger
	getLogger().addStream({stream, &IrStd::Logger::FormatRaw::getInstance()});

	// Double stream output
	{
		IRSTD_LOG_X(getLogger(), "test");
		ASSERT_TRUE(getLoggerStr() == "test\n") << "getLoggerStr()=\"" << getLoggerStr() << "\", stream.str()=\"" << stream.str() << "\"";
		ASSERT_TRUE(stream.str() == "test\n") << "stream.str()=\"" << stream.str() << "\", getLoggerStr()=\"" << getLoggerStr() << "\"";
	}
}

// ---- LoggerTest::testFilter ------------------------------------------------

TEST_F(LoggerTest, testFilter) {
	
}
