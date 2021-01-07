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

	[[nodiscard]] EightBit::Z80& CPU() { return m_cpu; }
	[[nodiscard]] Ula& ULA() { return m_ula; }
	[[nodiscard]] const Ula& ULA() const { return m_ula; }
	[[nodiscard]] Buzzer& sound() { return m_sound; }
	[[nodiscard]] EightBit::InputOutput& ports() { return m_ports; }
	[[nodiscard]] EightBit::Rom& ROM() { return m_basicRom; }
	[[nodiscard]] EightBit::Ram& VRAM() { return m_contendedRam; }
	[[nodiscard]] EightBit::Ram& WRAM() { return m_uncontendedRam; }

	void plug(std::shared_ptr<Expansion> expansion);
	[[nodiscard]] size_t numberOfExpansions() const { return m_expansions.size(); }
	[[nodiscard]] std::shared_ptr<Expansion> expansion(size_t i) { return m_expansions[i]; }

	void plug(const std::string& path);
	void loadSna(const std::string& path);
	void loadZ80(const std::string& path);

	virtual void initialise() final;
	virtual void raisePOWER() final;
	virtual void lowerPOWER() final;

	[[nodiscard]] uint8_t peek(uint16_t address) override;
	void poke(uint16_t address, uint8_t value) override;

	void renderLines();

protected:
	virtual EightBit::MemoryMapping mapping(uint16_t address) final;

private:
	const Configuration& m_configuration;
	const ColourPalette& m_palette;
	EightBit::InputOutput m_ports;
	EightBit::Z80 m_cpu = *this;
	Ula m_ula = { m_palette, *this };
	Buzzer m_sound;
	std::vector<std::shared_ptr<Expansion>> m_expansions;

	EightBit::Rom m_basicRom;				//0000h - 3FFFh  ROM(BASIC)
	EightBit::Ram m_contendedRam = 0x4000;	//4000h - 7FFFh  RAM(Work RAM and VRAM) (with waitstates)
	EightBit::Ram m_uncontendedRam = 0x8000;//8000h - FFFFh  Additional RAM

	EightBit::Disassembler m_disassembler = *this;
	EightBit::Profiler m_profiler = { m_cpu, m_disassembler };

	int m_allowed = 0;	// To track "overdrawn" cycle expendature

	void runCycle();
};
