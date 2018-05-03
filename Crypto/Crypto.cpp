#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <cstring>

#include "../Assert.hpp"
#include "../Crypto.hpp"

namespace
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	std::unique_ptr<IrStd::Type::Buffer> HelperHMAC(
			const ::EVP_MD *md,
			const size_t allocMem,
			const char* const pKey,
			const size_t length,
			const uint8_t* const pData,
			const size_t dataLength)
	{
		::HMAC_CTX ctx;
		::HMAC_CTX_init(&ctx);

		const int keyLength = static_cast<const int>((length) ? length : strlen(pKey));
		auto pResult = std::unique_ptr<IrStd::Type::Buffer>(new IrStd::Type::Buffer(allocMem));
		unsigned int resultLength;

		::HMAC_Init_ex(&ctx, pKey, keyLength, md, nullptr);
		::HMAC_Update(&ctx, pData, static_cast<int>(dataLength));
		::HMAC_Final(&ctx, pResult->getForWrite<unsigned char>(), &resultLength);
		::HMAC_CTX_cleanup(&ctx);

		pResult->setSize(resultLength);

		return std::move(pResult);
	}
#else
	std::unique_ptr<IrStd::Type::Buffer> HelperHMAC(
			const ::EVP_MD *md,
			const size_t allocMem,
			const char* const pKey,
			const size_t length,
			const uint8_t* const pData,
			const size_t dataLength)
	{
		::HMAC_CTX* ctx;
		ctx = ::HMAC_CTX_new();

		const int keyLength = static_cast<const int>((length) ? length : strlen(pKey));
		auto pResult = std::unique_ptr<IrStd::Type::Buffer>(new IrStd::Type::Buffer(allocMem));
		unsigned int resultLength;

		::HMAC_Init_ex(ctx, pKey, keyLength, md, nullptr);
		::HMAC_Update(ctx, pData, static_cast<int>(dataLength));
		::HMAC_Final(ctx, pResult->getForWrite<unsigned char>(), &resultLength);

		::HMAC_CTX_free(ctx);

		pResult->setSize(resultLength);

		return std::move(pResult);
	}
#endif
}

std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::HMACSHA1(const IrStd::Type::Buffer& key) const
{
	return std::move(::HelperHMAC(EVP_sha1(), 20, key.get<char>(), key.size(), m_buffer.get<uint8_t>(), m_buffer.size()));
}

std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::HMACSHA512(const IrStd::Type::Buffer& key) const
{
	return std::move(::HelperHMAC(EVP_sha512(), 512 / 8, key.get<char>(), key.size(), m_buffer.get<uint8_t>(), m_buffer.size()));
}

std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::SHA256() const
{
	auto pResult = std::unique_ptr<IrStd::Type::Buffer>(new IrStd::Type::Buffer(SHA256_DIGEST_LENGTH));

	::SHA256_CTX ctx;
    ::SHA256_Init(&ctx);
    ::SHA256_Update(&ctx, m_buffer.get<uint8_t>(), m_buffer.size());
    ::SHA256_Final(pResult->getForWrite<unsigned char>(), &ctx);

	return std::move(pResult); 
}
