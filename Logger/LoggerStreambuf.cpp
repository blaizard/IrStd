#include <mutex>

#include "LoggerStreambuf.hpp"

static std::mutex mtx;

void IrStd::loggerGetLock()
{
	mtx.lock();
}

void IrStd::loggerReleaseLock()
{
	mtx.unlock();
}
