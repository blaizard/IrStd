#pragma once

#include <map>
#include <atomic>
#include <mutex>

#include "Allocator.hpp"
#include "Utils.hpp"
#include "Bootstrap.hpp"
#include "Assert.hpp"
#include "Type/Memory.hpp"

#define IRSTD_MEMORY_DUMP_STREAM() \
		IRSTD_MEMORY_STATISTICS_STREAM(IrStd::Memory::getInstance().getStatistics())

#define IRSTD_MEMORY_STATISTICS_STREAM(stats) \
		std::dec << "current=" << (stats).getStatCurrent() \
		<< ", peak=" << (stats).getStatPeak() \
		<< ", nb.new=" << (stats).getStatNbNew() \
		<< ", nb.delete=" << (stats).getStatNbDelete()

namespace IrStd
{
	class Memory : public SingletonImpl<Memory>
	{
	public:
		class Statistics
		{
		public:
			Statistics() noexcept
			{
				reset();
			}

			IrStd::Type::Memory getStatCurrent() const noexcept
			{
				const int64_t value = m_allocCurrent.load();
				const auto max = std::max(static_cast<int64_t>(0), value);
				return max;
			}
			IrStd::Type::Memory getStatPeak() const noexcept
			{
				const int64_t value = m_allocPeak.load();
				const auto max = std::max(static_cast<int64_t>(0), value);
				return max;
			}
			size_t getStatNbNew() const noexcept
			{
				const int64_t value = m_allocNbNew.load();
				const auto max = std::max(static_cast<int64_t>(0), value);
				return static_cast<size_t>(max);
			}
			size_t getStatNbDelete() const noexcept
			{
				const int64_t value = m_allocNbDelete.load();
				const auto max = std::max(static_cast<int64_t>(0), value);
				return static_cast<size_t>(max);
			}

			int64_t getStatCurrentRaw() const noexcept
			{
				return m_allocCurrent.load();
			}
			int64_t getStatPeakRaw() const noexcept
			{
				return m_allocPeak.load();
			}
			int64_t getStatNbNewRaw() const noexcept
			{
				return m_allocNbNew.load();
			}
			int64_t getStatNbDeleteRaw() const noexcept
			{
				return m_allocNbDelete.load();
			}

		private:
			friend Memory;

			void reset() noexcept
			{
				m_allocPeak.store(0);
				m_allocCurrent.store(0);
				m_allocNbNew.store(0);
				m_allocNbDelete.store(0);
			}

			std::atomic<int64_t> m_allocPeak;
			std::atomic<int64_t> m_allocCurrent;
			std::atomic<int64_t> m_allocNbNew;
			std::atomic<int64_t> m_allocNbDelete;
		};

		Statistics& getStatistics() const noexcept;

		IrStd::Type::Memory getStatCurrent() const noexcept;
		IrStd::Type::Memory getStatPeak() const noexcept;
		size_t getStatNbNew() const noexcept;
		size_t getStatNbDelete() const noexcept;

		/**
		 * Virtual memory information
		 */
		IrStd::Type::Memory getVirtualMemoryTotal() const noexcept;
		IrStd::Type::Memory getVirtualMemoryTotalUsed() const noexcept;
		IrStd::Type::Memory getVirtualMemoryCurrent() const noexcept;

		/**
		 * RAM memory information
		 */
		IrStd::Type::Memory getRAMTotal() const noexcept;
		IrStd::Type::Memory getRAMTotalUsed() const noexcept;
		IrStd::Type::Memory getRAMCurrent() const noexcept;

		/**
		 * Warnign this class is not thread safe.
		 * This is deliberate not to compilate/slow down the implementation.
		 */
		class StatisticsScope
		{
		public:
			StatisticsScope()
					: m_prevStatistics(nullptr)
			{
			}

			~StatisticsScope()
			{
				if (m_prevStatistics)
				{
					stopMonitoring();
				}
			}

			const Statistics& getStatistics() const noexcept
			{
				return m_localStatistics;
			}

			void startMonitoring() noexcept
			{
				IRSTD_ASSERT(m_prevStatistics == nullptr, "Monitoring is already activated");
				m_localStatistics.reset();
				m_prevStatistics = Memory::getInstance().m_pStatistics.exchange(&m_localStatistics);
			}

			void stopMonitoring() noexcept
			{
				IRSTD_ASSERT(m_prevStatistics, "Monitoring is not activated");

				Memory::getInstance().m_pStatistics.store(m_prevStatistics);
				m_prevStatistics = nullptr;

				// Update the statistics
				{
					auto& stats = Memory::getInstance().getStatistics();
					stats.m_allocCurrent.store(m_localStatistics.getStatCurrentRaw());
					stats.m_allocNbNew += m_localStatistics.getStatNbNewRaw();
					stats.m_allocNbDelete += m_localStatistics.getStatNbDeleteRaw();
					stats.m_allocPeak.store(std::max(stats.getStatPeakRaw(), m_localStatistics.getStatPeakRaw()));
				}
			}

		private:
			Statistics* m_prevStatistics;
			Statistics m_localStatistics;
		};

	private:
		friend void* ::operator new(size_t);
		friend void* ::operator new(size_t, const std::nothrow_t&) noexcept;
		friend void ::operator delete(void* ptr);
		friend SingletonImpl<Memory>;
		friend Bootstrap;
		friend StatisticsScope;

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

		Statistics m_statistics;
		std::atomic<Statistics*> m_pStatistics;

		static bool m_enable;
	};
}
