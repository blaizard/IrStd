#pragma once

namespace IrStd
{
	namespace Flag
	{
	}

	class FlagImpl
	{
	public:
		FlagImpl()
				: m_flag(ATOMIC_FLAG_INIT)
		{
		}

		bool isSet() const noexcept
		{
			return m_flag;
		}

		void set(const bool value = true) noexcept
		{
			m_flag = value;
		}

		void unset() noexcept
		{
			m_flag = false;
		}

		bool setAndGet(const bool value = true) noexcept
		{
			const bool prev = m_flag.exchange(value);
			return prev;
		}

	private:
		std::atomic<bool> m_flag;
	};
}
