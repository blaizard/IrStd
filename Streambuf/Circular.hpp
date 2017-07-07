#pragma once

#include <iostream>
#include <iomanip>
#include <streambuf>
#include <memory>
#include <cstring>

namespace IrStd
{
	namespace Streambuf
	{
		template <class char_type, class traits = std::char_traits<char_type> >
		class BasicCircular: public std::basic_streambuf<char_type, traits>
		{
		public:
			explicit BasicCircular(const size_t bufferLength)
					: m_bufferLength(bufferLength)
					, m_pBuffer(std::unique_ptr<char_type[]>(new char_type[bufferLength + 1]))
					, m_isWrapped(false)
			{
				std::memset(m_pBuffer.get(), 0, (bufferLength + 1) * sizeof(char_type));
				// Assign the buffer, keep the last character for eof
				this->setp(m_pBuffer.get(), m_pBuffer.get() + bufferLength);
			}

			/**
			 * Dump the internal buffer content for debugging purpose
			 */
			void dump(std::ostream& out) const
			{
				const char_type* pBuffer = static_cast<char_type*>(m_pBuffer.get());
				out << "dump=";
				for (size_t i = 0; i<m_bufferLength; ++i)
				{
					out << ((i) ? " " : "") << std::hex << std::setw(2) << std::setfill('0')
							<< static_cast<int>(pBuffer[i]);
				}
				out << ", ptr=" << (this->pptr() - this->pbase()) << std::endl;
			}

			/**
			 * Print the content to a stream
			 */
			void toStream(std::ostream& out) const
			{
				*static_cast<char_type*>(this->pptr()) = 0;
				if (m_isWrapped)
				{
					out << (static_cast<char_type*>(this->pptr()) + 1);
				}
				out << this->pbase();
			}

		protected:
			typedef typename traits::int_type int_type;

			virtual int_type overflow(int_type c)
			{
				if (traits::eq_int_type(c, traits::eof()))
				{
					return traits::not_eof(c);
				}

				// Wrap the pointer
				this->pbump(-static_cast<int>(m_bufferLength));
				m_isWrapped = true;

				// Print the new character
				{
					const char_type ch = traits::to_char_type(c);
					this->sputc(ch);
				}

				return c;
			}

			const size_t m_bufferLength;
			const std::unique_ptr<char_type[]> m_pBuffer;
			bool m_isWrapped;
		};

		typedef BasicCircular<char> Circular;
	}
}
