#pragma once

#include <string>

#include <Rom.h>
#include <Register.h>

#include "Loader.h"

class TZXFile final : public Loader {
private:
	EightBit::Rom m_contents;
	int m_position = -1;

	uint8_t readByte(int position) const;
	std::vector<uint8_t> readBytes(int position, int amount) const;

	std::vector<uint8_t> fetchBytes(int amount);
	uint8_t fetchByte();

	EightBit::register16_t readWord(int position) const;
	std::vector<EightBit::register16_t> readWords(int position, int amount) const;

	std::vector<EightBit::register16_t> fetchWords(int amount);
	EightBit::register16_t fetchWord();

	auto remaining() const { return m_contents.size() - m_position; }
	auto finished() const { return remaining() <= 0; }

	void readHeader();
	void readBlock();
	void readStandardSpeedDataBlock();

public:
	TZXFile(std::string path);

	void load(Board& board) override;
};
