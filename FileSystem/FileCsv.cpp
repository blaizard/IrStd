#include "FileCsv.hpp"

// ---- IrStd::FileSystem::FileCsv --------------------------------------------

constexpr char IrStd::FileSystem::FileCsv::SEPARATOR;
constexpr char IrStd::FileSystem::FileCsv::NEWLINE[];

void IrStd::FileSystem::FileCsv::seekEnd()
{
	getStream().seekg(0, getStream().end);
	m_readSeek = getStream().tellg();
}

bool IrStd::FileSystem::FileCsv::read(std::string& entry)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	bool needFetch = (m_readBuffer.size() <= strlen(NEWLINE));
	bool fetched = false;
	while (true)
	{
		if (needFetch)
		{
			IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Type), !fetched,
					"The read buffer is too small (" << m_readBufferSize << ") to read a single line");
			m_readSeek += m_readBuffer.size();
			updateReadBufferNoLock();
			needFetch = false;
			fetched = true;
		}

		// The end of the file has been reached
		if (m_readBuffer.size() == 0)
		{
			return false;
		}

		IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Type), m_readBuffer.size() > strlen(NEWLINE),
				"The CSV file seems to be corrupted");

		const auto pos = m_readBuffer.rfind(NEWLINE, m_readBuffer.size() - strlen(NEWLINE) - 1);
		if (pos != std::string::npos || m_readSeek == 0)
		{
			// Make sure the end characters are the newline
			for (size_t n = 0; n < strlen(NEWLINE); ++n)
			{
				IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Type),
						m_readBuffer.at(m_readBuffer.size() - 1 - n) == NEWLINE[strlen(NEWLINE) - 1 - n],
						"Buffer read: " << m_readBuffer);
			}

			const auto beginPos = (pos != std::string::npos) ? pos + strlen(NEWLINE) : 0;

			entry.assign(m_readBuffer, beginPos, m_readBuffer.size() - beginPos - strlen(NEWLINE));
			m_readBuffer.resize(beginPos);

			return true;
		}
		// Otherwise more data needs to be fetched
		else
		{
			needFetch = true;
		}
	}
}

void IrStd::FileSystem::FileCsv::updateReadBufferNoLock()
{
	const size_t seek = (m_readSeek > m_readBufferSize) ? m_readSeek - m_readBufferSize : 0;
	const size_t nbDataExpected = (m_readSeek > m_readBufferSize) ? m_readBufferSize : m_readSeek;

	if (nbDataExpected == 0)
	{
		m_readBuffer.resize(0);
		return;
	}

	// Position the read pointer
	getStream().seekg(seek);

	// Read the data
	m_readBuffer.resize(nbDataExpected);
	getStream().read(&m_readBuffer.at(0), nbDataExpected);
	const size_t nbDataRead = getStream().gcount();

//	IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Type), !getStream().badbit,
//			"An error occured while reading the file");
	IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Type), nbDataExpected == nbDataRead,
			"The data expected (" << nbDataExpected << ") is different than the data read ("
			<< nbDataRead << ")");

	m_readSeek = seek;
}
