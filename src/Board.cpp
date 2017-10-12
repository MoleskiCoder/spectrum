#include "stdafx.h"
#include "Board.h"
#include "Configuration.h"

#include <iostream>

Board::Board(const ColourPalette& palette, const Configuration& configuration)
: m_configuration(configuration),
  m_palette(palette),
  m_cpu(*this, m_ports),
  m_ula(m_palette, *this),
  m_contendedRam(0x4000),
  m_uncontendedRam(0x8000),
  m_profiler(m_cpu, m_disassembler) {
}

void Board::initialise() {

	auto romDirectory = m_configuration.getRomDirectory();

	m_basicRom.load(romDirectory + "\\48.rom");

	if (m_configuration.isProfileMode()) {
		m_cpu.ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Profile, this, std::placeholders::_1));
	}

	if (m_configuration.isDebugMode()) {
		m_cpu.ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Debug, this, std::placeholders::_1));
	}

	m_cpu.initialise();
	m_ula.initialise();
}

void Board::Cpu_ExecutingInstruction_Profile(const EightBit::Z80& cpu) {
	const auto pc = m_cpu.PC();
	m_profiler.addAddress(pc.word);
	m_profiler.addInstruction(peek(pc.word));
}

void Board::Cpu_ExecutingInstruction_Debug(const EightBit::Z80& cpu) {
	std::cerr
		<< EightBit::Disassembler::state(m_cpu)
		<< " "
		<< m_disassembler.disassemble(m_cpu)
		<< '\n';
}

uint8_t& Board::reference(uint16_t address, bool& rom) {

	rom = true;
	if (address < 0x4000)
		return m_basicRom.reference(address);

	rom = false;
	if (address < 0x8000)
		return m_contendedRam.reference(address - 0x4000);
	return m_uncontendedRam.reference(address - 0x8000);
}

int Board::runRasterLines() {

	int count = 0;

	m_scanLine = 0;

	count += runBlankLines(Ula::UpperRasterBorder * Ula::HorizontalCyclesTotal, Ula::UpperRasterBorder);
	count += runRasterLines(Ula::ActiveRasterHeight * Ula::HorizontalCyclesTotal, Ula::ActiveRasterHeight);
	count += runBlankLines(Ula::LowerRasterBorder * Ula::HorizontalCyclesTotal, Ula::LowerRasterBorder);

	return count;
}

int Board::runRasterLines(int limit, int lines) {
	int count = 0;
	int allowed = Ula::HorizontalCyclesTotal;
	for (int line = 0; line < lines; ++line) {
		auto executed = runRasterLine(allowed);
		count += executed;
		allowed = Ula::HorizontalCyclesTotal - (executed - Ula::HorizontalCyclesTotal);
	}
	return count;
}

int Board::runRasterLine(int limit) {

	int count = 0;

	count += m_cpu.run(Ula::HorizontalDrawCycles);
	m_ula.render(m_scanLine++);
	count += m_cpu.run(limit - Ula::HorizontalDrawCycles);

	return count;
}

int Board::runBlankLines(int limit, int lines) {
	int count = 0;
	int allowed = Ula::HorizontalCyclesTotal;
	for (int line = 0; line < lines; ++line) {
		auto executed = runBlankLine(allowed);
		count += executed;
		allowed = Ula::HorizontalCyclesTotal - (executed - Ula::HorizontalCyclesTotal);
	}
	return count;
}

int Board::runBlankLine(int limit) {
	m_ula.renderBlank(m_scanLine++);
	return m_cpu.run(limit);
}
