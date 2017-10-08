#pragma once

#include <string>

#include <Rom.h>
#include <Ram.h>

#include <Z80.h>
#include <Profiler.h>
#include <Disassembler.h>

#include "Configuration.h"
#include "Ula.h"
#include "ColourPalette.h"

class Board : public EightBit::Bus {
public:
	Board(const Configuration& configuration);

	EightBit::Z80& CPU() { return m_cpu; }
	Ula &ULA() { return m_ula; }

	void initialise();

	int runRasterLines();

protected:
	virtual uint8_t& reference(uint16_t address, bool& rom);

private:
	const Configuration& m_configuration;
	EightBit::InputOutput m_ports;
	EightBit::Z80 m_cpu;
	ColourPalette m_palette;
	Ula m_ula;

	EightBit::Rom m_basicRom;		//0000h - 3FFFh  ROM(BASIC)
	EightBit::Ram m_contendedRam;	//4000h - 7FFFh  RAM(Work RAM and VRAM) (with waitstates)
	EightBit::Ram m_uncontendedRam;	//8000h - FFFFh  Additional RAM

	EightBit::Disassembler m_disassembler;
	EightBit::Profiler m_profiler;

	int m_scanLine;

	void Cpu_ExecutingInstruction_Debug(const EightBit::Z80& cpu);
	void Cpu_ExecutingInstruction_Profile(const EightBit::Z80& cpu);

	int runRasterLines(int limit, int lines);
	int runRasterLine(int limit);
};