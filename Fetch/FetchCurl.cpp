#include <curl/curl.h>
#include <sstream>
#include <cstring>

#include "FetchCurl.hpp"
#include "../Topic.hpp"
#include "../Logger.hpp"
#include "../Compiler.hpp"
#include "../Exception.hpp"

IRSTD_TOPIC_USE(IrStdFetch);

//#define CURL_DEBUG 1

static CURL* fetchCurlHelperInit()
{
	CURL* curl = ::curl_easy_init();
	IRSTD_THROW_ASSERT(IrStd::Topic::IrStdFetch, curl, "Unable to initialize libcurl");
	return curl;
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
	CURL* curl = fetchCurlHelperInit();
	char errorMsg[CURL_ERROR_SIZE];

	// Add debug functionalities
#if defined(CURL_DEBUG)
	::curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

	::curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	::curl_easy_setopt(curl, CURLOPT_URL, fetch.m_url);
	::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IrStd::FetchCurl::urlCallback);
	::curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_data);
	::curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorMsg);

	// Add POST data
	if (!fetch.m_post.empty())
	{
		auto res = ::curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fetch.m_post.c_str());
		IRSTD_THROW_ASSERT(IrStd::Topic::IrStdFetch, res == ::CURLE_OK, ::curl_easy_strerror(res));
	}

	// Add Headers
	if (!fetch.m_headerList.empty())
	{
		struct curl_slist *chunk = NULL;
		for (auto& header : fetch.m_headerList)
		{
			chunk = ::curl_slist_append(chunk, header.c_str());
		}
		auto res = ::curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		IRSTD_THROW_ASSERT(IrStd::Topic::IrStdFetch, res == ::CURLE_OK, ::curl_easy_strerror(res));
	}

	IRSTD_LOG_TRACE(IrStd::Topic::IrStdFetch, "Fetching data at url '" << fetch.m_url << "'");
	IRSTD_LOG_DEBUG(IrStd::Topic::IrStdFetch, "Debug information for '" << fetch.m_url
			<< "': post='" << fetch.m_post << "'");

	auto resPerform = ::curl_easy_perform(curl);
	::curl_easy_cleanup(curl);

	if (resPerform != ::CURLE_OK)
	{
		IRSTD_THROW_RETRY(IrStd::Topic::IrStdFetch, ::curl_easy_strerror(resPerform) << ": "
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
	CURL* curl = fetchCurlHelperInit();
	char* const output = ::curl_easy_escape(curl, str, strLength);

	IRSTD_THROW_ASSERT(IrStd::Topic::IrStdFetch, output, "Cannot escape the given string (str="
			<< str << ", length=" << length << ")");

	result.assign(output);
	::curl_free(output);
	::curl_easy_cleanup(curl);
}
