#include "stdafx.h"
#include "Z80File.h"
#include "Board.h"

#include <stdexcept>

Z80File::Z80File(const std::string path)
: SnapshotFile(path) {}

void Z80File::loadRegisters(EightBit::Z80& cpu) {

	resetPosition();

	cpu.raiseRESET();

	// V1

	cpu.A() = fetchByte();
	cpu.F() = fetchByte();

	cpu.BC() = fetchWord();
	cpu.HL() = fetchWord();
	cpu.PC() = fetchWord();
	m_version = cpu.PC() == 0 ? 2 : 1;

	cpu.SP() = fetchWord();

	cpu.IV() = fetchByte();

	cpu.REFRESH().variable = fetchByte();
	m_misc1 = fetchByte();
	m_misc1 = m_misc1 == 0xff ? 1 : m_misc1;
	cpu.REFRESH().high = refresh_high();

	cpu.DE() = fetchWord();

	cpu.exx();

	cpu.BC() = fetchWord();
	cpu.DE() = fetchWord();
	cpu.HL() = fetchWord();

	cpu.exxAF();

	cpu.A() = fetchByte();
	cpu.F() = fetchByte();

	cpu.IY() = fetchWord();
	cpu.IX() = fetchWord();

	cpu.IFF1() = fetchByte();
	cpu.IFF2() = fetchByte();

	m_misc2 = fetchByte();
	cpu.IM() = im();

	cpu.exx();
	cpu.exxAF();

	assert(version() >= 1);

	if (version() == 1) return;

	m_additionalHeaderLength = fetchWord();
	m_version = additionalHeaderLength() == 23 ? 2 : 3;

	cpu.PC() = fetchWord();

	m_hardwareMode = fetchByte();
	if (hardwareMode() != HardwareMode::k48)
		throw std::runtime_error("Only 48K ZX Spectrum is supported");

	fetchByte(); // offset 35
	fetchByte(); // offset 36

	m_emulationMode = fetchByte();

	fetchByte(); // offset 38, soundchip register number
	for (int i = 0; i < 16; ++i)
		fetchByte(); // offset 39 - 54, sound chip registers

	assert(version() == 2);
}

void Z80File::loadMemoryCompressedV1(Board& board) {
	reset_window();
	auto destination = board.ROM().size();
	while (true) {
		const auto current = fetchByteWindowed();
		if (compressed_window()) {
			const uint8_t repeats = fetchByteWindowed();
			if (finished_window()) break;
			const uint8_t value = fetchByteWindowed();
			--destination;	// Overwrite the initial ED of the compressed marker
			for (int j = 0; j < repeats; ++j)
				board.poke(destination++, value);
		} else {
			board.poke(destination++, current);
		}
	}
}

void Z80File::loadMemoryUncompressed(Board& board) {
	auto destination = board.ROM().size();
	while (!finished())
		board.poke(destination++, fetchByte());
}

void Z80File::loadMemoryV1(Board& board) {
	if (compressed())
		loadMemoryCompressedV1(board);
	else
		loadMemoryUncompressed(board);
}

void Z80File::loadMemoryCompressedV2(Board& board) {
	assert(hardwareMode() == HardwareMode::k48);
	const auto length = fetchWord();
	const auto page = fetchByte();
	reset_window();
	auto destination = m_block_addresses_48k[page];
	assert(destination != Impossible16);
	auto remaining = length;
	while (remaining > 0) {
		const auto current = fetchByteWindowed();
		--remaining;
		if (compressed_window()) {
			const uint8_t repeats = fetchByteWindowed();
			--remaining;
			const uint8_t value = fetchByteWindowed();
			--remaining;
			--destination;	// Overwrite the initial ED of the compressed marker
			for (int j = 0; j < repeats; ++j)
				board.poke(destination++, value);
		}
		else {
			board.poke(destination++, current);
		}
	}
}

void Z80File::loadMemoryV2(Board& board) {
	while (!finished())
		loadMemoryCompressedV2(board);
}

void Z80File::loadMemory(Board& board) {
	switch (version()) {
	case 1:
		loadMemoryV1(board);
		break;
	case 2:
		loadMemoryV2(board);
		break;
	default:
		throw std::runtime_error("Only V1 or V2 Z80 files are handled.");
		break;
	}
}

void Z80File::load(Board& board) {
	SnapshotFile::load(board);
	board.ULA().setBorder(border());
}
