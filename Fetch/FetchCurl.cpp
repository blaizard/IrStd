#define USE_OPENSSL

#include <curl/curl.h>
#include <openssl/crypto.h>
#include <sstream>
#include <cstring>
#include <deque>
#include <mutex>

#include "FetchCurl.hpp"
#include "../Topic.hpp"
#include "../Logger.hpp"
#include "../Compiler.hpp"
#include "../Exception.hpp"

IRSTD_TOPIC_USE_ALIAS(IrStdFetch, IrStd, Fetch);

//#define CURL_DEBUG 1

namespace
{
	/**
	 * Implement lock mechnaism for openssl lib, as described here:
	 * https://curl.haxx.se/libcurl/c/libcurl-tutorial.html#Multi-threading
	 * https://curl.haxx.se/libcurl/c/threaded-ssl.html
	 */
	class Curl : public IrStd::SingletonImpl<Curl>
	{
	public:
		Curl()
				: m_mutexList(CRYPTO_num_locks())
		{
			::curl_global_init(CURL_GLOBAL_ALL);
			CRYPTO_set_id_callback(threadId);
			CRYPTO_set_locking_callback(lockCallback);
		}

		~Curl()
		{
			CRYPTO_set_id_callback(nullptr);
			CRYPTO_set_locking_callback(nullptr);
			::curl_global_cleanup();
		}

		class CurlSafe
		{
		public:
			CurlSafe()
			{
				m_pCurl = ::curl_easy_init();
				IRSTD_THROW_ASSERT(IrStdFetch, m_pCurl, "Unable to initialize libcurl");
			}

			~CurlSafe()
			{
				::curl_easy_cleanup(m_pCurl);
			}

			::CURL* get() const noexcept
			{
				return m_pCurl;
			}

		private:
			::CURL* m_pCurl = nullptr;
		};

		CurlSafe get()
		{
			return CurlSafe();
		}

	private:
		static unsigned long threadId(void)
		{
			return static_cast<unsigned long>(pthread_self());
		}

		static void lockCallback(int mode, int type, const char*/*file*/, int /*line*/)
		{
			if (mode & CRYPTO_LOCK)
			{
				getInstance().m_mutexList[type].lock();
			}
			else
			{
				getInstance().m_mutexList[type].unlock();
			}
		}

		std::deque<std::mutex> m_mutexList;
	};
}

IrStd::FetchCurl::FetchCurl(std::string& data)
		: m_data(data)
{
	m_data.clear();
}

size_t IrStd::FetchCurl::urlCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}

IrStd::Fetch::Status IrStd::FetchCurl::url(const IrStd::FetchUrl& fetch)
{
	auto curl = ::Curl::getInstance().get();
	char errorMsg[CURL_ERROR_SIZE];

	// Add debug functionalities
#if defined(CURL_DEBUG)
	::curl_easy_setopt(curl.get(), CURLOPT_HEADER, 1L);
	::curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, 1L);
#endif

	::curl_easy_setopt(curl.get(), CURLOPT_FAILONERROR, 1L);
	::curl_easy_setopt(curl.get(), CURLOPT_URL, fetch.m_url.c_str());
	::curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, IrStd::FetchCurl::urlCallback);
	::curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &m_data);
	::curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, errorMsg);
	::curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 20L);

	// Add POST data
	if (!fetch.m_post.empty())
	{
		auto res = ::curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, fetch.m_post.c_str());
		IRSTD_THROW_ASSERT(IrStdFetch, res == ::CURLE_OK, ::curl_easy_strerror(res));
	}

	// Add Headers
	if (!fetch.m_headerList.empty())
	{
		struct curl_slist *chunk = NULL;
		for (auto& header : fetch.m_headerList)
		{
			chunk = ::curl_slist_append(chunk, header.c_str());
		}
		auto res = ::curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, chunk);
		IRSTD_THROW_ASSERT(IrStdFetch, res == ::CURLE_OK, ::curl_easy_strerror(res));
	}

	IRSTD_LOG_TRACE(IrStdFetch, "Fetching data at url '" << fetch.m_url << "': post='"
			<< fetch.m_post << "'");

	const auto resPerform = ::curl_easy_perform(curl.get());

	if (resPerform != ::CURLE_OK)
	{
		IRSTD_THROW_RETRY(IrStdFetch, ::curl_easy_strerror(resPerform) << ": "
				<< errorMsg << " (url=" << fetch.m_url << ")");
	}

	return Fetch::Status::OK;
}

void IrStd::FetchCurl::urlEncode(
		std::string& result,
		const char* const str,
		const size_t length)
{
	const int strLength = static_cast<const int>((length) ? length : strlen(str));
	auto curl = ::Curl::getInstance().get();
	char* const output = ::curl_easy_escape(curl.get(), str, strLength);

	IRSTD_THROW_ASSERT(IrStdFetch, output, "Cannot escape the given string (str="
			<< str << ", length=" << length << ")");

	result.assign(output);
	::curl_free(output);
}
