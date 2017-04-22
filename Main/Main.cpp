#include "../Bootstrap.hpp"
#include "../Logger.hpp"
#include "../Main.hpp"
#include "../Memory.hpp"
#include "../Scope.hpp"
#include "../Topic.hpp"
#include "../Compiler.hpp"

IRSTD_TOPIC_REGISTER(IrStdMain);

IrStd::Main::Main()
		: SingletonScopeImpl<Main>()
{
	IrStd::Bootstrap::init();
	IrStd::Memory::getInstance();
	IRSTD_LOG_TRACE(IrStd::Topic::IrStdMain, "Main initialized");
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