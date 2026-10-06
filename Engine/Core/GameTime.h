#pragma once
#include <cstdint>

namespace nu
{
	class Time
	{
	public:
		Time();

		void Reset();
		void Tick();

		float GetTime() const { return TicksToSeconds(m_currentTicks - m_startTicks); }
		float GetDeltaTime() const { return TicksToSeconds(m_deltaTicks) * m_timeScale; }

		float TicksToSeconds(uint64_t ticks) const { return (float)ticks / 1'000'000'000;	}

		void SetTimeScale(float timeScale) { m_timeScale = timeScale; }
		float GetTimeScale() const { return m_timeScale; }

	private:
		float m_timeScale = 1.0f;

		uint64_t m_startTicks = 0;
		uint64_t m_currentTicks = 0;
		uint64_t m_frameTicks = 0;
		uint64_t m_deltaTicks = 0;
	};
}