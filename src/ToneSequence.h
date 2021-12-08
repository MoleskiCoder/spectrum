#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <queue>

#include <Device.h>
#include <Rom.h>

#include "TAPBlock.h"

namespace EightBit {
	class Rom;
}

class TAPBlock;

class ToneSequence {
private:
	int m_pauseTime = -1;
	int m_oneBitTonePulseLength = -1;
	int m_zeroBitTonePulseLength = -1;
	int m_pilotTonePulseLength = -1;
	int m_headerPilotTonePulses = -1;
	int m_dataPilotTonePulses = -1;
	int m_firstSyncTonePulseLength = -1;
	int m_secondSyncTonePulseLength = -1;

	std::vector<std::pair<EightBit::Device::PinLevel, int>> m_states;
	EightBit::Device::PinLevel m_last = EightBit::Device::PinLevel::Low;

	void generatePulse(EightBit::Device::PinLevel level, int length);
	void generatePulse(int length);

	void generatePause(int length);
	void generatePause();

	void generate(bool bit);
	void generate(uint8_t byte);
	void generate(const EightBit::Rom& contents);

	void generatePilotTone(int pulses);

	static void expand(std::queue<EightBit::Device::PinLevel>& queue, EightBit::Device::PinLevel level, int length);

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

public:
	[[nodiscard]] constexpr const auto& states() const noexcept { return m_states; }

	void generate(const TAPBlock& block);
	void generate(const std::vector<TAPBlock>& blocks);

	void reset();

	[[nodiscard]] std::queue<EightBit::Device::PinLevel> expand() const;
};