#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <Bus.h>
#include <InputOutput.h>
#include <Ram.h>
#include <Rom.h>

#include <Z80.h>
#include <Profiler.h>
#include <Disassembler.h>

#include "Ula.h"
#include "Buzzer.h"

class Configuration;
class ColourPalette;
class Expansion;

class Board final : public EightBit::Bus {
public:
	Board(const ColourPalette& palette, const Configuration& configuration);

	EightBit::Z80& CPU() { return m_cpu; }
	Ula& ULA() { return m_ula; }
	const Ula& ULA() const { return m_ula; }
	Buzzer& sound() { return m_sound; }
	EightBit::InputOutput& ports() { return m_ports; }
	EightBit::Rom& ROM() { return m_basicRom; }
	EightBit::Ram& VRAM() { return m_contendedRam; }
	EightBit::Ram& WRAM() { return m_uncontendedRam; }

	void plug(std::shared_ptr<Expansion> expansion);
	size_t numberOfExpansions() const { return m_expansions.size(); }
	std::shared_ptr<Expansion> expansion(size_t i) { return m_expansions[i]; }

	void plug(const std::string& path);
	void loadSna(const std::string& path);
	void loadZ80(const std::string& path);

	virtual void initialise() final;
	virtual void raisePOWER() final;
	virtual void lowerPOWER() final;

	void runVerticalBlank();
	void runRasterLines();

protected:
	virtual EightBit::MemoryMapping mapping(uint16_t address) final;

private:
	const Configuration& m_configuration;
	const ColourPalette& m_palette;
	EightBit::InputOutput m_ports;
	EightBit::Z80 m_cpu;
	Ula m_ula;
	Buzzer m_sound;
	std::vector<std::shared_ptr<Expansion>> m_expansions;

	EightBit::Rom m_basicRom;				//0000h - 3FFFh  ROM(BASIC)
	EightBit::Ram m_contendedRam = 0x4000;	//4000h - 7FFFh  RAM(Work RAM and VRAM) (with waitstates)
	EightBit::Ram m_uncontendedRam = 0x8000;//8000h - FFFFh  Additional RAM

	EightBit::Disassembler m_disassembler;
	EightBit::Profiler m_profiler;

	int m_allowed = 0;	// To track "overdrawn" cycle expendature

	void Cpu_ExecutingInstruction_Debug(const EightBit::Z80& cpu);
	void Cpu_ExecutingInstruction_Profile(const EightBit::Z80& cpu);

	void Ula_Proceed(const int& cycles);

	void runCycles(int suggested);
};
