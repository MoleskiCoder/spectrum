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
#include "ToneSequence.h"

class Configuration;
class ColourPalette;
class Expansion;

class Board final : public EightBit::Bus {
public:
	Board(const ColourPalette& palette, const Configuration& configuration);

	[[nodiscard]] constexpr auto& CPU() noexcept { return m_cpu; }
	[[nodiscard]] constexpr auto& ULA() noexcept { return m_ula; }
	[[nodiscard]] constexpr const auto& ULA() const noexcept { return m_ula; }
	[[nodiscard]] constexpr auto& beep() noexcept { return m_beep; }
	[[nodiscard]] constexpr auto& mic() noexcept { return m_mic; }
	[[nodiscard]] constexpr auto& ports() noexcept { return m_ports; }
	[[nodiscard]] constexpr auto& ROM() noexcept { return m_basicRom; }
	[[nodiscard]] constexpr auto& VRAM() noexcept { return m_contendedRam; }
	[[nodiscard]] constexpr auto& WRAM() noexcept { return m_uncontendedRam; }

	void plug(std::shared_ptr<Expansion> expansion);
	[[nodiscard]] auto numberOfExpansions() const noexcept { return m_expansions.size(); }
	[[nodiscard]] auto expansion(size_t i) noexcept { return m_expansions[i]; }

	void plug(std::string path);
	void loadSna(std::string path);
	void loadZ80(std::string path);

	void attachTZX(std::string path);
	void playTape();

	void initialise() final;
	void raisePOWER() final;
	void lowerPOWER() final;

	[[nodiscard]] uint8_t peek(uint16_t address) noexcept override;
	void poke(uint16_t address, uint8_t value) noexcept override;

	void renderLines();

protected:
	EightBit::MemoryMapping mapping(uint16_t address) noexcept final;

private:
	const Configuration& m_configuration;
	const ColourPalette& m_palette;
	EightBit::InputOutput m_ports;
	EightBit::Z80 m_cpu = *this;
	Ula m_ula = { m_palette, *this };
	Buzzer m_beep = { Ula::FramesPerSecond, Ula::ClockRate };
	Buzzer m_mic = { Ula::FramesPerSecond, Ula::ClockRate };
	std::vector<std::shared_ptr<Expansion>> m_expansions;

	EightBit::Rom m_basicRom;				//0000h - 3FFFh  ROM(BASIC)
	EightBit::Ram m_contendedRam = 0x4000;	//4000h - 7FFFh  RAM(Work RAM and VRAM) (with waitstates)
	EightBit::Ram m_uncontendedRam = 0x8000;//8000h - FFFFh  Additional RAM

	EightBit::Disassembler m_disassembler = *this;
	EightBit::Profiler m_profiler = { m_cpu, m_disassembler };

	int m_allowed = 0;	// To track "overdrawn" cycle expendature

	void runCycle();
};
