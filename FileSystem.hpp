#pragma once

#include <string>

#include "Compiler.hpp"

namespace IrStd
{
	namespace FileSystem
	{
		bool mkdir(const std::string& path);
		bool remove(const std::string& path);

		bool pwd(std::string& path);
		void append(std::string& path, const std::string& directory);

		bool isFile(const std::string& path);
		bool isDirectory(const std::string& path);

	#if IRSTD_IS_PLATFORM(LINUX)
		constexpr static char DIRECTORY_SEPARATOR = '/';
	#elif IRSTD_IS_PLATFORM(WINDOWS)
		constexpr static char DIRECTORY_SEPARATOR = '\\';
	#endif
	};
}

// File specific implementations
#include "FileSystem/FileStream.hpp"
#include "FileSystem/FileCsv.hpp"
