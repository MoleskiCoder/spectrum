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

	virtual void examineHeaders();
	virtual void loadRegisters(EightBit::Z80& cpu) const = 0;
	virtual void loadMemory(Board& board) const = 0;

	[[nodiscard]] const EightBit::Rom& ROM() const { return m_rom; }
	[[nodiscard]] EightBit::Rom& ROM() { return m_rom; }

	void read();

	[[nodiscard]] uint8_t peek(uint16_t offset) const;
	[[nodiscard]] uint16_t peekWord(uint16_t offset) const;

	[[nodiscard]] auto size() const { return m_rom.size(); }

public:
	virtual void load(Board& board);
};

