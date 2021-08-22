#include "stdafx.h"
#include "ToneSequence.h"
#include "Ula.h"

void ToneSequence::generatePause(int length) {
	for (int i = 0; i < length; ++i)
		m_states.push_back(EightBit::Device::PinLevel::Low);
}

void ToneSequence::generatePause() {
	const auto period = Ula::ClockRate / 2; // Giving CPU clock at 1/2 ULA clock
	generatePause(period);
}

// ----____ is one full period
// a half period(whether high or low) is a pulse
void ToneSequence::generatePulse(int length) {
	// Doesn't matter what the value is, as long as it's flipped
	EightBit::Device::flip(m_last);
	for (int i = 0; i < length; ++i)
		m_states.push_back(m_last);
}

void ToneSequence::generate(bool bit) {
	generatePulse(bit ? OneBitTonePulseLength : ZeroBitTonePulseLength);
}

void ToneSequence::generate(boost::dynamic_bitset<> bits) {
	for (size_t i = 0; i < bits.size(); ++i)
		generate(bits[i]);
}

boost::dynamic_bitset<> ToneSequence::emit(const EightBit::Rom& contents) {
	const auto size = contents.size();
	boost::dynamic_bitset<> returned(size * 8);
	for (int i = 0; i < size; ++i) {
		const auto byte = contents.peek(i);
		const std::bitset<8> bits(byte);
		for (int j = 0; j < 8; ++j)
			returned.set(i * j, bits.test(j));
	}
	return returned;
}

void ToneSequence::generate(const EightBit::Rom& contents) {
	generate(emit(contents));
}

void ToneSequence::generatePilotTone(int pulses) {
	for (int i = 0; i < pulses; ++i)
		generatePulse(PilotTonePulseLength);
}

void ToneSequence::generate(const TAPBlock& block) {
	generatePilotTone(block.isHeaderBlock() ? HeaderPilotTonePulses : DataPilotTonePulses);
	generatePulse(FirstSyncTonePulseLength);
	generatePulse(SecondSyncTonePulseLength);
	generate(block.block());
	generatePause();
}

void ToneSequence::generate(const std::vector<TAPBlock>& blocks) {
	for (const auto& block : blocks)
		generate(block);
}

void ToneSequence::reset() {
	m_states.clear();
}
