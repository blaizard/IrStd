#pragma once

#include <memory>
#include <string>

#include "Type/Buffer.hpp"

namespace IrStd
{
	/**
	 * \defgroup IrStd-Crypto
	 * \brief Helper function to deal with crypto algorithms
	 * \ingroup IrStd
	 */

	class Crypto
	{
	public:

		/**
		 * \brief Initialize the crypto object with a buffer to process
		 * \ingroup IrStd-Crypto
		 */
		template<class ... Args>
		Crypto(Args&& ... args)
				: m_buffer(std::forward<Args>(args)...)
		{
		}

		/**
		 * \brief Generates a SHA-256 hash
		 * \ingroup IrStd-Crypto
		 * \return The buffer containing the hash
		 */
		std::unique_ptr<IrStd::Type::Buffer> SHA256() const;

		/**
		 * \brief Cypher the internal buffer with the HMAC-SHA-1 algorithm
		 * \ingroup IrStd-Crypto
		 * \param key The key to be used
		 * \return The buffer containing the cyphered message
		 */
		std::unique_ptr<IrStd::Type::Buffer> HMACSHA1(const IrStd::Type::Buffer& key) const;


		/**
		 * \brief Cypher the internal buffer with the HMAC-SHA-512 algorithm
		 * \ingroup IrStd-Crypto
		 * \param key The key to be used
		 * \return The buffer containing the cyphered message
		 */
		std::unique_ptr<IrStd::Type::Buffer> HMACSHA512(const IrStd::Type::Buffer& key) const;

	private:
		const IrStd::Type::Buffer m_buffer;
	};

}
