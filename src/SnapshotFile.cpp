#include "stdafx.h"
#include "SnapshotFile.h"

#include "Board.h"

#include <Register.h>

SnapshotFile::SnapshotFile(const std::string path)
: m_path(path) {}

void SnapshotFile::read() {
	LittleEndianContent::load(path());
}

void SnapshotFile::load(Board& board) {

	read();

	// N.B. Power must be raised prior to loading
	// registers, otherwise power on defaults will override
	// loaded values.
	if (!board.CPU().powered())
		throw std::runtime_error("Whoops: CPU has not been powered on.");

	loadRegisters(board.CPU());
	loadMemory(board);
}
