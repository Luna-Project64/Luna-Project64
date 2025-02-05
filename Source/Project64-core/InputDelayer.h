#pragma once

#include <windows.h>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "CircularBuffer.h"

// #define DEBUG_RECORD_ERROR_DURATIONS 64

class InputDelayer
{
public:
	InputDelayer();
	~InputDelayer();

	DWORD getDelayedKeys();

private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Duration = Clock::duration;

	struct SampledInput
	{
		TimePoint time;
		DWORD key;
	};

	struct SampleInputLessThan
	{
		bool operator() (const SampledInput& left, const SampledInput& right);
		bool operator() (const SampledInput& left, InputDelayer::TimePoint right);
		bool operator() (InputDelayer::TimePoint left, const SampledInput& right);
	};

	static Duration absDiff(TimePoint, TimePoint);

	void work();

	Duration estimateFrameTime();
	DWORD findBestKeys(TimePoint now);

	std::mutex m_StartStopMutex;
	std::mutex m_Mutex;
	bool m_Running = false;
	std::condition_variable m_CV;
	std::thread m_Worker;

	// worker is sampling inputs in this circular buffer
	// 'getDelayedKeys' will lookup the 'SampledInput' that has the closest input
	CircularBuffer<SampledInput, 64> m_SampledInputs;

	// to estimate the duration between the request, this circular buffer is used
	// median value is taken to estimate the 'frame time'
	TimePoint m_DelayedKeysLastRequestTime;
	CircularBuffer<Duration, 64> m_DelayedKeysRequestDurations;

	// timepoint when 'worker' will roughly sample the input
	// if 'expectedWakeUpTime_' is too far from calculated input, worker might be woken up
	TimePoint m_ExpectedWakeUpTime;
	TimePoint m_ExpectedPollingTime;

#ifdef DEBUG_RECORD_ERROR_DURATIONS
	CircularBuffer<Duration, DEBUG_RECORD_ERROR_DURATIONS> recordedErrorDurations_;
#endif
};
