#include "../FileSystem.hpp"
#include "../Assert.hpp"
#include "../Main.hpp"

#include <array>
#include <stdio.h>
#include <fstream>

#if IRSTD_IS_PLATFORM(LINUX)
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#else
	IRSTD_STATIC_ERROR("This platform is not supported");
#endif

IRSTD_TOPIC_REGISTER(IrStd, FileSystem);

// ---- IrStd::FileSystem -----------------------------------------------------

bool IrStd::FileSystem::mkdir(const std::string& path)
{
	std::string tempPath(path);
	{
		size_t pos = 0;
		do
		{
			pos = tempPath.find(DIRECTORY_SEPARATOR, pos + 1);
			if (pos != std::string::npos)
			{
				tempPath.at(pos) = '\0';
			}
			if (!isDirectory(tempPath))
			{
				const int retVal = ::mkdir(tempPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				if (retVal == -1)
				{
					return false;
				}
			}
			if (pos != std::string::npos)
			{
				tempPath.at(pos) = DIRECTORY_SEPARATOR;
			}
		} while (pos != std::string::npos);
	}
	return true;
}

bool IrStd::FileSystem::pwd(std::string& path)
{
	const auto pExecutablePath = IrStd::Main::getExecutablePath();
	if (pExecutablePath == nullptr)
	{
		return false;
	}

	path.assign(pExecutablePath);
	const auto pos = path.find_last_of(DIRECTORY_SEPARATOR);
	path.resize(pos);

	return true;
}

void IrStd::FileSystem::append(std::string& path, const std::string& directory)
{
	if (directory.empty())
	{
		return;
	}

	// Position the pointer after the separator
	size_t indexPostSeparator = 0;
	while (directory.at(indexPostSeparator) == DIRECTORY_SEPARATOR)
	{
		indexPostSeparator++;
	}

	if (path.back() != DIRECTORY_SEPARATOR)
	{
		path += DIRECTORY_SEPARATOR;
	}
	path.append(directory, indexPostSeparator, std::string::npos);
}

bool IrStd::FileSystem::isFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return ((s.st_mode & S_IFREG) ? true : false);
	}
	return false;
}

bool IrStd::FileSystem::isDirectory(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return ((s.st_mode & S_IFDIR) ? true : false);
	}
	return false;
}

bool IrStd::FileSystem::remove(const std::string& path)
{
	const auto result = std::remove(path.c_str());
	return (result) ? false : true;
}
