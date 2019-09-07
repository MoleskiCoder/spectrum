#include "stdafx.h"
#include "SnapshotFile.h"

#include "Board.h"

#include <Register.h>

SnapshotFile::SnapshotFile(const std::string& path)
: m_path(path) {
}

uint8_t SnapshotFile::peek(uint16_t offset) const {
	return m_rom.peek(offset);
}

// Assumed to be little-endian!
uint16_t SnapshotFile::peekWord(uint16_t offset) const {
	const auto low = peek(offset++);
	const auto high = peek(offset);
	return EightBit::register16_t(low, high).word;
}

void SnapshotFile::read() {
	m_rom.load(m_path);
}

void SnapshotFile::load(Board& board) {

	read();

	// N.B. Power must be raised prior to loading
	// registers, otherwise power on defaults will override
	// loaded values.
	board.raisePOWER();

	loadRegisters(board.CPU());
	loadMemory(board);
}
