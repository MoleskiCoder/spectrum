#include "stdafx.h"
#include "Z80File.h"
#include "Board.h"

#include <stdexcept>

Z80File::Z80File(const std::string& path)
: SnapshotFile(path) {
}

uint8_t Z80File::misc1() const {
	const auto misc1 = peek(Offset_misc_1);
	return misc1 == 0xff ? 1 : misc1;
}

void Z80File::loadRegisters(EightBit::Z80& cpu) const {

	cpu.raiseRESET();

	cpu.A() = peek(Offset_A);
	cpu.F() = peek(Offset_F);

	cpu.BC().word = peekWord(Offset_BC);
	cpu.HL().word = peekWord(Offset_HL);
	cpu.PC().word = peekWord(Offset_PC);
	cpu.SP().word = peekWord(Offset_SP);

	cpu.IV() = peek(Offset_I);

	cpu.REFRESH().variable = peek(Offset_R);

	cpu.REFRESH().high = misc1() & EightBit::Chip::Mask1;

	cpu.DE().word = peekWord(Offset_DE);

	cpu.exx();

	cpu.BC().word = peekWord(Offset_BC_);
	cpu.DE().word = peekWord(Offset_DE_);
	cpu.HL().word = peekWord(Offset_HL_);

	cpu.exxAF();

	cpu.A() = peek(Offset_A_);
	cpu.F() = peek(Offset_F_);

	cpu.IY().word = peekWord(Offset_IY);
	cpu.IX().word = peekWord(Offset_IX);

	cpu.IFF1() = peek(Offset_IFF1);
	cpu.IFF2() = peek(Offset_IFF2);

	const auto misc2 = peek(Offset_misc_2);
	cpu.IM() = misc2 & EightBit::Chip::Mask2;

	cpu.exx();
	cpu.exxAF();
}

void Z80File::loadMemoryCompressed(Board& board) const {
	int previous = 0x100;
	size_t position = board.ROM().size();
	const auto fileSize = size() - 4;
	for (size_t i = HeaderSize; i != fileSize; ++i) {
		const auto current = ROM().peek(i);
		if (current == 0xed && previous == 0xed) {
			if (i > (fileSize - 2))
				throw std::runtime_error("File is too short for repeat specification.");
			const uint8_t repeats = peek(++i);
			const uint8_t value = peek(++i);
			--position;
			for (int j = 0; j < repeats; ++j)
				board.poke(position++, value);
			previous = 0x100;
		} else {
			board.poke(position++, current);
			previous = current;
		}
	}
}

void Z80File::loadMemoryUncompressed(Board& board) const {
	for (int i = 0; i < RamSize; ++i)
		board.poke(board.ROM().size() + i, peek(HeaderSize + i));
}

void Z80File::loadMemoryV1(Board& board) const {
	const bool compressed = (misc1() & EightBit::Chip::Bit5) != 0;
	if (compressed)
		loadMemoryCompressed(board);
	else
		loadMemoryUncompressed(board);
}

void Z80File::loadMemory(Board& board) const {
	const bool v1 = board.CPU().PC().word != 0;
	if (!v1)
		throw std::runtime_error("Only V1 Z80 files are handled.");
	loadMemoryV1(board);
}

void Z80File::load(Board& board) {

	SnapshotFile::load(board);

	board.ULA().setBorder((misc1() >> 1) & EightBit::Chip::Mask3);
}