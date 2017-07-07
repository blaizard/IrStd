#pragma once

#include <memory>
#include <vector>

namespace IrStd
{
	class Data
	{
	public:
		Data(const char* const pData);
		Data(const uint8_t* const pData, const size_t length);
		Data(const size_t length);

		uint8_t* get() noexcept;
		const uint8_t* get() const noexcept;
		size_t getLength() const noexcept;
		void setLength(const size_t length) noexcept;

		/**
		 * Convert the data to an hexadecimal string
		 */
		std::unique_ptr<std::string> toHex() const;

	private:
		size_t m_length;
		size_t m_bufferLength;
		std::vector<uint8_t> m_pBuffer;
	};
}
