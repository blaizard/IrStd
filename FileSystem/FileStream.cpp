#include "FileStream.hpp"

#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_REGISTER(IrStd, FileSystem, File);
IRSTD_TOPIC_USE_ALIAS(IrStdFile, IrStd, FileSystem, File);

// ---- IrStd::FileSystem::FileStream -----------------------------------------

IrStd::FileSystem::FileStream::FileStream(const std::string& path, const FileMode mode)
{
	switch (mode)
	{
	case FileMode::READ_WRITE:
		m_fileStream.open(path, std::fstream::in | std::fstream::out);
		break;
	case FileMode::APPEND:
		m_fileStream.open(path, std::fstream::in | std::fstream::app);
		break;
	case FileMode::READ:
		m_fileStream.open(path, std::fstream::in);
		break;
	default:
		IRSTD_UNREACHABLE(IrStdFile);
	}
}

IrStd::FileSystem::FileStream::~FileStream()
{
	m_fileStream.close();
}

std::fstream& IrStd::FileSystem::FileStream::getStream() noexcept
{
	return m_fileStream;
}
