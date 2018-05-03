#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>

#include "../Logger.hpp"
#include "../Scope.hpp"
#include "../Thread.hpp"

IRSTD_TOPIC_REGISTER(IrStd, None);
IRSTD_TOPIC_USE_ALIAS(IrStdNone, IrStd, None);

// ---- IrStd::Logger ---------------------------------------------------------

// Global flag to enable/disable traces
bool IrStd::Logger::m_globalEnable = true;

IrStd::Logger::Logger()
{
	// By default topics are displayed only based on the min level
}

IrStd::Logger::Logger(const Stream& stream)
		: Logger()
{
	addStream(stream);
}

IrStd::Logger::Logger(const Logger& logger)
{
	*this = logger;
}

void IrStd::Logger::operator=(Logger const& logger)
{
	IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);
	m_streamList.empty();
	for (const auto& stream : logger.m_streamList)
	{
		addStream(stream);
	}
	m_filter = logger.m_filter;
}

IrStd::Logger& IrStd::Logger::getDefault()
{
	static Logger instance{std::cout};
	return instance;
}

void IrStd::Logger::disable() noexcept
{
	m_globalEnable = false;
}

// ---- IrStd::Logger::entry --------------------------------------------------

IrStd::Logger::OutputStreamPtr IrStd::Logger::entryStatic(
		const size_t line,
		const char* const file,
		const char* const func,
		Logger& logger,
		const Level level,
		const TopicImpl& topic)
{
	return std::move(logger.entry(line, file, func, level, topic));
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entryStatic(
		const size_t line,
		const char* const file,
		const char* const func,
		Logger& logger,
		const TopicImpl& topic,
		const Level level)
{
	return std::move(logger.entry(line, file, func, level, topic));
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entryStatic(
		const size_t line,
		const char* const file,
		const char* const func,
		const TopicImpl& topic,
		const Level level)
{
	return std::move(IrStd::Logger::getDefault().entry(line, file, func, level, topic));
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entryStatic(
		const size_t line,
		const char* const file,
		const char* const func,
		const Level level,
		const TopicImpl& topic)
{
	return std::move(IrStd::Logger::getDefault().entry(line, file, func, level, topic));
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entryStatic(
		const size_t line,
		const char* const file,
		const char* const func,
		const Level level,
		Logger& logger,
		const TopicImpl& topic)
{
	return std::move(logger.entry(line, file, func, level, topic));
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entry(
		const size_t line,
		const char* const file,
		const char* const func,
		const Level level,
		const TopicImpl& topic)
{
	Info info{level, topic, line, file, func};
	info.m_time = Type::Timestamp::now();
	auto pOutStream = OutputStreamPtr(new OutputStream(m_filter, m_streamList, info));
	return std::move(pOutStream);
}

void IrStd::Logger::addStream(const Stream& stream)
{
	IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);
	m_streamList.push_back(stream);
}

// ---- IrStd::Logger::isIgnored ----------------------------------------------

bool IrStd::Logger::isIgnoredStatic(
		Logger& logger,
		const Level level,
		const TopicImpl& topic) noexcept
{
	return logger.isIgnored(level, topic);
}

bool IrStd::Logger::isIgnoredStatic(
		Logger& logger,
		const TopicImpl& topic,
		const Level level) noexcept
{
	return logger.isIgnored(level, topic);
}

bool IrStd::Logger::isIgnoredStatic(
		const TopicImpl& topic,
		const Level level) noexcept
{
	return IrStd::Logger::getDefault().isIgnored(level, topic);
}

bool IrStd::Logger::isIgnoredStatic(
		const Level level, 
		const TopicImpl& topic) noexcept
{
	return IrStd::Logger::getDefault().isIgnored(level, topic);
}

bool IrStd::Logger::isIgnoredStatic(
		const Level level,
		Logger& logger,
		const TopicImpl& topic) noexcept
{
	return logger.isIgnored(level, topic);
}

bool IrStd::Logger::isIgnored(
		const Level level,
		const TopicImpl& topic) noexcept
{
	const bool isMainIgnored = m_filter.isIgnored(level, topic);
	for (auto& stream : m_streamList)
	{
		if (!stream.m_hasFilter)
		{
			if (!isMainIgnored)
			{
				return false;
			}
		}
		else if (!stream.m_filter.isIgnored(level, topic))
		{
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------

void IrStd::Logger::setLevel(const Level level) noexcept
{
	m_filter.setLevel(level);
}

void IrStd::Logger::allTopics() noexcept
{
	m_filter.allTopics();
}

void IrStd::Logger::addTopic(const TopicImpl& topic, const Level minLevel) noexcept
{
	m_filter.addTopic(topic, minLevel);
}

// ---- IrStd::Logger::Formater -----------------------------------------------

void IrStd::Logger::Format::raw(
		std::ostream& out,
		const Info& /*info*/,
		const std::string& str)
{
	out << str << std::endl;
}

void IrStd::Logger::Format::standard(
		std::ostream& out,
		const Info& info,
		const std::string& str)
{
	constexpr size_t SIZE_DATE = 23;
	constexpr size_t SIZE_THREADID = 21;
	constexpr size_t SIZE_LEVEL = 2;
	constexpr size_t SIZE_TOPIC = 20;
	constexpr size_t SIZE_FILE_LINE = 28;
	constexpr size_t SIZE_SEPARATOR = 1;

	// Format the date
	{
		out << info.m_time;
	}
	// Format the thread ID
	{
		out << " [0x" << std::setw(SIZE_THREADID - 5) << std::setfill('0') << std::hex << Thread::getHash() << "]";
	}
	// Format the level
	{
		out << " " << levelToChar(info.m_level);
	}
	// Format the topic
	{
		const auto pTopicStr = info.m_topic.getStr();
		const auto topicStrLength = std::strlen(pTopicStr);
		out << " " << std::setfill(' ') << std::left << std::setw((SIZE_TOPIC - 1))
				<< ((topicStrLength > (SIZE_TOPIC - 1)) ? &pTopicStr[topicStrLength - (SIZE_TOPIC - 1)] : pTopicStr);
	}
	// Format the file & line
	{
		constexpr size_t FILE_LENGTH = SIZE_FILE_LINE - 6;
		char buffer[FILE_LENGTH + 16];
		const size_t lenFile =  strlen(info.m_file);
		const size_t startFile = (lenFile > FILE_LENGTH) ? lenFile - FILE_LENGTH : 0;

		sprintf(buffer, "%s:%i", &info.m_file[startFile], static_cast<int>(info.m_line));

		out << " " << std::setw(SIZE_FILE_LINE - 1) << buffer;
	}

	// Reset
	{
		out << " " << std::right << std::dec;
	}

	// Body
	{
		std::string::size_type startPos = 0;
		std::string::size_type endPos = 0;
		bool firstLine = true;

		while ((endPos = str.find("\n", startPos)) != std::string::npos)
		{
			if (!firstLine)
			{
				out << "\n" << std::string(SIZE_DATE + SIZE_THREADID + SIZE_LEVEL
						+ SIZE_TOPIC + SIZE_FILE_LINE + SIZE_SEPARATOR, ' ');
			}
			out.write(&str.at(startPos), endPos - startPos);
			startPos = endPos + 1;
			firstLine = false;
		}

		// Print the rest
		if (startPos < str.size())
		{
			out << &str.at(startPos);
		}
	}

	// Tail
	out << std::endl;
}

// ---- IrStd::Logger::OutputStream -------------------------------------------

IrStd::Logger::OutputStream::OutputStream(
		const Filter filter,
		StreamList& streamList,
		const Info& info)
		: m_filter(filter)
		, m_streamList(streamList)
		, m_info(info)
		, m_bufferStream(m_buffer)
{
	m_buffer.reserve(1024);
}

IrStd::Logger::OutputStream::OutputStream(const OutputStream&& os)
		: OutputStream{os.m_filter, os.m_streamList, os.m_info}
{
}

IrStd::Logger::OutputStream::~OutputStream()
{
	static std::mutex mtx;

	// This prevents everything inside this scope to output trace
	IRSTD_SCOPE_THREAD(scope);
	if (scope.isActivator() && m_globalEnable)
	{
		const bool isMainIgnored = m_filter.isIgnored(m_info.m_level, m_info.m_topic);
		for (auto& stream : m_streamList)
		{
			if ((stream.m_hasFilter && !stream.m_filter.isIgnored(m_info.m_level, m_info.m_topic))
					|| (!stream.m_hasFilter && !isMainIgnored))
			{
				const std::string& str = m_bufferStream.str();
				std::lock_guard<std::mutex> lock(mtx);
				stream.m_format(stream.m_os, m_info, str);
			}
		}
	}
}
