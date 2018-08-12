#pragma once

#include <cstdint>
#include <string>

#include <Bus.h>
#include <InputOutput.h>
#include <Ram.h>
#include <Rom.h>

#include <Z80.h>
#include <Profiler.h>
#include <Disassembler.h>

#include "Ula.h"

class Configuration;
class ColourPalette;

class Board final : public EightBit::Bus {
public:
	Board(const ColourPalette& palette, const Configuration& configuration);

	EightBit::Z80& CPU() { return m_cpu; }
	Ula &ULA() { return m_ula; }
	EightBit::InputOutput &ports() { return m_ports; }
	EightBit::Ram& VRAM() { return m_contendedRam; }
	EightBit::Ram& WRAM() { return m_uncontendedRam; }

	void initialise();

	int runRasterLines();

protected:
	virtual uint8_t& reference(uint16_t address) final;

private:
	const Configuration& m_configuration;
	const ColourPalette& m_palette;
	EightBit::InputOutput m_ports;
	EightBit::Z80 m_cpu;
	Ula m_ula;
	uint8_t m_frameCounter = 0;

	EightBit::Rom m_basicRom;				//0000h - 3FFFh  ROM(BASIC)
	EightBit::Ram m_contendedRam = 0x4000;	//4000h - 7FFFh  RAM(Work RAM and VRAM) (with waitstates)
	EightBit::Ram m_uncontendedRam = 0x8000;//8000h - FFFFh  Additional RAM

	EightBit::Disassembler m_disassembler;
	EightBit::Profiler m_profiler;

	int m_scanLine = 0;

	void Cpu_ExecutingInstruction_Debug(const EightBit::Z80& cpu);
	void Cpu_ExecutingInstruction_Profile(const EightBit::Z80& cpu);

	int runRasterLines(int limit, int lines);
	int runRasterLine(int limit);

	int runBlankLines(int limit, int lines);
	int runBlankLine(int limit);
};
