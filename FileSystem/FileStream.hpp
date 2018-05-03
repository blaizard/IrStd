#pragma once

#include <fstream>
#include <string>

namespace IrStd
{
	enum class FileMode
	{
		/**
		 * Open or create for read/write.
		 */
		READ_WRITE,
		/**
		 * Open for write only and append new content at the end of the file.
		 * Create the file if it doe snot exists.
		 */
		APPEND,
		/**
		 * Open for read only.
		 */
		READ
	};

	namespace FileSystem
	{
		class FileStream
		{
		public:
			/**
			 * \brief Open a file and keep it open until the destruction of this object
			 */
			FileStream(const std::string& path, const FileMode mode);

			/**
			 * \brief Close the file uppon instance destruction
			 */
			~FileStream();

			/**
			 * Return an instance of the stream
			 */
			std::fstream& getStream() noexcept;

		private:
			std::fstream m_fileStream;
		};
	}
}
