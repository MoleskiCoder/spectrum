#include "stdafx.h"
#include "SnapshotFile.h"

#include "Board.h"

#include <Register.h>

SnapshotFile::SnapshotFile(const std::string path)
: Loader(path) {}

uint8_t SnapshotFile::peek(uint16_t offset) const {
	return content().peek(offset);
}

// Assumed to be little-endian!
uint16_t SnapshotFile::peekWord(uint16_t offset) const {
	const auto low = peek(offset++);
	const auto high = peek(offset);
	return EightBit::register16_t(low, high).word;
}

void SnapshotFile::read() {
	content().load(path());
}

void SnapshotFile::load(Board& board) {

	read();

	// N.B. Power must be raised prior to loading
	// registers, otherwise power on defaults will override
	// loaded values.
	if (!board.CPU().powered())
		throw std::runtime_error("Whoops: CPU has not been powered on.");

	examineHeaders();
	loadRegisters(board.CPU());
	loadMemory(board);
}

void SnapshotFile::examineHeaders() {

}