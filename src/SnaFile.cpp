#include "stdafx.h"
#include "SnaFile.h"
#include "Board.h"

SnaFile::SnaFile(const std::string path)
: m_path(path) {}

void SnaFile::loadRegisters(EightBit::Z80& cpu) {

	resetPosition();

	cpu.raiseRESET();

	cpu.IV() = fetchByte();

	// Alternate set first
	cpu.HL() = fetchWord();
	cpu.DE() = fetchWord();
	cpu.BC() = fetchWord();
	cpu.AF() = fetchWord();

	cpu.exx();

	// Current set
	cpu.HL() = fetchWord();
	cpu.DE() = fetchWord();
	cpu.BC() = fetchWord();

	cpu.IY() = fetchWord();
	cpu.IX() = fetchWord();

	cpu.IFF2() = (fetchByte() >> 2) != 0;
	cpu.REFRESH() = fetchByte();

	cpu.exxAF();

	cpu.AF() = fetchWord();	// Current
	cpu.SP() = fetchWord();
	cpu.IM() = fetchByte();

	m_border = fetchByte();
}

void SnaFile::loadMemory(Board& board) {
	auto destination = board.ROM().size();
	while (!finished())
		board.poke(destination++, fetchByte());
}

void SnaFile::load(Board& board) {

	LittleEndianContent::load(path());

	// N.B. Power must be raised prior to loading
	// registers, otherwise power on defaults will override
	// loaded values.
	if (!board.CPU().powered())
		throw std::runtime_error("Whoops: CPU has not been powered on.");

	loadRegisters(board.CPU());
	loadMemory(board);

	board.ULA().setBorder(border());

	// XXXX HACK, HACK, HACK!!
	const auto original = board.CPU().peekWord(0xfffe);
	board.poke(0xfffe, 0xed);
	board.poke(0xffff, 0x45);	// ED45 is RETN
	board.CPU().PC().word = 0xfffe;
	board.CPU().step();
	board.CPU().pokeWord(0xfffe, original);
}
