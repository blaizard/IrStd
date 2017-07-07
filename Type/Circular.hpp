#pragma once

#include <mutex>
#include <array>

#include "../Type.hpp"

namespace IrStd
{
	namespace Type
	{
		template<class T, size_t N>
		class Circular
		{
		public:
			Circular()
					: m_index(0)
			{
			}

			/**
			 * Access an element from the array.
			 * 0 is the current element, 1 is the previosu element,
			 * 2 is the second previous...
			 */
			T& get(const size_t index) noexcept
			{
				if (index == 0)
				{
					return m_data[m_index];
				}
				return m_data[(m_index - index) % N];
			}

			T get(const size_t index) const noexcept
			{
				if (index == 0)
				{
					return m_data[m_index];
				}
				return m_data[(m_index - index) % N];
			}

			/**
			 * Add a new element
			 */
			void push(const T& element) noexcept
			{
				std::lock_guard<std::mutex> lock(m_lock);
				// Increase the index
				m_index = (m_index + 1) % N;
				m_data[m_index] = element;
			}

		protected:
			size_t m_index;
			std::array<T, N> m_data;
			std::mutex m_lock;
		};
	}
}
