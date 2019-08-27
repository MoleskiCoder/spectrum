#include "stdafx.h"
#include "Board.h"
#include "Configuration.h"

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

	ULA().initialise();
	buzzer().initialise();
}

void Board::plug(const std::string& path) {
	ROM().load(path);
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

int Board::runFrame(int limit) {

	const auto excess = Ula::CyclesPerFrame - limit;
	int allowed = -excess;
	resetFrameCycles();

	for (int i = 0; i < Ula::TotalHeight; ++i) {
		ULA().renderLine(i);
		runLine(allowed);
	}

	return frameCycles();
}

void Board::runLine(int& allowed) {
	allowed += Ula::CyclesPerLine;
	const int taken = CPU().run(allowed);
	m_frameCycles += taken;
	allowed -= taken;
}
