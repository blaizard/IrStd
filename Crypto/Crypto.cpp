#include <openssl/hmac.h>
#include <cstring>

#include "../Assert.hpp"
#include "../Crypto.hpp"

IrStd::Crypto::Crypto(const char* const pString)
		: Crypto(reinterpret_cast<const uint8_t*>(pString), strlen(pString))
{
}

IrStd::Crypto::Crypto(const uint8_t* const pData, const size_t length)
		: m_pData(pData)
		, m_length(length)
{
}

static std::unique_ptr<IrStd::Data> HMAC(
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
	::HMAC_Init(&ctx, pKey, keyLength, md);

	::HMAC_Update(&ctx, pData, static_cast<int>(dataLength));

	auto pResult = std::move(std::unique_ptr<IrStd::Data>(new IrStd::Data(allocMem)));

	unsigned int resultLength;
	HMAC_Final(&ctx, pResult->get(), &resultLength);
	HMAC_CTX_cleanup(&ctx);

	pResult->setLength(resultLength);

	return std::move(pResult);
}

std::unique_ptr<IrStd::Data> IrStd::Crypto::HMACSHA1(const char* const key, const size_t length)
{
	return std::move(HMAC(EVP_sha1(), 20, key, length, m_pData, m_length));
}

std::unique_ptr<IrStd::Data> IrStd::Crypto::HMACSHA512(const char* const key, const size_t length)
{
	return std::move(HMAC(EVP_sha512(), 512 / 8, key, length, m_pData, m_length));
}