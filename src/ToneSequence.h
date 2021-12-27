#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include <Device.h>

#include <co_generator_t.h>

class TAPBlock;
class Content;

class ToneSequence {
public:
	typedef EightBit::Device::PinLevel amplitude_t;
	typedef std::pair<amplitude_t, int> pulse_t;

protected:
	[[nodiscard]] constexpr auto pauseTime() const noexcept { return m_pauseTime; }
	[[nodiscard]] constexpr auto& pauseTime() noexcept { return m_pauseTime; }

	[[nodiscard]] constexpr auto oneBitTonePulseLength() const noexcept { return m_oneBitTonePulseLength; }
	[[nodiscard]] constexpr auto& oneBitTonePulseLength() noexcept { return m_oneBitTonePulseLength; }

	[[nodiscard]] constexpr auto zeroBitTonePulseLength() const noexcept { return m_zeroBitTonePulseLength; }
	[[nodiscard]] constexpr auto& zeroBitTonePulseLength() noexcept { return m_zeroBitTonePulseLength; }

	[[nodiscard]] constexpr auto pilotTonePulseLength() const noexcept { return m_pilotTonePulseLength; }
	[[nodiscard]] constexpr auto& pilotTonePulseLength() noexcept { return m_pilotTonePulseLength; }

	[[nodiscard]] constexpr auto headerPilotTonePulses() const noexcept { return m_headerPilotTonePulses; }
	[[nodiscard]] constexpr auto& headerPilotTonePulses() noexcept { return m_headerPilotTonePulses; }

	[[nodiscard]] constexpr auto dataPilotTonePulses() const noexcept { return m_dataPilotTonePulses; }
	[[nodiscard]] constexpr auto& dataPilotTonePulses() noexcept { return m_dataPilotTonePulses; }

	[[nodiscard]] constexpr auto firstSyncTonePulseLength() const noexcept { return m_firstSyncTonePulseLength; }
	[[nodiscard]] constexpr auto& firstSyncTonePulseLength() noexcept { return m_firstSyncTonePulseLength; }

	[[nodiscard]] constexpr auto secondSyncTonePulseLength() const noexcept { return m_secondSyncTonePulseLength; }
	[[nodiscard]] constexpr auto& secondSyncTonePulseLength() noexcept { return m_secondSyncTonePulseLength; }

private:
	int m_pauseTime = -1;					// ms
	int m_oneBitTonePulseLength = -1;		// T states
	int m_zeroBitTonePulseLength = -1;		// T states
	int m_pilotTonePulseLength = -1;		// T states
	int m_headerPilotTonePulses = -1;		// Pulses
	int m_dataPilotTonePulses = -1;			// Pulses
	int m_firstSyncTonePulseLength = -1;	// T states
	int m_secondSyncTonePulseLength = -1;	// T states

	mutable amplitude_t m_last = amplitude_t::High;	// Meaning the first pulse will be low

	// ----____ is one full period
	// a half period(whether high or low) is a pulse
	[[nodiscard]] static constexpr pulse_t generatePulse(amplitude_t level, int length) noexcept {
		return { level, length };
	}

	[[nodiscard]] constexpr auto generatePulse(int length) const noexcept {
		// Doesn't matter what the value is, as long as it's flipped
		EightBit::Device::flip(m_last);
		return generatePulse(m_last, length);
	}

	[[nodiscard]] static constexpr auto generatePause(int length) noexcept {
		return generatePulse(amplitude_t::Low, length);
	}

	[[nodiscard]] constexpr auto generatePause() const noexcept {
		return generatePause(pauseTime());
	}

	[[nodiscard]] pulse_t generate(bool bit) const noexcept {
		return generatePulse(bit ? oneBitTonePulseLength() : zeroBitTonePulseLength());
	}

	[[nodiscard]] std::vector<pulse_t> generate(uint8_t byte) const;
	[[nodiscard]] std::vector<pulse_t> generate(const Content& content) const;

	[[nodiscard]] std::vector<pulse_t> generatePilotTone(int pulses) const;

public:
	[[nodiscard]] EightBit::co_generator_t<pulse_t> generate(const TAPBlock& block) const;
};