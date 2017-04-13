#pragma once

#include <map>
#include <atomic>
#include <mutex>

#include "Allocator.hpp"
#include "Utils.hpp"

namespace IrStd
{
	class Memory : public SingletonImpl<Memory>
	{
	public:

		size_t getStatCurrent() const noexcept;
		size_t getStatPeak() const noexcept;

	private:
		friend void* ::operator new(size_t);
		friend void* ::operator new(size_t, const std::nothrow_t&) noexcept;
		friend void ::operator delete(void *ptr); 
		friend SingletonImpl<Memory>;

		Memory();

		void* newImpl(size_t size) noexcept;
		void deleteImpl(void* ptr) noexcept;

		template<typename Key, typename T>
		using MapAllocBypass = std::map<Key, T, std::less<Key>, IrStd::AllocatorObj<std::pair<const Key, T>, AllocatorRaw>>;

		MapAllocBypass<void*, size_t> m_allocMap;
		std::mutex m_mutexAlloxMap;
		std::atomic<size_t> m_allocStatPeak;
		std::atomic<size_t> m_allocStatCurrent;
	};
}
