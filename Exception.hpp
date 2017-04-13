#pragma once

namespace IrStd
{
	class Exception
	{
	public:
		static void callStack(std::ostream& out, const size_t skipFirstNb = 1) noexcept;
	};
}
