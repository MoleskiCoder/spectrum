#pragma once

#include <string>

#include "SnapshotFile.h"

class Board;

class SnaFile : public SnapshotFile {
private:
	const static size_t Offset_I = 0x0;
	const static size_t Offset_HL_ = 0x1;
	const static size_t Offset_DE_ = 0x3;
	const static size_t Offset_BC_ = 0x5;
	const static size_t Offset_AF_ = 0x7;
	const static size_t Offset_HL = 0x9;
	const static size_t Offset_DE = 0xb;
	const static size_t Offset_BC = 0xd;
	const static size_t Offset_IY = 0xf;
	const static size_t Offset_IX = 0x11;
	const static size_t Offset_IFF2 = 0x13;
	const static size_t Offset_R = 0x14;
	const static size_t Offset_AF = 0x15;
	const static size_t Offset_SP = 0x17;
	const static size_t Offset_IM = 0x19;
	const static size_t Offset_BorderColour = 0x1a;

	const static size_t HeaderSize = Offset_BorderColour + 1;

	const static size_t RamSize = (32 + 16) * 1024;

protected:
	virtual void loadRegisters(EightBit::Z80& cpu) const final;
	virtual void loadMemory(Board& board) const final;

public:
	SnaFile(const std::string& path);

	virtual void load(Board& board) final;
};
