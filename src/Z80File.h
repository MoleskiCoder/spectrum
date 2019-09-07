#pragma once

#include <string>
#include <cstdint>

#include "SnapshotFile.h"

class Board;

// https://www.worldofspectrum.org/faq/reference/z80format.htm

class Z80File : public SnapshotFile {
private:
	const static size_t Offset_A = 0;
	const static size_t Offset_F = 1;
	const static size_t Offset_BC = 2;
	const static size_t Offset_HL = 4;
	const static size_t Offset_PC = 6;
	const static size_t Offset_SP = 8;
	const static size_t Offset_I = 10;
	const static size_t Offset_R = 11; // Bit 7 is not significant!

	// Bit 0	 : Bit 7 of the R - register
	// Bit 1 - 3 : Border colour
	// Bit 4     : 1 = Basic SamRom switched in
	// Bit 5     : 1 = Block of data is compressed
	// Bit 6 - 7 : No meaning
	const static size_t Offset_misc_1 = 12;

	const static size_t Offset_DE = 13;
	const static size_t Offset_BC_ = 15;
	const static size_t Offset_DE_ = 17;
	const static size_t Offset_HL_ = 19;
	const static size_t Offset_A_ = 21;
	const static size_t Offset_F_ = 22;
	const static size_t Offset_IY = 23;
	const static size_t Offset_IX = 25;
	const static size_t Offset_IFF1 = 27;
	const static size_t Offset_IFF2 = 28;

	// Bit 0 - 1 : Interrupt mode(0, 1 or 2)
	// Bit 2     : 1 = Issue 2 emulation
	// Bit 3     : 1 = Double interrupt frequency
	// Bit 4 - 5 : 1 = High video synchronisation
	//             3 = Low video synchronisation
	//             0, 2 = Normal
	// Bit 6 - 7 : 0 = Cursor / Protek / AGF joystick
	//             1 = Kempston joystick
	//             2 = Sinclair 2 Left joystick(or user
	//	               defined, for version 3.z80 files)
	//             3 = Sinclair 2 Right joystick
	const static size_t Offset_misc_2 = 29;

	const static size_t HeaderSize = Offset_misc_2 + 1;

	const static size_t RamSize = (32 + 16) * 1024;

	uint8_t misc1() const;

	void loadMemoryV1(Board& board) const;
	void loadMemoryCompressed(Board& board) const;
	void loadMemoryUncompressed(Board& board) const;

protected:
	virtual void loadRegisters(EightBit::Z80& cpu) const final;
	virtual void loadMemory(Board& board) const final;

public:
	Z80File(const std::string& path);

	virtual void load(Board& board) final;
};
