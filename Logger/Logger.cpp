#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <thread>

#include "../Logger.hpp"
#include "../Scope.hpp"

IRSTD_TOPIC_REGISTER(None, "");


IRSTD_TOPIC_REGISTER(TEST, "");

// ---- IrStd::Logger ---------------------------------------------------------

IrStd::Logger::Logger(const Stream& stream)
{
	addStream(stream);
}

IrStd::Logger::Logger(const Logger& logger)
{
	*this = logger;
}

void IrStd::Logger::operator=(Logger const& logger)
{
	m_streamList.empty();
	for (const auto& stream : logger.m_streamList)
	{
		addStream(stream);
	}
}

IrStd::Logger& IrStd::Logger::getDefault()
{
	static Logger instance{std::cout};
	return instance;
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entry(
		const size_t line,
		const char* const file,
		const char* const func,
		const Level level,
		const TopicImpl& topic)
{
	Info info{level, topic, line, file, func};
	gettimeofday(&info.m_time, 0);
	OutputStreamPtr pOutStream = IrStd::makeUnique<AllocatorRaw, OutputStream>(m_streamList, info);
	return std::move(pOutStream);
}

void IrStd::Logger::addStream(const Stream& stream)
{
	m_streamList.push_back(stream);
}

bool IrStd::Logger::isIgnored(const Level level, const TopicImpl& topic) const noexcept
{
	if (m_filter.isIgnored(level, topic))
	{
		return true;
	}
	for (auto& stream : m_streamList)
	{
		if (stream.m_filter.isIgnored(level, topic))
		{
			return true;
		}
	}
	return false;
}

void IrStd::Logger::setLevel(const Level level) noexcept
{
	m_filter.setLevel(level);
}

void IrStd::Logger::addTopic(const TopicImpl& topic) noexcept
{
	m_filter.addTopic(topic);
}

// ---- IrStd::Logger::Format -------------------------------------------------

void IrStd::Logger::FormatRaw::header(
		std::ostream& out,
		const Info& info) const
{
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
		const tm *pltm = localtime(&info.m_time.tv_sec);
		out << std::dec << std::right << std::setfill('0')
				<< std::setw(4) << (pltm->tm_year + 1900)
				<< "-" << std::setw(2) << (pltm->tm_mon + 1)
				<< "-" << std::setw(2) << (pltm->tm_mday)
				<< " " << std::setw(2) << (pltm->tm_hour)
				<< ":" << std::setw(2) << (pltm->tm_min)
				<< ":" << std::setw(2) << (pltm->tm_sec)
				<< "." << std::setw(6) << (info.m_time.tv_usec);
	}
	// Format the thread ID
	{
		const std::thread::id id = std::this_thread::get_id();
		out << " [0x" << std::setw(10) << std::hex << id << "]";
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
		constexpr size_t FILE_LENGTH = 32;
		char buffer[FILE_LENGTH + 16];
		const size_t lenFile =  strlen(info.m_file);
		const size_t startFile = (lenFile > FILE_LENGTH) ? lenFile - FILE_LENGTH : 0;
		const size_t newlenFile = (lenFile > FILE_LENGTH) ? FILE_LENGTH : lenFile;

		strcpy(buffer, &info.m_file[startFile]);
		buffer[newlenFile] = ':';
		sprintf(&buffer[newlenFile + 1], "%i", static_cast<int>(info.m_line));

		out << " " << std::setw(20) << buffer << "\t";
	}
}

void IrStd::Logger::FormatDefault::tail(std::ostream& out) const
{
	out << std::endl;
}

// ---- IrStd::Logger::OutputStream -------------------------------------------

IrStd::Logger::OutputStream::OutputStream(
		const StreamList& streamList,
		const Info& info)
		: m_streamList(streamList)
		, m_info(info)
		, m_bufferStream(m_buffer)
{
	m_buffer.reserve(1024);
}

IrStd::Logger::OutputStream::OutputStream(const OutputStream&& os)
		: OutputStream{os.m_streamList, os.m_info}
{
}

IrStd::Logger::OutputStream::~OutputStream()
{
	static std::mutex mtx;

	// This prevents everything inside this scope to output trace
	IRSTD_SCOPE_THREAD(scope);
	if (scope.isActivator())
	{
		for (auto& stream : m_streamList)
		{
			if (!stream.m_filter.isIgnored(m_info.m_level, m_info.m_topic))
			{
				const std::string& str = m_bufferStream.str();
				mtx.lock();
				stream.m_pFormat->header(stream.m_os, m_info);
				stream.m_os << str;
				stream.m_pFormat->tail(stream.m_os);
				mtx.unlock();
			}
		}
	}
}
