#include "stdafx.h"
#include "SnaFile.h"
#include "Board.h"

SnaFile::SnaFile(const std::string& path)
: SnapshotFile(path) {
}

void SnaFile::loadRegisters(EightBit::Z80& cpu) const {

	cpu.raiseRESET();

	cpu.IV() = peek(Offset_I);

	cpu.HL().word = peekWord(Offset_HL_);
	cpu.DE().word = peekWord(Offset_DE_);
	cpu.BC().word = peekWord(Offset_BC_);
	cpu.AF().word = peekWord(Offset_AF_);

	cpu.exx();

	cpu.HL().word = peekWord(Offset_HL);
	cpu.DE().word = peekWord(Offset_DE);
	cpu.BC().word = peekWord(Offset_BC);

	cpu.IY().word = peekWord(Offset_IY);
	cpu.IX().word = peekWord(Offset_IX);

	cpu.IFF2() = (peek(Offset_IFF2) >> 2) != 0;
	cpu.REFRESH() = peek(Offset_R);

	cpu.exxAF();

	cpu.AF().word = peekWord(Offset_AF);
	cpu.SP().word = peekWord(Offset_SP);
	cpu.IM() = peek(Offset_IM);
}

void SnaFile::loadMemory(Board& board) const {
	for (int i = 0; i < RamSize; ++i)
		board.poke(board.ROM().size() + i, peek(HeaderSize + i));
}

void SnaFile::load(Board& board) {

	SnapshotFile::load(board);

	board.ULA().setBorder(peek(Offset_BorderColour));

	// XXXX HACK, HACK, HACK!!
	board.poke(0xfffe, 0xed);
	board.poke(0xffff, 0x45);	// ED45 is RETN
	board.CPU().PC().word = 0xfffe;
	board.CPU().step();
	board.CPU().pokeWord(0xfffe, peekWord(HeaderSize + 0xfffe));
}
