#include "stdafx.h"
#include "Board.h"
#include "Configuration.h"
#include "SnaFile.h"
#include "Z80File.h"
#include "TZXFile.h"
#include "Expansion.h"

#include <iostream>

Board::Board(const ColourPalette& palette, const Configuration& configuration)
: m_configuration(configuration),
  m_palette(palette) {
}

void Board::initialise() {

	auto romDirectory = m_configuration.getRomDirectory();
	plug(romDirectory + "\\48.rom");	// ZX Spectrum Basic

	if (m_configuration.isProfileMode()) {
		CPU().ExecutingInstruction.connect([this](const EightBit::Z80&) {
			const auto pc = CPU().PC().word;
			m_profiler.addAddress(pc);
			m_profiler.addInstruction(peek(pc));
		});
	}

	if (m_configuration.isDebugMode()) {
		CPU().ExecutingInstruction.connect([this](const EightBit::Z80&) {
			std::cerr
				<< EightBit::Disassembler::state(CPU())
				<< " "
				<< m_disassembler.disassemble(CPU())
				<< '\n';
		});
	}

	ULA().Proceed.connect([this](EightBit::EventArgs) {
		runCycle();
	});
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

void Board::attachTZX(const std::string path) {
	ULA().attachTZX(path);
}

void Board::playTape() {
	ULA().playTape();
}

void Board::raisePOWER() {
	EightBit::Bus::raisePOWER();
	for (auto expansion : m_expansions)
		expansion->raisePOWER();
	ULA().raisePOWER();
	CPU().raisePOWER();
	CPU().lowerRESET();
	CPU().raiseINT();
	CPU().raiseNMI();
	beep().start();
	mic().start();
}

void Board::lowerPOWER() {
	mic().stop();
	beep().stop();
	CPU().lowerPOWER();
	ULA().lowerPOWER();
	for (auto expansion : m_expansions)
		expansion->lowerPOWER();
	EightBit::Bus::lowerPOWER();
}

uint8_t Board::peek(uint16_t address) noexcept {
	CPU().lower(CPU().MREQ());
	CPU().raise(CPU().IORQ());
	return EightBit::Bus::peek(address);
}

void Board::poke(uint16_t address, uint8_t value) noexcept {
	CPU().lower(CPU().MREQ());
	CPU().raise(CPU().IORQ());
	return EightBit::Bus::poke(address, value);
}

EightBit::MemoryMapping Board::mapping(const uint16_t address) noexcept {

	if (CPU().requestingMemory()) {
		if (address < 0x4000)
			return { ROM(), 0x0000, 0xffff, EightBit::MemoryMapping::AccessLevel::ReadOnly };
		if (address < 0x8000)
			return { VRAM(), 0x4000, 0xffff, EightBit::MemoryMapping::AccessLevel::ReadWrite };
		return { WRAM(), 0x8000, 0xffff,  EightBit::MemoryMapping::AccessLevel::ReadWrite };
	}

	if (CPU().requestingIO()) {
		if (CPU().requestingRead())
			ports().accessType() = EightBit::InputOutput::AccessType::Reading;
		if (CPU().requestingWrite())
			ports().accessType() = EightBit::InputOutput::AccessType::Writing;
		return { ports(), 0x00, 0xff, EightBit::MemoryMapping::AccessLevel::ReadWrite };
	}

	UNREACHABLE;
}

void Board::renderLines() {
	ULA().renderLines();
}

void Board::runCycle() {
	const int taken = CPU().run(++m_allowed);
	m_allowed -= taken;
}
