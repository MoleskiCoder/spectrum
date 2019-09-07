#pragma once

#include <string>

#include <Rom.h>

class Board;

class SnapshotFile {
private:
	std::string m_path;
	EightBit::Rom m_rom;

protected:
	SnapshotFile(const std::string& path);

	virtual void loadRegisters(EightBit::Z80& cpu) const = 0;
	virtual void loadMemory(Board& board) const = 0;

	const EightBit::Rom& ROM() const { return m_rom; }
	EightBit::Rom& ROM() { return m_rom; }

	void read();

	uint8_t peek(uint16_t offset) const;
	uint16_t peekWord(uint16_t offset) const;

	auto size() const { return m_rom.size(); }

public:
	virtual void load(Board& board);
};

