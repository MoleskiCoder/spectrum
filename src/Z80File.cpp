#include "stdafx.h"
#include "Z80File.h"
#include "Board.h"

#include <stdexcept>

Z80File::Z80File(const std::string& path)
: SnapshotFile(path) {
}

void Z80File::load(Board& board) {

	read();

	EightBit::Z80& cpu = board.CPU();

	board.raisePOWER();
	cpu.raiseRESET();

	cpu.A() = peek(Offset_A);
	cpu.F() = peek(Offset_F);

	cpu.BC().word = peekWord(Offset_BC);
	cpu.HL().word = peekWord(Offset_HL);
	cpu.PC().word = peekWord(Offset_PC);
	cpu.SP().word = peekWord(Offset_SP);

	cpu.IV() = peek(Offset_I);

	cpu.REFRESH().variable = peek(Offset_R);

	auto misc1 = peek(Offset_misc_1);
	if (misc1 == 0xff)
		misc1 = 1;
	cpu.REFRESH().high = misc1 & EightBit::Chip::Mask1;
	board.ULA().setBorder((misc1 >> 1) & EightBit::Chip::Mask3);

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

	const bool compressed = (misc1 & EightBit::Chip::Bit5) != 0;
	const bool v1 = cpu.PC().word != 0;

	if (v1) {
		if (compressed) {
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
		} else {
			for (int i = 0; i < RamSize; ++i)
				board.poke(board.ROM().size() + i, peek(HeaderSize + i));
		}
	} else {
		throw std::runtime_error("Only V1 Z80 files are handled.");
	}
}