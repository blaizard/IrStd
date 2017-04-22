#pragma once

#include "../Fetch.hpp"

#include <string>

namespace IrStd
{
	class FetchCurl
	{
	public:
		FetchCurl(std::string& data);

		IrStd::Fetch::Status url(const char* const url);
	private:
		static size_t urlCallback(void *contents, size_t size, size_t nmemb, void *userp);

		std::string& m_data;
	};
}