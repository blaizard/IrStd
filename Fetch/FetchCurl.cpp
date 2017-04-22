#include "FetchCurl.hpp"
#include "../Topic.hpp"
#include "../Logger.hpp"
#include "../Compiler.hpp"

#include <curl/curl.h>
#include <sstream>

IRSTD_TOPIC_USE(IrStdFetch);

//#define CURL_DEBUG 1

IrStd::FetchCurl::FetchCurl(std::string& data)
		: m_data(data)
{
}

size_t IrStd::FetchCurl::urlCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

IrStd::Fetch::Status IrStd::FetchCurl::url(const char* const url)
{
	CURL* curl = ::curl_easy_init();
	if (curl)
	{
		// Add debug functionalities
#if defined(CURL_DEBUG)
		::curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

		::curl_easy_setopt(curl, CURLOPT_URL, url);
		::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IrStd::FetchCurl::urlCallback);
		::curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_data);

		IRSTD_LOG_TRACE(IrStd::Topic::IrStdFetch, "Fetching data at url '" << url << "'");

		CURLcode res = ::curl_easy_perform(curl);
		::curl_easy_cleanup(curl);

		if (res == ::CURLE_OK)
		{
			return IrStd::Fetch::Status::OK;
		}

		IRSTD_LOG_ERROR(IrStd::Topic::IrStdFetch, ::curl_easy_strerror(res) << " (url=" << url << ")");
	}

	return IrStd::Fetch::Status::ERROR;
}