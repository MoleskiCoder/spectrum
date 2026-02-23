#include "stdafx.h"
#include "Board.h"
#include "Configuration.h"
#include "SnaFile.h"
#include "Z80File.h"
#include "TZXFile.h"
#include "Expansion.h"

#include <iostream>

Board::Board(const ColourPalette& palette, Configuration& configuration)
: m_configuration(configuration),
  m_palette(palette) {
}

void Board::initialise() {

	auto romDirectory = m_configuration.getRomDirectory();
	plug(romDirectory + "\\48.rom");	// ZX Spectrum Basic

	ULA().Proceed.connect([this](EightBit::EventArgs) {
		runCycle();
	});

	CPU().ExecutedInstruction.connect([this](const EightBit::Z80&) {
		CPU().raiseRESET();
	});

	if (m_configuration.isDebugMode()) {
		CPU().ExecutingInstruction.connect([this](const EightBit::Z80&) {
			std::cerr
				<< EightBit::Disassembler::state(CPU())
				<< " "
				<< m_disassembler.disassemble(CPU())
				<< '\n';
		});
	}
}

void Board::plug(std::shared_ptr<Expansion> expansion) {
	m_expansions.push_back(expansion);
}

void Board::plug(const std::string path) {
	ROM().load(path);
}

void Board::loadSna(const std::string path) {
	SnaFile sna(path);
	sna.load(*this);
}

void Board::loadZ80(const std::string path) {
	Z80File z80(path);
	z80.load(*this);
}

void Board::raisePOWER() noexcept {

	EightBit::Bus::raisePOWER();

	for (auto& expansion : m_expansions)
		expansion->raisePOWER();
	
	ULA().raisePOWER();
	CPU().raisePOWER();
	CPU().lowerRESET();
	CPU().raiseINT();
	CPU().raiseNMI();
	
	sound().start();
}

void Board::lowerPOWER() noexcept {

	sound().stop();

	CPU().lowerPOWER();
	ULA().lowerPOWER();

	for (auto& expansion : m_expansions)
		expansion->lowerPOWER();

	EightBit::Bus::lowerPOWER();
}

EightBit::MemoryMapping Board::mapping(const uint16_t address) noexcept {
	if (address < 0x4000)
		return { ROM(), 0x0000, 0xffff, EightBit::MemoryMapping::AccessLevel::ReadOnly };
	if (address < 0x8000)
		return { VRAM(), 0x4000, 0xffff, EightBit::MemoryMapping::AccessLevel::ReadWrite };
	return { WRAM(), 0x8000, 0xffff,  EightBit::MemoryMapping::AccessLevel::ReadWrite };
}

void Board::renderLines() {
	ULA().renderLines();
}

void Board::runCycle() {
	const int taken = CPU().run(++m_allowed);
	m_allowed -= taken;
}
