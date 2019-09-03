#include "stdafx.h"
#include "SnaFile.h"
#include "Board.h"

SnaFile::SnaFile(const std::string& path)
: SnapshotFile(path) {
}

void SnaFile::load(Board& board) {

	read();

	board.raisePOWER();
	board.CPU().raiseRESET();

	board.CPU().AF().word = peekWord(Offset_AF_);
	board.CPU().BC().word = peekWord(Offset_BC_);
	board.CPU().DE().word = peekWord(Offset_DE_);
	board.CPU().HL().word = peekWord(Offset_HL_);

	board.CPU().exx();
	board.CPU().exxAF();

	board.CPU().AF().word = peekWord(Offset_AF);
	board.CPU().BC().word = peekWord(Offset_BC);
	board.CPU().DE().word = peekWord(Offset_DE);
	board.CPU().HL().word = peekWord(Offset_HL);

	board.CPU().SP().word = peekWord(Offset_SP);

	board.CPU().IY().word = peekWord(Offset_IY);
	board.CPU().IX().word = peekWord(Offset_IX);

	board.CPU().IV() = peek(Offset_I);
	board.CPU().IM() = peek(Offset_IM);
	board.CPU().IFF2() = (peek(Offset_IFF2) >> 2) != 0;
	board.CPU().REFRESH() = peek(Offset_R);

	board.ULA().setBorder(peek(Offset_BorderColour));

	for (int i = 0; i < RamSize; ++i)
		board.poke(board.ROM().size() + i, peek(HeaderSize + i));

	// XXXX HACK, HACK, HACK!!
	board.poke(0xfffe, 0xed);
	board.poke(0xffff, 0x45);	// ED45 is RETN
	board.CPU().PC().word = 0xfffe;
	board.CPU().step();
	board.CPU().pokeWord(0xfffe, peekWord(HeaderSize + 0xfffe));
}
