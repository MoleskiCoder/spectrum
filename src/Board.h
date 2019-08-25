#pragma once

#include <cstdint>
#include <string>

#include <Bus.h>
#include <InputOutput.h>
#include <Ram.h>

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
	EightBit::Rom& ROM() { return m_basicRom; }
	EightBit::Ram& VRAM() { return m_contendedRam; }
	EightBit::Ram& WRAM() { return m_uncontendedRam; }

	virtual void initialise() final;
	virtual void raisePOWER() final;
	virtual void lowerPOWER() final;

	int runFrame(int limit);

protected:
	virtual EightBit::MemoryMapping mapping(uint16_t address) final;

private:
	const Configuration& m_configuration;
	const ColourPalette& m_palette;
	EightBit::InputOutput m_ports;
	EightBit::Z80 m_cpu;
	Ula m_ula;

	EightBit::Rom m_basicRom;				//0000h - 3FFFh  ROM(BASIC)
	EightBit::Ram m_contendedRam = 0x4000;	//4000h - 7FFFh  RAM(Work RAM and VRAM) (with waitstates)
	EightBit::Ram m_uncontendedRam = 0x8000;//8000h - FFFFh  Additional RAM

	EightBit::Disassembler m_disassembler;
	EightBit::Profiler m_profiler;

	void Cpu_ExecutingInstruction_Debug(const EightBit::Z80& cpu);
	void Cpu_ExecutingInstruction_Profile(const EightBit::Z80& cpu);

	void runLine(int& allowed, int& count);
};
