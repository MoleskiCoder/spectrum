#pragma once

#include <cassert>
#include <algorithm>
#include <limits>
#include <vector>

#include <Device.h>

// Monophonic ZX Spectrum buzzer emulation (abstract base class)
template<typename T>
class AbstractBuzzer : public EightBit::Device {

	using base = Device;

protected:
	int m_audioFrequency;
	float m_frameRate;
	int m_clockRate;

	static const T LowLevel = std::numeric_limits<T>::min();
	static const T HighLevel = std::numeric_limits<T>::max();

	std::vector<T> m_buffer;

	int m_lastSample = 0;	// position in buffer

	T m_lastLevel = LowLevel;

	AbstractBuzzer(int audioFrequency, float frameRate = Ula::FramesPerSecond, int clockRate = Ula::CpuClockRate)
	: m_audioFrequency(audioFrequency),
	  m_frameRate(frameRate),
	  m_clockRate(clockRate) {
		m_buffer.resize(static_cast<uint64_t>(samplesPerFrame()));
	}

	[[nodiscard]] constexpr float sampleLength() const noexcept { return static_cast<float>(m_audioFrequency) / static_cast<float>(m_clockRate); }
	[[nodiscard]] constexpr float cyclesPerSample() const noexcept { return static_cast<float>(m_clockRate) / static_cast<float>(m_audioFrequency); }
	[[nodiscard]] constexpr float samplesPerFrame() const noexcept { return static_cast<float>(m_audioFrequency) / m_frameRate + 1.0f; }

	[[nodiscard]] constexpr auto sample(int cycle) const noexcept {
		const auto sample = static_cast<float>(cycle) * sampleLength();
		return static_cast<int>(sample);
	}

	constexpr void buzz(T level, int sample) {
		assert(sample >= m_lastSample);
		std::fill(m_buffer.begin() + m_lastSample, m_buffer.begin() + sample, m_lastLevel);
		m_lastSample = sample;
		m_lastLevel = level;
	}

public:
	virtual ~AbstractBuzzer() noexcept = default;

	void raisePOWER() noexcept override {
		base::raisePOWER();
		initialise();
	}

	void lowerPOWER() noexcept override {
		terminate();
		base::lowerPOWER();
	}

	virtual void initialise() {
		start();
	}

	virtual void terminate() {
		stop();
	}

	virtual void playBuffer() = 0;
	virtual void flush() = 0;
	virtual void clear() = 0;
	virtual void stop() = 0;
	virtual void start() = 0;

	constexpr void buzz(EightBit::Device::PinLevel state, int cycle) {
		const T level = EightBit::Device::raised(state) ? HighLevel : LowLevel;
		buzz(level, sample(cycle));
	}

	void endFrame() {
		std::fill(m_buffer.begin() + m_lastSample, m_buffer.end(), m_lastLevel);
		playBuffer();
		m_lastSample = 0;
	}
};
