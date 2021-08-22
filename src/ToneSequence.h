#pragma once

#include <vector>

#include <boost/dynamic_bitset.hpp>

#include <Device.h>

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

	std::vector<EightBit::Device::PinLevel> m_states;
	EightBit::Device::PinLevel m_last = EightBit::Device::PinLevel::Low;

	[[nodiscard]] static boost::dynamic_bitset<> emit(const EightBit::Rom& contents);

	void generatePause(int length);
	void generatePause();
	void generatePulse(int length);

	void generate(bool bit);
	void generate(boost::dynamic_bitset<> bits);
	void generate(const EightBit::Rom& contents);

	void generatePilotTone(int pulses);

public:
	[[nodiscard]] constexpr const auto& states() const noexcept { return m_states; }

	void generate(const TAPBlock& block);
	void generate(const std::vector<TAPBlock>& blocks);

	void reset();
};

