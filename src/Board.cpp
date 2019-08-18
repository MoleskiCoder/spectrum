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

	ROM().load(romDirectory + "\\48.rom");	// ZX Spectrum Basic
	//ROM().load(romDirectory + "\\G12R_ROM.bin");	// Planetoids (Asteroids)
	//ROM().load(romDirectory + "\\G24R_ROM.bin");	// Horace and the Spiders
	//ROM().load(romDirectory + "\\G9R_ROM.bin");	// Space Raiders (Space Invaders)
	//ROM().load(romDirectory + "\\System_Test_ROM.bin");	// Sinclair test ROM by Dr. Ian Logan

	if (m_configuration.isProfileMode()) {
		CPU().ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Profile, this, std::placeholders::_1));
	}

	if (m_configuration.isDebugMode()) {
		CPU().ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Debug, this, std::placeholders::_1));
	}

	ULA().initialise();
}

void Board::raisePOWER() {
	EightBit::Bus::raisePOWER();
	CPU().raisePOWER();
	CPU().lowerRESET();
	CPU().raiseHALT();
	CPU().raiseINT();
	CPU().raiseNMI();
}

void Board::lowerPOWER() noexcept {
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

int Board::runRasterLines() {

	int count = 0;

	m_scanLine = 0;

	count += runBlankLines(Ula::UpperRasterBorder * Ula::HorizontalCyclesTotal, Ula::UpperRasterBorder);
	count += runRasterLines(Ula::ActiveRasterHeight * Ula::HorizontalCyclesTotal, Ula::ActiveRasterHeight);
	count += runBlankLines(Ula::LowerRasterBorder * Ula::HorizontalCyclesTotal, Ula::LowerRasterBorder);

	ULA().finishFrame();

	return count;
}

int Board::runRasterLines(int limit, int lines) {
	int count = 0;
	int allowed = Ula::HorizontalCyclesTotal;
	for (int line = 0; line < lines; ++line) {
		const auto executed = runRasterLine(allowed);
		count += executed;
		allowed = Ula::HorizontalCyclesTotal - (executed - Ula::HorizontalCyclesTotal);
	}
	return count;
}

int Board::runRasterLine(int limit) {

	int count = 0;

	count += CPU().run(Ula::HorizontalDrawCycles);
	ULA().render(m_scanLine++);
	count += CPU().run(limit - Ula::HorizontalDrawCycles);

	return count;
}

int Board::runBlankLines(const int limit, const int lines) {
	int count = 0;
	int allowed = Ula::HorizontalCyclesTotal;
	for (int line = 0; line < lines; ++line) {
		const auto executed = runBlankLine(allowed);
		count += executed;
		allowed = Ula::HorizontalCyclesTotal - (executed - Ula::HorizontalCyclesTotal);
	}
	return count;
}

int Board::runBlankLine(const int limit) {
	ULA().renderBlank(m_scanLine++);
	return CPU().run(limit);
}
