#pragma once

#include <cstdint>
#include <string>

#include "LittleEndianContent.h"

namespace EightBit {
	class Z80;
}

class Board;

class SnapshotFile : public LittleEndianContent {
private:
	const std::string m_path;

protected:
	SnapshotFile(std::string path);

	virtual void loadRegisters(EightBit::Z80& cpu) = 0;
	virtual void loadMemory(Board& board) = 0;

	void read();

public:
	virtual void load(Board& board);
	[[nodiscard]] auto path() const { return m_path; }
};

