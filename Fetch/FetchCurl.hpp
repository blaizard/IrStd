#pragma once

#include "../Fetch.hpp"

#include <string>

namespace IrStd
{
	class FetchCurl
	{
	public:
		FetchCurl(std::string& data);

		Fetch::Status url(const IrStd::FetchUrl& fetch);

		/**
		 * Encode the given string 
		 */
		static void urlEncode(std::string& result, const char* const str, const size_t length = 0);

	private:
		static size_t urlCallback(void *contents, size_t size, size_t nmemb, void *userp);
		std::string& m_data;
	};
}