#include "stdafx.h"
#include "ToneSequence.h"
#include "Ula.h"

// ----____ is one full period
// a half period(whether high or low) is a pulse
void ToneSequence::generatePulse(EightBit::Device::PinLevel level, int length) {
	m_states.push_back({ level, length });
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
	generatePause(pauseTime());
}

void ToneSequence::generate(bool bit) {
	generatePulse(bit ? oneBitTonePulseLength() : zeroBitTonePulseLength());
}

void ToneSequence::generate(uint8_t byte) {
	const std::bitset<8> bits(byte);
	for (int i = 7; i >= 0; --i)
		generate(bits[i]);
}

void ToneSequence::generate(const EightBit::Rom& contents) {
	const auto size = contents.size();
	for (int i = 0; i < size; ++i)
		generate(contents.peek(i));
}

void ToneSequence::generatePilotTone(int pulses) {
	for (int i = 0; i < pulses; ++i)
		generatePulse(pilotTonePulseLength());
}

void ToneSequence::generate(const TAPBlock& block) {
	generatePilotTone(block.isHeaderBlock() ? headerPilotTonePulses() : dataPilotTonePulses());
	generatePulse(firstSyncTonePulseLength());
	generatePulse(secondSyncTonePulseLength());
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

EightBit::co_generator_t<EightBit::Device::PinLevel> ToneSequence::expand() {
	if (!playing())
		throw std::logic_error("Cannot expand tones, if tape is not playing.");
	const auto& compressed = states();
	for (const auto& rle : compressed) {
		const auto& [level, length] = rle;
		for (int i = 0; i < length; ++i)
			co_yield level;
	}
}
