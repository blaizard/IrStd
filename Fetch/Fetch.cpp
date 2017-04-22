#include "../Fetch.hpp"
#include "../Logger.hpp"

#include "FetchCurl.hpp"

IRSTD_TOPIC_REGISTER(IrStdFetch);

IrStd::Fetch::Fetch(std::string& data)
		: m_data(data)
{
}

std::future<IrStd::Fetch::Status> IrStd::Fetch::url(const char* const url)
{
	FetchCurl curl(m_data);
	return std::async(std::launch::async, &IrStd::FetchCurl::url, curl, url); 
}
