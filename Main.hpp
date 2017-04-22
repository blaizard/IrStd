#pragma once

#include "Utils.hpp"

namespace IrStd
{
	class Main : public SingletonScopeImpl<Main>
	{
	public:
		Main();
		~Main();
		int call(int (*mainFct)());
		int call(int (*mainFct)(int argc, char* argv[]), int argc, char* argv[]);
	};
}
