#include "stdafx.h"
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
	returned.reserve(content.size() * 8 ); // bytes * bits-per-byte
	for (int i = 0; i < content.size(); ++i) {
		auto tones = generate(content.peek(i));
		for (const auto& tone : tones)
			returned.push_back(tone);
	}
	return returned;
}

std::vector<ToneSequence::pulse_t> ToneSequence::generatePilotTone(int pulses) const {
	std::vector<pulse_t> returned(pulses);
	for (int i = 0; i < pulses; ++i)
		returned[i] = generatePulse(pilotTonePulseLength());
	return returned;
}

#ifdef USE_COROUTINES

#if __cplusplus >= 202002L

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

#else

void ToneSequence::generate(const TAPBlock& block, pulse_push_t& sink) {

	{
		const auto pulses = generatePilotTone(block.isHeaderBlock() ? headerPilotTonePulses() : dataPilotTonePulses());
		for (const auto& pulse : pulses)
			sink(pulse);
	}

	sink(generatePulse(firstSyncTonePulseLength()));
	sink(generatePulse(secondSyncTonePulseLength()));

	{
		const auto pulses = generate(block.content());
		for (const auto& pulse : pulses)
			sink(pulse);
	}

	sink(generatePause());
}

#endif

#else

std::vector<ToneSequence::pulse_t> ToneSequence::generate(const TAPBlock& block) const {

	std::vector<pulse_t> returned;

	{
		const auto pulses = generatePilotTone(block.isHeaderBlock() ? headerPilotTonePulses() : dataPilotTonePulses());
		returned.insert(returned.end(), pulses.begin(), pulses.end());
	}

	returned.push_back(generatePulse(firstSyncTonePulseLength()));
	returned.push_back(generatePulse(secondSyncTonePulseLength()));

	{
		const auto pulses = generate(block.content());
		returned.insert(returned.end(), pulses.begin(), pulses.end());
	}

	returned.push_back(generatePause());

	return returned;
}

#endif
