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

	EightBit::Rom& ROM() { return m_rom; }

	void read();

	uint8_t peek(uint16_t offset) const;
	uint16_t peekWord(uint16_t offset) const;

	auto size() const { return m_rom.size(); }

public:
	virtual void load(Board& board) = 0;
};

