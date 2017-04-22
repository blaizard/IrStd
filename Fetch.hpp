#pragma once

#include <string>
#include <future>

namespace IrStd
{
	class Fetch
	{
	public:
		enum class Status
		{
			OK = 0,
			ERROR
		};

		/**
		 * \param data A buffer where to store the data fetch
		 */
		Fetch(std::string& data);

		/**
		 * Fetch data from an URL
		 */
		std::future<Status> url(const char* const url);

	private:
		bool urlSync(const char* const url);

		std::string& m_data;
	};
}
