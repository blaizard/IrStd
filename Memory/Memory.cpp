#include <cstdlib>
#include <new>
#include <stdexcept>
#include <iomanip>

#include "../Main.hpp"
#include "../Memory.hpp"
#include "../Assert.hpp"
#include "../Logger.hpp"
#include "../Scope.hpp"
#include "../Compiler.hpp"
#include "../Utils.hpp"

#if IRSTD_IS_PLATFORM(LINUX)
	#include <sys/types.h>
	#include <sys/sysinfo.h>
#endif

IRSTD_TOPIC_REGISTER(IrStd, Memory);
IRSTD_TOPIC_USE_ALIAS(IrStdMemory, IrStd, Memory);
IRSTD_SCOPE_LOCALTHREAD_REGISTER(IrStdMemoryNoTrace);

#define IS_MEMORY_MONITOR (defined(IRSTD_IS_DEBUG) || (defined(IRSTD_MEMORY_MONITOR) && IRSTD_MEMORY_MONITOR))

//#define IRSTDMEMORY_DEBUG 1

// ----------------------------------------------------------------------------

/**
 * Overload new and delete operators to monitor the heap
 */
void* operator new(size_t size)
{
	void* ptr = ::operator new(size, std::nothrow_t());
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	return ptr;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	return (IrStd::Main::isAlive() && IrStd::Memory::m_enable) ?
			IrStd::Memory::getInstance().newImpl(size) : std::malloc(size);
}

void operator delete(void* ptr) noexcept
{
	(IrStd::Main::isAlive() && ptr && IrStd::Memory::m_enable) ?
			IrStd::Memory::getInstance().deleteImpl(ptr) : std::free(ptr);
}

// ----------------------------------------------------------------------------

bool IrStd::Memory::m_enable = true;

IrStd::Memory::Memory()
		: m_statistics()
		, m_pStatistics(&m_statistics)
{
}


// ---- Statistics related  ---------------------------------------------------

IrStd::Memory::Statistics& IrStd::Memory::getStatistics() const noexcept
{
	return *m_pStatistics.load();
}

IrStd::Type::Memory IrStd::Memory::getStatCurrent() const noexcept
{
	return getStatistics().getStatCurrent();
}

IrStd::Type::Memory IrStd::Memory::getStatPeak() const noexcept
{
	return getStatistics().getStatPeak();
}

size_t IrStd::Memory::getStatNbNew() const noexcept
{
	return getStatistics().getStatNbNew();
}

size_t IrStd::Memory::getStatNbDelete() const noexcept
{
	return getStatistics().getStatNbDelete();
}

void IrStd::Memory::disable() noexcept
{
	m_enable = false;
#if IS_MEMORY_MONITOR
	IRSTD_LOG_INFO(IrStdMemory, "Disabled memory manager: " << IRSTD_MEMORY_DUMP_STREAM());
#endif
}

// ---- IrStd::Memory::newImpl ------------------------------------------------

void* IrStd::Memory::newImpl(size_t size) noexcept
{
	void* ptr = std::malloc(size);

	if (ptr == nullptr)
	{
		IRSTD_LOG_ERROR(IrStdMemory, "Memory allocation failed (" << IrStd::Type::Memory(size) << " or " << size << ")");
		return nullptr;
	}

#if IS_MEMORY_MONITOR
	// Record this entry
	{
		IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);
		std::pair<void*, size_t> entry(ptr, size);
		{
			std::lock_guard<std::mutex> lock(m_mutexAllocMap);
			const auto ret = m_allocMap.insert(entry);
			IRSTD_ASSERT(IrStdMemory, ret.second, "Entry " << static_cast<void*>(ptr) << " already exists");
		}
		{
			auto& stats = getStatistics();
			stats.m_allocCurrent.fetch_add(size);
			stats.m_allocPeak.store(std::max(stats.getStatPeakRaw(), stats.getStatCurrentRaw()));
			stats.m_allocNbNew++;
		}
	}

#if IS_MEMORY_MONITOR && defined(IRSTDMEMORY_DEBUG)
	{
		IRSTD_SCOPE(scope, IrStd::Flag::IrStdMemoryNoTrace);
		if (scope.isActivator())
		{
			IRSTD_LOG_TRACE(IrStdMemory, "Allocated (size=" << size
					<< " or " << IrStd::Type::Memory(size)
					<< ", ptr=" << static_cast<void*>(ptr)
					<< ", " << IRSTD_MEMORY_DUMP_STREAM() << ")");
		}
	}
#endif
#endif

	return ptr;
}

// ---- IrStd::Memory::deleteImpl ---------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void IrStd::Memory::deleteImpl(void* ptr) noexcept
{
#if IS_MEMORY_MONITOR
	size_t size = 0;
	// Update the records
	{
		IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);
		auto& stats = getStatistics();
		{
			std::lock_guard<std::mutex> lock(m_mutexAllocMap);
			const auto it = m_allocMap.find(ptr);
			IRSTD_ASSERT(IrStdMemory, it != m_allocMap.end(),
					"Unable to find ptr " << static_cast<void*>(ptr));
			size = it->second;
			stats.m_allocCurrent.fetch_sub(it->second);
			m_allocMap.erase(it);
		}
		stats.m_allocNbDelete++;
	}
#endif

	std::free(ptr);

#if IS_MEMORY_MONITOR && defined(IRSTDMEMORY_DEBUG)
	{
		IRSTD_SCOPE(scope, IrStd::Flag::IrStdMemoryNoTrace);
		if (scope.isActivator())
		{
			IRSTD_LOG_TRACE(IrStdMemory, "Free      (size=" << size
					<< " or " << IrStd::Type::Memory(size)
					<< ", ptr=" << static_cast<void*>(ptr)
					<< ", " << IRSTD_MEMORY_DUMP_STREAM() << ")");
		}
	}
#endif
}
#pragma GCC diagnostic pop

#if IRSTD_IS_PLATFORM(LINUX)

// ---- IrStd::Memory::getVirtualMemory* --------------------------------------

IrStd::Type::Memory IrStd::Memory::getVirtualMemoryTotal() const noexcept
{
	struct ::sysinfo memInfo;
	::sysinfo(&memInfo);
	IrStd::Type::Memory totalVirtualMem = memInfo.totalram;

	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;

	return totalVirtualMem;
}

IrStd::Type::Memory IrStd::Memory::getVirtualMemoryTotalUsed() const noexcept
{
	struct ::sysinfo memInfo;
	::sysinfo(&memInfo);
	IrStd::Type::Memory virtualMemUsed = memInfo.totalram - memInfo.freeram;

	virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
	virtualMemUsed *= memInfo.mem_unit;

	return virtualMemUsed;
}

IrStd::Type::Memory IrStd::Memory::getVirtualMemoryCurrent() const noexcept
{
	FILE* file = ::fopen("/proc/self/status", "r");
	IrStd::Type::Memory result = 0;
	char line[128];
	if (file)
	{
		while (::fgets(line, 128, file) != NULL)
		{
			if (::strncmp(line, "VmSize:", 7) == 0)
			{
				const char* p = line;
				while (*p <'0' || *p > '9') p++;
				line[::strlen(line) - 3] = '\0';
				result = ::atoi(p);
				break;
			}
		}
		::fclose(file);
	}
	return result;
}

// ---- IrStd::Memory::getRAM* ------------------------------------------------

IrStd::Type::Memory IrStd::Memory::getRAMTotal() const noexcept
{
	struct ::sysinfo memInfo;
	::sysinfo(&memInfo);
	IrStd::Type::Memory totalVirtualMem = memInfo.totalram;

	totalVirtualMem *= memInfo.mem_unit;

	return totalVirtualMem;
}

IrStd::Type::Memory IrStd::Memory::getRAMTotalUsed() const noexcept
{
	struct ::sysinfo memInfo;
	::sysinfo(&memInfo);
	IrStd::Type::Memory virtualMemUsed = memInfo.totalram - memInfo.freeram;

	virtualMemUsed *= memInfo.mem_unit;

	return virtualMemUsed;
}

IrStd::Type::Memory IrStd::Memory::getRAMCurrent() const noexcept
{
	FILE* file = ::fopen("/proc/self/status", "r");
	IrStd::Type::Memory result = 0;
	char line[128];
	if (file)
	{
		while (::fgets(line, 128, file) != NULL)
		{
			if (::strncmp(line, "VmRSS:", 6) == 0)
			{
				const char* p = line;
				while (*p <'0' || *p > '9') p++;
				line[::strlen(line) - 3] = '\0';
				result = ::atoi(p);
				break;
			}
		}
		::fclose(file);
	}
	return result;
}
#endif
