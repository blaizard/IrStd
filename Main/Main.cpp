#include <unistd.h>

#include "../Bootstrap.hpp"
#include "../Logger.hpp"
#include "../Main.hpp"
#include "../Memory.hpp"
#include "../Scope.hpp"
#include "../Topic.hpp"
#include "../Compiler.hpp"

IRSTD_TOPIC_REGISTER(IrStd);
IRSTD_TOPIC_REGISTER(IrStd, Main);
IRSTD_TOPIC_USE_ALIAS(IrStdMain, IrStd, Main);

IrStd::Main::Main()
		: SingletonScopeImpl<Main>()
{
	IrStd::Bootstrap::init();
	IrStd::Memory::getInstance();
	IRSTD_LOG_TRACE(IrStdMain, "Main initialized");
}

int IrStd::Main::call(int (*mainFct)())
{
	return mainFct();
}

int IrStd::Main::call(int (*mainFct)(int argc, char* argv[]), int argc, char* argv[])
{
	return mainFct(argc, argv);
}

IrStd::Main::~Main()
{
#if IRSTD_IS_DEBUG
	std::cout << "Memory allocation statistics: " << IRSTD_MEMORY_DUMP_STREAM() << std::endl;
#endif
}

const char* IrStd::Main::getExecutablePath() noexcept
{
	static char path[4096] = {'\0'};

	// Construct the path only once
	if (path[0] == '\0')
	{
#if IRSTD_IS_PLATFORM(LINUX)
		const ssize_t retVal = ::readlink("/proc/self/exe", path, sizeof(path) - 1);
		IRSTD_ASSERT(IrStdMain, retVal > 0, "::readlink returned " << retVal);
		path[retVal] = '\0';
#elif IRSTD_IS_PLATFORM(WINDOWS)
		::GetModuleFileName(nullptr, path, sizeof(path));
#else
	IRSTD_STATIC_ERROR("Platform not supported")
#endif
	}

	return path;
}

void IrStd::Main::setVersion(const size_t major, const size_t minor) noexcept
{
	IRSTD_ASSERT(IrStdMain, m_version.empty(), "A version (" << m_version << ") has laready been set");
	m_version = std::to_string(major) + "." + std::to_string(minor) + "." + IrStd::Compiler::getBuildVersion();
}

const std::string& IrStd::Main::getVersion() const noexcept
{
	IRSTD_ASSERT(IrStdMain, !m_version.empty(), "No version has been defined");
	return m_version;
}
