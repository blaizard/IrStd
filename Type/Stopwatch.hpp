#pragma once

#include <chrono>

namespace IrStd
{
	namespace Type
	{
		class Stopwatch
		{
		public:
			class Counter
			{
			public:
				typedef std::chrono::steady_clock::time_point Time;
				Counter();

				template<class T>
				Counter(const T value)
						: Counter()
				{
					add(value);
				}

				template<class T>
				void add(const T value) noexcept
				{
					m_value += value;
				}

				uint64_t getNs() const noexcept;
				uint64_t getUs() const noexcept;
				uint64_t getMs() const noexcept;
				uint64_t getS() const noexcept;

			private:
				friend Stopwatch;
				std::chrono::nanoseconds m_value;
			};

			Stopwatch(const bool autoStart = false);
			Stopwatch(Counter& counter, const bool autoStart = false);
			~Stopwatch();

			void start() noexcept;
			Counter get() const noexcept;
			Counter stop() noexcept;

		private:
			Counter* m_pCounter;
			std::chrono::steady_clock::time_point m_start;
			bool m_running;
		};
	};
}
