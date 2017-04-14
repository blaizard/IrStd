#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <thread>

#include "../Logger.hpp"
#include "../Exception.hpp"
#include "../Scope.hpp"

IRSTD_TOPIC_REGISTER(None, "");

// ---- IrStd::Logger ---------------------------------------------------------

IrStd::Logger::~Logger()
{
	// Flush the streams
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entry()
{
	timeval t;
	gettimeofday(&t, 0);
	const Info info{Level::Info, IrStd::Topic::None, 0, "", "", t};
	auto pOutStream = entry(info);
	return std::move(pOutStream);
}

IrStd::Logger::OutputStreamPtr IrStd::Logger::entry(const Info& info)
{
	OutputStreamPtr pOutStream = IrStd::makeUnique<AllocatorRaw, OutputStream>(m_streamList, info);
	return std::move(pOutStream);
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
		const tm *pltm = localtime(&info.time.tv_sec);
		out << std::dec << std::right << std::setfill('0')
				<< std::setw(4) << (pltm->tm_year + 1900)
				<< "-" << std::setw(2) << (pltm->tm_mon + 1)
				<< "-" << std::setw(2) << (pltm->tm_mday)
				<< " " << std::setw(2) << (pltm->tm_hour)
				<< ":" << std::setw(2) << (pltm->tm_min)
				<< ":" << std::setw(2) << (pltm->tm_sec)
				<< "." << std::setw(6) << (info.time.tv_usec);
	}
	// Format the thread ID
	{
		const std::thread::id id = std::this_thread::get_id();
		out << " [0x" << std::setw(10) << std::hex << id << "]";
	}
	// Format the level
	{
		const uint32_t level = static_cast<uint32_t>(info.level);
		out << " " << static_cast<const char>(level & 0xff);
	}
	// Format the level, topic and file information
	out << " " << std::setfill(' ') << std::left << std::setw(12) << info.topic.getStr();
	// Format the file & line
	{
		constexpr size_t FILE_LENGTH = 32;
		char buffer[FILE_LENGTH + 16];
		const size_t lenFile =  strlen(info.file);
		const size_t startFile = (lenFile > FILE_LENGTH) ? lenFile - FILE_LENGTH : 0;
		const size_t newlenFile = (lenFile > FILE_LENGTH) ? FILE_LENGTH : lenFile;

		strcpy(buffer, &info.file[startFile]);
		buffer[newlenFile] = ':';
		sprintf(&buffer[newlenFile + 1], "%i", static_cast<int>(info.line));

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
			mtx.lock();
			stream.m_pFormat->header(stream.m_os, m_info);
			stream.m_os << m_bufferStream.str();
			stream.m_pFormat->tail(stream.m_os);
			mtx.unlock();
		}
	}
}
