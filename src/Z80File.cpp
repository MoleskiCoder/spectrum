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

	auto misc1 = peek(Offset_misc_1);
	const auto misc2 = peek(Offset_misc_2);

	if (misc1 == 0xff)
		misc1 = 1;

	cpu.A() = peekWord(Offset_A_);
	cpu.F() = peekWord(Offset_F_);
	cpu.BC().word = peekWord(Offset_BC_);
	cpu.DE().word = peekWord(Offset_DE_);
	cpu.HL().word = peekWord(Offset_HL_);

	cpu.exx();
	cpu.exxAF();

	cpu.A() = peekWord(Offset_A);
	cpu.F() = peekWord(Offset_F);
	cpu.BC().word = peekWord(Offset_BC);
	cpu.DE().word = peekWord(Offset_DE);
	cpu.HL().word = peekWord(Offset_HL);

	cpu.PC().word = peekWord(Offset_PC);
	cpu.SP().word = peekWord(Offset_SP);

	cpu.IY().word = peekWord(Offset_IY);
	cpu.IX().word = peekWord(Offset_IX);

	cpu.IV() = peek(Offset_I);
	cpu.IM() = misc2 & EightBit::Chip::Mask2;
	cpu.IFF1() = peek(Offset_IFF1);
	cpu.IFF2() = peek(Offset_IFF2);
	cpu.REFRESH().variable = peek(Offset_R);
	cpu.REFRESH().high = misc1 & EightBit::Chip::Mask1;

	board.ULA().setBorder((misc1 >> 1) & EightBit::Chip::Mask3);

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