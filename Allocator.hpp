#pragma once

#include <memory>
#include <limits>
#include <iostream>

namespace IrStd
{
	class Allocator
	{
	public:
		virtual void* allocate(size_t size) noexcept = 0;
		virtual void deallocate(void* ptr) = 0;
	};

	class AllocatorStd : public Allocator
	{
	public:
		void* allocate(size_t size) noexcept
		{
			return ::operator new(size, std::nothrow);
		}

		void deallocate(void* ptr)
		{
			::operator delete(ptr);
		}
	};

	class AllocatorRaw : public Allocator
	{
	public:
		void* allocate(size_t size) noexcept
		{
			return std::malloc(size);
		}

		void deallocate(void* ptr)
		{
			std::free(ptr);
		}
	};

	template<typename T, typename A = AllocatorStd>
	class AllocatorObj
	{
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
 
		template<typename U>
		struct rebind
		{
			typedef AllocatorObj<U, A> other;
		};

		AllocatorObj() = default;
 
		template<typename U>
		AllocatorObj(AllocatorObj<U, A> const&)
		{
		}
 
		pointer address(reference r)
		{
			return &r;
		}
		const_pointer address(const_reference r)
		{
			return &r;
		}

		pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0)
		{
			void* ptr = A().allocate(cnt * sizeof(T));
			if (!ptr)
			{
				throw std::bad_alloc();
			}
			pointer newMemory = reinterpret_cast<pointer>(ptr);
			return newMemory;
		}

		void deallocate(pointer p, size_type n = 0)
		{
			A().deallocate(reinterpret_cast<void*>(p));
		}

		size_type max_size() const
		{
			return std::numeric_limits<size_type>::max() / sizeof(T);
		}

		template<class... Args>
		void construct(pointer p, Args&&... args)
		{
			::new(static_cast<void*>(p)) T(std::forward<Args>(args)...);
		}

		void destroy(pointer p)
		{
			p->~T();
		}
 
		bool operator==(AllocatorObj const&)
		{
			return true;
		}

		bool operator!=(AllocatorObj const& a)
		{
			return !operator==(a);
		}
	};

	template<typename T>
	class AllocatorImpl
	{
	public:
		/**
		 * Overload new and delete functions to make it work with a custom allocator
		 */
		void* operator new(size_t size) 
		{
			return T::allocate(size);
		}

		void operator delete(void* ptr)
		{
			T::deallocate(ptr);
		}

		void* operator new[](size_t size)
		{
			return T::allocate(size);
		}

		void operator delete[](void* ptr)
		{
			T::deallocate(ptr);
		}
	};
}

