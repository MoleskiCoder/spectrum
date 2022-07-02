#include "stdafx.h"
#include <bitset>
#include "ToneSequence.h"
#include "TAPBlock.h"

std::vector<ToneSequence::pulse_t> ToneSequence::generate(uint8_t byte) const {
	std::vector<pulse_t> returned;
	returned.reserve(8);
	const std::bitset<8> bits(byte);
	for (int i = 7; i >= 0; --i)
		returned.push_back(generate(bits[i]));
	return returned;
}

std::vector<ToneSequence::pulse_t> ToneSequence::generate(const Content& content) const {
	std::vector<pulse_t> returned;
	returned.reserve(content.size() * 8); // bytes * bits-per-byte
	const auto bytes = const_cast<Content&>(content).readBytes();
	for (auto byte : bytes) {
		const auto pulses = generate(byte);
		assert(pulses.size() == 8);
		returned.insert(returned.end(), pulses.begin(), pulses.end());
	}
	return returned;
}

std::vector<ToneSequence::pulse_t> ToneSequence::generatePilotTone(int pulses) const {
	std::vector<pulse_t> returned(pulses);
	for (int i = 0; i < pulses; ++i)
		returned[i] = generatePulse(pilotTonePulseLength());
	return returned;
}

ToneSequence::pulse_generator_t ToneSequence::generate(const TAPBlock& block) const {

	{
		const auto pulses = generatePilotTone(block.isHeaderBlock() ? headerPilotTonePulses() : dataPilotTonePulses());
		for (const auto& pulse : pulses)
			co_yield pulse;
	}

	co_yield generatePulse(firstSyncTonePulseLength());
	co_yield generatePulse(secondSyncTonePulseLength());

	{
		const auto pulses = generate(block.content());
		for (const auto& pulse : pulses)
			co_yield pulse;
	}

	co_yield generatePause();
}