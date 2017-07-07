#pragma once

#include <map>
#include <atomic>
#include <mutex>

#include "Allocator.hpp"
#include "Utils.hpp"
#include "Bootstrap.hpp"

#define IRSTD_MEMORY_DUMP_STREAM() \
		std::dec << "current=" << IrStd::Memory::getInstance().getStatCurrent() \
		<< ", peak=" << IrStd::Memory::getInstance().getStatPeak() \
		<< ", nb.new=" << IrStd::Memory::getInstance().getStatNbNew() \
		<< ", nb.delete=" << IrStd::Memory::getInstance().getStatNbDelete()

#define IRSTD_MEMORY_STATISTICS_STREAM(stats) \
		std::dec << "current=" << stats.m_allocStatCurrent \
		<< ", peak=" << stats.m_allocStatPeak \
		<< ", nb.new=" << stats.m_allocStatNbNew \
		<< ", nb.delete=" << stats.m_allocStatNbDelete

namespace IrStd
{
	class Memory : public SingletonImpl<Memory>
	{
	public:

		struct Statistics
		{
			Statistics& operator-=(const Statistics& rhs)
			{
				m_allocStatCurrent -= rhs.m_allocStatCurrent;
				m_allocStatNbNew -= rhs.m_allocStatNbNew;
				m_allocStatNbDelete -= rhs.m_allocStatNbDelete;
				return *this;
			}

			size_t m_allocStatPeak;
			size_t m_allocStatCurrent;
			size_t m_allocStatNbNew;
			size_t m_allocStatNbDelete;
		};

		void getStatistics(Statistics& stats) const noexcept;

		size_t getStatCurrent() const noexcept;
		size_t getStatPeak() const noexcept;
		size_t getStatNbNew() const noexcept;
		size_t getStatNbDelete() const noexcept;

	private:
		friend void* ::operator new(size_t);
		friend void* ::operator new(size_t, const std::nothrow_t&) noexcept;
		friend void ::operator delete(void* ptr);
		friend SingletonImpl<Memory>;
		friend Bootstrap;

		Memory();

		/**
		 * Disable the memory allocator. This cannot be undone,
		 * and should be used in critical situation only.
		 */
		static void disable() noexcept;

		void* newImpl(size_t size) noexcept;
		void deleteImpl(void* ptr) noexcept;

		template<typename Key, typename T>
		using MapAllocBypass = std::map<Key, T, std::less<Key>, IrStd::AllocatorObj<std::pair<const Key, T>, AllocatorRaw>>;
		MapAllocBypass<void*, size_t> m_allocMap;

		std::mutex m_mutexAllocMap;
		std::atomic<size_t> m_allocStatPeak;
		std::atomic<size_t> m_allocStatCurrent;
		std::atomic<size_t> m_allocStatNbNew;
		std::atomic<size_t> m_allocStatNbDelete;
		static bool m_enable;
	};
}
