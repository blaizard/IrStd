#include <cstdlib>
#include <new>
#include <stdexcept>
#include <iomanip>

#include "../Memory.hpp"
#include "../Assert.hpp"
#include "../Logger.hpp"
#include "../Scope.hpp"

IRSTD_TOPIC_REGISTER(IrStdMemory);
IRSTD_SCOPE_THREAD_REGISTER(IrStdMemoryNoTrace);

// ----------------------------------------------------------------------------

/**
 * Overload new and delete operators to monitor the heap
 */
void* operator new(size_t size)
{
	void* ptr = IrStd::Memory::getInstance().newImpl(size);
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	return ptr;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	return IrStd::Memory::getInstance().newImpl(size);
}

void operator delete(void *ptr) noexcept
{
	IrStd::Memory::getInstance().deleteImpl(ptr);
}

// ----------------------------------------------------------------------------

IrStd::Memory::Memory()
		: m_allocStatPeak(0)
		, m_allocStatCurrent(0)
{
}

size_t IrStd::Memory::getStatCurrent() const noexcept
{
	return m_allocStatCurrent.load();
}

size_t IrStd::Memory::getStatPeak() const noexcept
{
	return m_allocStatPeak.load();
}

void* IrStd::Memory::newImpl(size_t size) noexcept
{
	void* ptr = std::malloc(size);

	if (ptr == nullptr)
	{
		IRSTD_LOG_ERROR(IrStd::Topic::IrStdMemory, "Memory allocation failed (size=" << size << ")");
		return nullptr;
	}

	// Record this entry
	{
		std::pair<void*, size_t> entry(ptr, size);
		m_mutexAlloxMap.lock();
		const auto ret = m_allocMap.insert(entry);
		m_mutexAlloxMap.unlock();
		IRSTD_ASSERT(IrStd::Topic::IrStdMemory, ret.second, "Entry " << static_cast<void*>(ptr) << " already exists");
		m_allocStatCurrent.fetch_add(size);
		m_allocStatPeak.store(std::max(getStatPeak(), getStatCurrent()));
	}

	{
		IRSTD_SCOPE_THREAD(scope, IrStdMemoryNoTrace);
		if (scope.isActivator())
		{
			IRSTD_LOG_TRACE(IrStd::Topic::IrStdMemory, "Allocated (size=" << std::setw(10) << size
					<< ", ptr=" << std::setw(18) << static_cast<void*>(ptr)
					<< ", current=" << std::setw(10)  << getStatCurrent()
					<< ", peak=" << std::setw(10)  << getStatPeak() << ")");
		}
	}

	return ptr;
}

void IrStd::Memory::deleteImpl(void* ptr) noexcept
{
	size_t size = 0;
	{
		// Update the records
		{
			m_mutexAlloxMap.lock();
			const auto it = m_allocMap.find(ptr);
			IRSTD_ASSERT(IrStd::Topic::IrStdMemory, it != m_allocMap.end(), "Unabe to find ptr " << static_cast<void*>(ptr));
			size = it->second;
			m_allocStatCurrent.fetch_sub(it->second);
			m_allocMap.erase(it);
			m_mutexAlloxMap.unlock();
		}

		std::free(ptr);
	}

	{
		IRSTD_SCOPE_THREAD(scope, IrStdMemoryNoTrace);
		if (scope.isActivator())
		{
			IRSTD_LOG_TRACE(IrStd::Topic::IrStdMemory, "Free      (size=" << std::setw(10) << size
					<< ", ptr=" << std::setw(18) << static_cast<void*>(ptr)
					<< ", current=" << std::setw(10)  << getStatCurrent()
					<< ", peak=" << std::setw(10) << getStatPeak() << ")");
		}
	}
}
