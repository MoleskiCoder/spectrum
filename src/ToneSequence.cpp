#include "stdafx.h"
#include "ToneSequence.h"
#include "Ula.h"
#include "TAPBlock.h"
#include <Rom.h>

std::queue<ToneSequence::pulse_t> ToneSequence::generate(uint8_t byte) {
	std::queue<pulse_t> returned;
	const std::bitset<8> bits(byte);
	for (int i = 7; i >= 0; --i)
		returned.push(generate(bits[i]));
	return returned;
}

std::queue<ToneSequence::pulse_t> ToneSequence::generate(const EightBit::Rom& contents) {
	std::queue<pulse_t> returned;
	for (int i = 0; i < contents.size(); ++i) {
		auto generated = generate(contents.peek(i));
		while (!generated.empty()) {
			returned.push(generated.front());
			generated.pop();
		}
	}
	return returned;
}

std::queue<ToneSequence::pulse_t> ToneSequence::generatePilotTone(int pulses) {
	std::queue<pulse_t> returned;
	for (int i = 0; i < pulses; ++i)
		returned.push(generatePulse(pilotTonePulseLength()));
	return returned;
}

EightBit::co_generator_t<ToneSequence::pulse_t> ToneSequence::generate(const TAPBlock& block) {

	{
		auto generated = generatePilotTone(block.isHeaderBlock() ? headerPilotTonePulses() : dataPilotTonePulses());
		while (!generated.empty()) {
			co_yield generated.front();
			generated.pop();
		}
	}

	co_yield generatePulse(firstSyncTonePulseLength());
	co_yield generatePulse(secondSyncTonePulseLength());

	{
		auto generated = generate(block.block());
		while (!generated.empty()) {
			co_yield generated.front();
			generated.pop();
		}
	}

	co_yield generatePause();
}
