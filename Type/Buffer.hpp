#pragma once

#include <memory>
#include <vector>

namespace IrStd
{
	namespace Type
	{
		class Buffer
		{
		public:
			/**
			 * \brief Create an empty buffer of a specific size
			 */
			Buffer(const size_t length);

			/**
			 * \brief Create a buffer from a string
			 */
			Buffer(const std::string& str, const bool copy = true);

			/**
			 * \brief Create a buffer from a constant char
			 */
			Buffer(const char* const pData, const bool copy = true);

			/**
			 * \brief Copy constructor
			 */
			Buffer(const Buffer& buffer, const bool copy = true);

			/**
			 * \brief Create a buffer from a raw buffer
			 */
			template<class T>
			Buffer(const T* const pData, const size_t length, const bool copy = true)
					: m_length(length)
					, m_pBuffer((copy) ? (new uint8_t[m_length]) : const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(pData)))
					, m_allocated(copy)
			{
				if (copy)
				{
					std::memcpy(m_pBuffer, pData, m_length);
				}
			}

			~Buffer();

			template<class ... Args>
			void memcpy(const size_t position, Args&& ... args)
			{
				const Buffer buffer(std::forward<Args>(args)..., /*copy*/false);
				this->memcpy(position, buffer);
			}

			template<class T>
			T* getForWrite() noexcept
			{
				IRSTD_ASSERT(m_allocated == true, "Only allocated (i.e. copied) buffers can be used for write");
				return reinterpret_cast<T*>(m_pBuffer);
			}

			template<class T>
			const T* get() const noexcept
			{
				return reinterpret_cast<T*>(m_pBuffer);
			}


			size_t size() const noexcept;
			void setSize(const size_t length) noexcept;

			/**
			 * Convert the data to an hexadecimal string
			 */
			std::string hex() const;

			/**
			 * Encode/Decode in base64
			 */
			std::string base64Encode() const;
			std::string base64Decode() const;

		private:
			size_t m_length;
			uint8_t* const m_pBuffer;
			const bool m_allocated;
		};

		template<>
		void Buffer::memcpy(const size_t position, Buffer& buffer);
		template<>
		void Buffer::memcpy(const size_t position, const Buffer& buffer);
	}
}
