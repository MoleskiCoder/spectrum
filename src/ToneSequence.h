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
	static const int PilotTonePulseLength = 2168;	// T states
	static const int HeaderPilotTonePulses = 8063;	// Pulses
	static const int DataPilotTonePulses = 3223;	// Pulses

	static const int FirstSyncTonePulseLength = 667;	// T states
	static const int SecondSyncTonePulseLength = 735;	// T states

	static const int ZeroBitTonePulseLength = 855;	// T states
	static const int OneBitTonePulseLength = 1710;	// T states

	static const int PostBlockPause = 1000;	// ms

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

public:
	[[nodiscard]] constexpr const auto& states() const noexcept { return m_states; }

	void generate(const TAPBlock& block);
	void generate(const std::vector<TAPBlock>& blocks);

	void reset();

	std::queue<EightBit::Device::PinLevel> expand() const;
};

