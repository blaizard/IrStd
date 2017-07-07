#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>

#include "../Logger.hpp"
#include "../Scope.hpp"
#include "../Thread.hpp"

IRSTD_TOPIC_REGISTER(None, "");

// ---- IrStd::Logger ---------------------------------------------------------

// Global flag to enable/disable traces
bool IrStd::Logger::m_globalEnable = true;

IrStd::Logger::Logger()
{
	// By default only None topics are displayed
	m_filter.addTopic(IrStd::Topic::None);
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

void IrStd::Logger::addTopic(const TopicImpl& topic) noexcept
{
	m_filter.addTopic(topic);
}

// ---- IrStd::Logger::Format -------------------------------------------------

void IrStd::Logger::FormatRaw::header(
		std::ostream& /*out*/,
		const Info& /*info*/) const
{
}

void IrStd::Logger::FormatRaw::body(
	std::ostream& out,
	const std::string& str) const
{
	out << str;
}

void IrStd::Logger::FormatRaw::tail(std::ostream& out) const
{
	out << std::endl;
}

// ---- IrStd::Logger::FormatDefault ------------------------------------------

void IrStd::Logger::FormatDefault::header(
		std::ostream& out,
		const Info& info) const
{
	// Format the date
	{
		out << info.m_time;
	}
	// Format the thread ID
	{
		out << " [0x" << std::setw(16) << std::setfill('0') << std::hex << Thread::getHash() << "]";
	}
	// Format the level
	{
		const uint32_t level = static_cast<uint32_t>(info.m_level);
		out << " " << static_cast<const char>(level & 0xff);
	}
	// Format the level, topic and file information
	out << " " << std::setfill(' ') << std::left << std::setw(12) << info.m_topic.getStr();
	// Format the file & line
	{
		constexpr size_t FILE_LENGTH = 22;
		char buffer[FILE_LENGTH + 16];
		const size_t lenFile =  strlen(info.m_file);
		const size_t startFile = (lenFile > FILE_LENGTH) ? lenFile - FILE_LENGTH : 0;

		sprintf(buffer, "%s:%i", &info.m_file[startFile], static_cast<int>(info.m_line));

		out << " " << std::setw(FILE_LENGTH + 5) << buffer;
	}

	// Reset
	{
		out << " " << std::right << std::dec;
	}
}

void IrStd::Logger::FormatDefault::body(
	std::ostream& out,
	const std::string& str) const
{
	std::string::size_type startPos = 0;
	std::string::size_type endPos = 0;
	bool firstLine = true;

	while ((endPos = str.find("\n", startPos)) != std::string::npos)
	{
		if (!firstLine)
		{
			out << "\n" << std::string(88, ' ');
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

void IrStd::Logger::FormatDefault::tail(std::ostream& out) const
{
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
				stream.m_pFormat->header(stream.m_os, m_info);
				stream.m_pFormat->body(stream.m_os, str);
				stream.m_pFormat->tail(stream.m_os);
			}
		}
	}
}
