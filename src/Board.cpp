#include "stdafx.h"
#include "Board.h"
#include "Configuration.h"
#include "SnaFile.h"
#include "Z80File.h"
#include "Expansion.h"

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

	buzzer().initialise();
}

void Board::plug(std::shared_ptr<Expansion> expansion) {
	m_expansions.push_back(expansion);
}

void Board::plug(const std::string& path) {
	ROM().load(path);
}

void Board::loadSna(const std::string& path) {
	SnaFile sna(path);
	sna.load(*this);
}

void Board::loadZ80(const std::string& path) {
	Z80File z80(path);
	z80.load(*this);
}

void Board::raisePOWER() {
	EightBit::Bus::raisePOWER();
	for (auto expansion : m_expansions)
		expansion->raisePOWER();
	ULA().raisePOWER();
	CPU().raisePOWER();
	CPU().lowerRESET();
	CPU().raiseHALT();
	CPU().raiseINT();
	CPU().raiseNMI();
}

void Board::lowerPOWER() {
	CPU().lowerPOWER();
	ULA().lowerPOWER();
	for (auto expansion : m_expansions)
		expansion->lowerPOWER();
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

void Board::runVerticalBlank() {
	for (int i = 0; i < Ula::VerticalRetraceLines; ++i)
		ULA().renderLine(i);
}

void Board::runRasterLines() {
	for (int i = 0; i < Ula::RasterHeight; ++i)
		ULA().renderLine(i + Ula::VerticalRetraceLines);
}

void Board::Ula_Proceed(const int& cycles) {
	runCycles(cycles);
}

void Board::runCycles(int suggested) {
	m_allowed += suggested;
	const int taken = CPU().run(m_allowed);
	m_allowed -= taken;
}
