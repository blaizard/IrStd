#include "../Test.hpp"
#include "../IrStd.hpp"

#include <sstream>
#include <thread>

IRSTD_TOPIC_REGISTER(TestTopic1);
IRSTD_TOPIC_REGISTER(TestTopic2);
IRSTD_TOPIC_REGISTER(TestTopic3);

class LoggerTest : public IrStd::Test
{
protected:
	void SetUp()
	{
		// Call parent function setup
		IrStd::Test::SetUp();

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

	std::stringstream& getLoggerStream() noexcept
	{
		return m_loggerStream;
	}

	void loggerClear() noexcept
	{
		m_loggerStream.str("");
	}

	IrStd::Logger m_logger;
	std::stringstream m_loggerStream;

public:
	// Helper functions
	void threadLogSync(const size_t id);
	void validateEachLine(const char* const regexMatch);
	void randomLoggerOperations(std::vector<IrStd::Logger>& loggerList);
};

// ---- LoggerTest::testSimple ------------------------------------------------

TEST_F(LoggerTest, testSimple) {
	// Simple string output
	{
		loggerClear();
		IRSTD_LOG(getLogger(), "test");
		ASSERT_TRUE(getLoggerStr() == "test\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}

	// Composed string output
	{
		loggerClear();
		IRSTD_LOG(getLogger(), "test" << 1.5 << "another" << -8);
		ASSERT_TRUE(getLoggerStr() == "test1.5another-8\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}

	// Various argument combination, this tests only at compilation level
	{
		IRSTD_LOG("test");
		IRSTD_LOG(IrStd::Topic::TestTopic1, "test");
		IRSTD_LOG(IrStd::Logger::Level::Trace, "test");
		IRSTD_LOG(IrStd::Topic::TestTopic1, IrStd::Logger::Level::Trace, "test");
		IRSTD_LOG(IrStd::Logger::Level::Trace, IrStd::Topic::TestTopic1, "test");
		IRSTD_LOG(getLogger(), IrStd::Topic::TestTopic1, "test");
		IRSTD_LOG(getLogger(), IrStd::Logger::Level::Trace, "test");
		IRSTD_LOG(getLogger(), IrStd::Logger::Level::Trace, IrStd::Topic::TestTopic1, "test");
		IRSTD_LOG(getLogger(), IrStd::Topic::TestTopic1, IrStd::Logger::Level::Trace, "test");
	}
}

// ---- LoggerTest::testThreadSync --------------------------------------------

void LoggerTest::threadLogSync(const size_t id)
{
	size_t loop = 50;
	while (loop--)
	{
		IRSTD_LOG(getLogger(), "[START] This is thread #" << id << " [END]");
	}
}

void LoggerTest::validateEachLine(const char* const regexMatch)
{
	std::istringstream f(getLoggerStr().c_str());
	std::string line;
	while (std::getline(f, line))
	{
		ASSERT_TRUE(validateOutput(line, regexMatch, RegexMatch::MATCH_ALL));
	}
}

TEST_F(LoggerTest, testThreadSync) {
	const size_t NB_THREADS = 10;
	std::thread t[NB_THREADS];

	for (size_t i = 0; i < NB_THREADS; ++i)
	{
		t[i] = std::thread(&LoggerTest::threadLogSync, this, i);
	}

	for (size_t i = 0; i < NB_THREADS; ++i)
	{
		t[i].join();
	}

	validateEachLine("\\[START\\] This is thread #[0-9]+ \\[END\\]");
}

// ---- LoggerTest::testMultiStream -------------------------------------------

TEST_F(LoggerTest, testMultiStream) {
	std::stringstream stream;
	// Add a new stream to the logger
	getLogger().addStream({stream, &IrStd::Logger::FormatRaw::getInstance()});

	// Double stream output
	{
		IRSTD_LOG(getLogger(), "test");
		ASSERT_TRUE(getLoggerStr() == "test\n") << "getLoggerStr()=\"" << getLoggerStr() << "\", stream.str()=\"" << stream.str() << "\"";
		ASSERT_TRUE(stream.str() == "test\n") << "stream.str()=\"" << stream.str() << "\", getLoggerStr()=\"" << getLoggerStr() << "\"";
	}
}

// ---- LoggerTest::testGlobalFilter ------------------------------------------

TEST_F(LoggerTest, testGlobalFilter)
{
	getLogger().setLevel(IrStd::Logger::Level::Trace);
	{
		loggerClear();
		IRSTD_LOG_TRACE(getLogger(), "trace");
		IRSTD_LOG_INFO(getLogger(), "info");
		IRSTD_LOG_INFO(getLogger(), IrStd::Topic::TestTopic1, "topic");
		ASSERT_TRUE(getLoggerStr() == "trace\ninfo\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}

	// Level
	getLogger().setLevel(IrStd::Logger::Level::Info);
	{
		loggerClear();
		IRSTD_LOG_TRACE(getLogger(), "trace");
		IRSTD_LOG_INFO(getLogger(), "info");
		IRSTD_LOG_INFO(getLogger(), IrStd::Topic::TestTopic1, "topic");
		ASSERT_TRUE(getLoggerStr() == "info\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}

	// Topic
	getLogger().addTopic(IrStd::Topic::TestTopic1);
	{
		loggerClear();
		IRSTD_LOG_TRACE(getLogger(), "trace");
		IRSTD_LOG_INFO(getLogger(), "info");
		IRSTD_LOG_INFO(getLogger(), IrStd::Topic::TestTopic1, "topic");
		ASSERT_TRUE(getLoggerStr() == "info\ntopic\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}

	// Non maskable
/*	{
		loggerClear();
		IRSTD_LOG_ERROR(getLogger(), "notopic");
		IRSTD_LOG_ERROR(getLogger(), IrStd::Topic::TestTopic1, "topic");
		ASSERT_TRUE(getLoggerStr() == "info\ntopic\n") << "getLoggerStr()=\"" << getLoggerStr() << "\"";
	}*/
}

// ---- LoggerTest::testStress ------------------------------------------------

void LoggerTest::randomLoggerOperations(std::vector<IrStd::Logger>& loggerList)
{
	std::vector<const IrStd::TopicImpl*> topicList;
	std::vector<IrStd::Logger::Level> levelList;
	const size_t nbOperations = 100;

	// Set the various topics
	{
		topicList.push_back(&IrStd::Topic::None);
		topicList.push_back(&IrStd::Topic::TestTopic1);
		topicList.push_back(&IrStd::Topic::TestTopic2);
		topicList.push_back(&IrStd::Topic::TestTopic3);
	}

	// Set the various levels
	{
		levelList.push_back(IrStd::Logger::Level::Trace);
		levelList.push_back(IrStd::Logger::Level::Debug);
		levelList.push_back(IrStd::Logger::Level::Info);
		levelList.push_back(IrStd::Logger::Level::Warning);
		levelList.push_back(IrStd::Logger::Level::Error);
	}

	// Perform a random operation
	for (size_t i = 0; i < nbOperations; i++)
	{
		auto& logger = m_rand.getVectorItem<IrStd::Logger>(loggerList);

		// Pick a random action
		switch (m_rand.getNumber<int>(0, 6))
		{
		// Use a random logger
		case 0:
		case 1:
		case 2:
		case 3:
			{
				const auto pTopic = m_rand.getVectorItem<const IrStd::TopicImpl*>(topicList);
				const auto level = m_rand.getVectorItem<IrStd::Logger::Level>(levelList);
				IRSTD_LOG(logger, level, *pTopic, "Test from logger ptr=" << reinterpret_cast<void*>(&logger));
			}
			break;
		// Add a new topic
		case 4:
			logger.addTopic(*m_rand.getVectorItem<const IrStd::TopicImpl*>(topicList));
			break;
		// Set level
		case 5:
			logger.setLevel(m_rand.getVectorItem<IrStd::Logger::Level>(levelList));
			break;
		// Remove all topics
		case 6:
			logger.allTopics();
			break;
		default:
			IRSTD_CRASH();
		}
	}
}

TEST_F(LoggerTest, testStress) {
	// Create the loggers
	std::vector<IrStd::Logger> loggerList;
	{
		for (size_t i = 0; i < m_rand.getNumber<size_t>(2, 10); i++)
		{
			loggerList.push_back(getLogger());
		}
	}

	randomLoggerOperations(loggerList);
	validateEachLine("Test from logger ptr=0x[0-9a-fA-F]+");
};

// ---- LoggerTest::testStressMultiThread -------------------------------------

TEST_F(LoggerTest, testStressMultiThread) {
	const size_t NB_THREADS = 10;
	std::thread t[NB_THREADS];
	// Create the loggers
	std::vector<IrStd::Logger> loggerList;
	{
		for (size_t i = 0; i < m_rand.getNumber<size_t>(2, 10); i++)
		{
			loggerList.push_back(getLogger());
		}
	}

	for (size_t i = 0; i < NB_THREADS; ++i)
	{
		t[i] = std::thread(&LoggerTest::randomLoggerOperations, this, std::ref(loggerList));
	}

	for (size_t i = 0; i < NB_THREADS; ++i)
	{
		t[i].join();
	}

	validateEachLine("Test from logger ptr=0x[0-9a-fA-F]+");
};
