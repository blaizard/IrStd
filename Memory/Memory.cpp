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

IRSTD_TOPIC_REGISTER(IrStdMemory);
IRSTD_SCOPE_LOCALTHREAD_REGISTER(IrStdMemoryNoTrace);

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
		: m_allocStatPeak(0)
		, m_allocStatCurrent(0)
		, m_allocStatNbNew(0)
		, m_allocStatNbDelete(0)
{
}

// ---- Statistics related  ---------------------------------------------------

void IrStd::Memory::getStatistics(Statistics& stats) const noexcept
{
	// TODO: make this function atomic
	stats.m_allocStatPeak = getStatPeak();
	stats.m_allocStatCurrent = getStatCurrent();
	stats.m_allocStatNbNew = getStatNbNew();
	stats.m_allocStatNbDelete = getStatNbDelete();
}

size_t IrStd::Memory::getStatCurrent() const noexcept
{
	return m_allocStatCurrent.load();
}

size_t IrStd::Memory::getStatPeak() const noexcept
{
	return m_allocStatPeak.load();
}

size_t IrStd::Memory::getStatNbNew() const noexcept
{
	return m_allocStatNbNew.load();
}

size_t IrStd::Memory::getStatNbDelete() const noexcept
{
	return m_allocStatNbDelete.load();
}

void IrStd::Memory::disable() noexcept
{
	m_enable = false;
#if IRSTD_IS_DEBUG
	IRSTD_LOG_INFO(IrStd::Topic::IrStdMemory, "Disabled memory manager: " << IRSTD_MEMORY_DUMP_STREAM());
#endif
}

// ---- IrStd::Memory::newImpl ------------------------------------------------

void* IrStd::Memory::newImpl(size_t size) noexcept
{
	void* ptr = std::malloc(size);

	if (ptr == nullptr)
	{
		IRSTD_LOG_ERROR(IrStd::Topic::IrStdMemory, "Memory allocation failed (size=" << size << ")");
		return nullptr;
	}

#if IRSTD_IS_DEBUG
	// Record this entry
	{
		IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);
		std::pair<void*, size_t> entry(ptr, size);
		{
			std::lock_guard<std::mutex> lock(m_mutexAllocMap);
			const auto ret = m_allocMap.insert(entry);
			IRSTD_ASSERT(IrStd::Topic::IrStdMemory, ret.second, "Entry " << static_cast<void*>(ptr) << " already exists");
		}
		m_allocStatCurrent.fetch_add(size);
		m_allocStatPeak.store(std::max(getStatPeak(), getStatCurrent()));
		m_allocStatNbNew++;
	}

#if defined(IRSTDMEMORY_DEBUG)
	{
		IRSTD_SCOPE(scope, IrStd::Flag::IrStdMemoryNoTrace);
		if (scope.isActivator())
		{
			IRSTD_LOG_TRACE(IrStd::Topic::IrStdMemory, "Allocated (size=" << size
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
#if IRSTD_IS_DEBUG
	size_t size = 0;
	// Update the records
	{
		IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);
		{
			std::lock_guard<std::mutex> lock(m_mutexAllocMap);
			const auto it = m_allocMap.find(ptr);
			IRSTD_ASSERT(IrStd::Topic::IrStdMemory, it != m_allocMap.end(),
					"Unable to find ptr " << static_cast<void*>(ptr));
			size = it->second;
			m_allocStatCurrent.fetch_sub(it->second);
			m_allocMap.erase(it);
		}
		m_allocStatNbDelete++;
	}
#endif

	std::free(ptr);

#if IRSTD_IS_DEBUG && defined(IRSTDMEMORY_DEBUG)
	{
		IRSTD_SCOPE(scope, IrStd::Flag::IrStdMemoryNoTrace);
		if (scope.isActivator())
		{
			IRSTD_LOG_TRACE(IrStd::Topic::IrStdMemory, "Free      (size=" << size
					<< ", ptr=" << static_cast<void*>(ptr)
					<< ", " << IRSTD_MEMORY_DUMP_STREAM() << ")");
		}
	}
#endif
}
#pragma GCC diagnostic pop
