#pragma once

#include "Utils.hpp"
#include "Type.hpp"

namespace IrStd
{
	class Main : public SingletonScopeImpl<Main>
	{
	public:
		Main();
		~Main();
		int call(int (*mainFct)());
		int call(int (*mainFct)(int argc, char* argv[]), int argc, char* argv[]);

		/**
		 * \brief The version return gets the following format:
		 * Major.Minor.BuildDate.BuildTime
		 */
		void setVersion(const size_t major, const size_t minor = 0) noexcept;
		const std::string& getVersion() const noexcept;

		/**
		 * Return the current executable path
		 */
		static const char* getExecutablePath() noexcept;

	private:
		std::string m_version;
	};
}
