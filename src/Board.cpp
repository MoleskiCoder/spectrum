#include "stdafx.h"
#include "Board.h"
#include "Configuration.h"
#include "SnaFile.h"

#include <iostream>

Board::Board(const ColourPalette& palette, const Configuration& configuration)
: m_configuration(configuration),
  m_palette(palette),
  m_cpu(*this, m_ports),
  m_ula(m_palette, *this),
  m_disassembler(*this),
  m_profiler(m_cpu, m_disassembler) {
}

void Board::initialise() {

	auto romDirectory = m_configuration.getRomDirectory();
	plug(romDirectory + "\\48.rom");	// ZX Spectrum Basic

	if (m_configuration.isProfileMode()) {
		CPU().ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Profile, this, std::placeholders::_1));
	}

	if (m_configuration.isDebugMode()) {
		CPU().ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Debug, this, std::placeholders::_1));
	}

	ULA().Proceed.connect(std::bind(&Board::Ula_Proceed, this, std::placeholders::_1));

	ULA().initialise();
	buzzer().initialise();
}

void Board::plug(const std::string& path) {
	ROM().load(path);
}

void Board::loadSna(const std::string& path) {

	raisePOWER();

	SnaFile sna(path);
	sna.load();

	CPU().IV() = sna.peek(SnaFile::Offset_I);

	CPU().HL().word = sna.peekWord(SnaFile::Offset_HL_);
	CPU().DE().word = sna.peekWord(SnaFile::Offset_DE_);
	CPU().BC().word = sna.peekWord(SnaFile::Offset_BC_);
	CPU().AF().word = sna.peekWord(SnaFile::Offset_AF_);

	CPU().exx();
	CPU().exxAF();

	CPU().HL().word = sna.peekWord(SnaFile::Offset_HL);
	CPU().DE().word = sna.peekWord(SnaFile::Offset_DE);
	CPU().BC().word = sna.peekWord(SnaFile::Offset_BC);

	CPU().IY().word = sna.peekWord(SnaFile::Offset_IY);
	CPU().IX().word = sna.peekWord(SnaFile::Offset_IX);

	CPU().IFF2() = (sna.peek(SnaFile::Offset_IFF2) >> 2) != 0;
	CPU().REFRESH() = sna.peek(SnaFile::Offset_R);

	CPU().AF().word = sna.peekWord(SnaFile::Offset_AF);
	CPU().SP().word = sna.peekWord(SnaFile::Offset_SP);

	CPU().IM() = sna.peek(SnaFile::Offset_IM);

	ULA().setBorder(sna.peek(SnaFile::Offset_BorderColour));

	for (int i = 0; i < SnaFile::RamSize; ++i)
		poke(ROM().size() + i, sna.peek(SnaFile::HeaderSize + i));

	// XXXX HACK, HACK, HACK!!
	CPU().raiseRESET();
	poke(0xfffe, 0xed);
	poke(0xffff, 0x45);
	CPU().PC().word = 0xfffe;
	CPU().step();
	CPU().pokeWord(0xfffe, sna.peekWord(SnaFile::HeaderSize + 0xfffe));
}

void Board::raisePOWER() {
	EightBit::Bus::raisePOWER();
	CPU().raisePOWER();
	CPU().lowerRESET();
	CPU().raiseHALT();
	CPU().raiseINT();
	CPU().raiseNMI();
}

void Board::lowerPOWER() {
	CPU().lowerPOWER();
	EightBit::Bus::lowerPOWER();
}

void Board::Cpu_ExecutingInstruction_Profile(const EightBit::Z80& cpu) {
	const auto pc = CPU().PC();
	m_profiler.addAddress(pc.word);
	m_profiler.addInstruction(peek(pc.word));
}

void Board::Cpu_ExecutingInstruction_Debug(const EightBit::Z80& cpu) {
	std::cerr
		<< EightBit::Disassembler::state(CPU())
		<< " "
		<< m_disassembler.disassemble(CPU())
		<< '\n';
}

EightBit::MemoryMapping Board::mapping(const uint16_t address) {
	if (address < 0x4000)
		return { ROM(), 0x0000, 0xffff, EightBit::MemoryMapping::AccessLevel::ReadOnly };
	if (address < 0x8000)
		return { VRAM(), 0x4000, 0xffff, EightBit::MemoryMapping::AccessLevel::ReadWrite };
	return { WRAM(), 0x8000, 0xffff,  EightBit::MemoryMapping::AccessLevel::ReadWrite };
}

void Board::runFrame() {
	resetFrameCycles();
	for (int i = 0; i < Ula::TotalHeight; ++i)
		ULA().renderLine(i);
}

void Board::Ula_Proceed(const int& cycles) {
	runCycles(cycles);
}

void Board::runCycles(int suggested) {
	m_allowed += suggested;
	const int taken = CPU().run(m_allowed);
	m_frameCycles += taken;
	m_allowed -= taken;
}
