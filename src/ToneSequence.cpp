#include "stdafx.h"
#include "ToneSequence.h"
#include "Ula.h"

// ----____ is one full period
// a half period(whether high or low) is a pulse
void ToneSequence::generatePulse(EightBit::Device::PinLevel level, int length) {
	for (int i = 0; i < length; ++i)
		m_states.push_back(level);
}

void ToneSequence::generatePulse(int length) {
	// Doesn't matter what the value is, as long as it's flipped
	EightBit::Device::flip(m_last);
	generatePulse(m_last, length);
}

void ToneSequence::generatePause(int length) {
	generatePulse(EightBit::Device::PinLevel::Low, length);
}

void ToneSequence::generatePause() {
	generatePause(Ula::ClockRate / 2);	// Giving CPU clock at 1/2 ULA clock
}

void ToneSequence::generate(bool bit) {
	generatePulse(bit ? OneBitTonePulseLength : ZeroBitTonePulseLength);
}

void ToneSequence::generate(uint8_t byte) {
	const std::bitset<8> bits(byte);
	for (int i = 0; i < 8; ++i)
		generate(bits[i]);
}

void ToneSequence::generate(const EightBit::Rom& contents) {
	const auto size = contents.size();
	for (int i = 0; i < size; ++i)
		generate(contents.peek(i));
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
	reset();
	for (const auto& block : blocks)
		generate(block);
}

void ToneSequence::reset() {
	m_states.clear();
}
