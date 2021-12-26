#include "stdafx.h"
#include "Z80File.h"
#include "Board.h"

#include <stdexcept>

Z80File::Z80File(const std::string path)
: SnapshotFile(path) {}

uint8_t Z80File::misc1() const {
	const auto misc1 = peek(Offset_misc_1);
	return misc1 == 0xff ? 1 : misc1;
}

void Z80File::examineHeaders() {
	switch (peekWord(Offset_PC)) {
	case 0:
		m_version = 2;
		break;
	default:
		m_version = 1;
		break;
	}
}

void Z80File::loadRegisters(EightBit::Z80& cpu) const {

	cpu.raiseRESET();

	cpu.A() = peek(Offset_A);
	cpu.F() = peek(Offset_F);

	cpu.BC().word = peekWord(Offset_BC);
	cpu.HL().word = peekWord(Offset_HL);
	cpu.PC().word = m_version == 1 ? peekWord(Offset_PC) : peekWord(Offset_additional_PC);
	cpu.SP().word = peekWord(Offset_SP);

	cpu.IV() = peek(Offset_I);

	cpu.REFRESH().variable = peek(Offset_R);

	cpu.REFRESH().high = misc1() & EightBit::Chip::Mask1;

	cpu.DE().word = peekWord(Offset_DE);

	cpu.exx();

	cpu.BC().word = peekWord(Offset_BC_);
	cpu.DE().word = peekWord(Offset_DE_);
	cpu.HL().word = peekWord(Offset_HL_);

	cpu.exxAF();

	cpu.A() = peek(Offset_A_);
	cpu.F() = peek(Offset_F_);

	cpu.IY().word = peekWord(Offset_IY);
	cpu.IX().word = peekWord(Offset_IX);

	cpu.IFF1() = peek(Offset_IFF1);
	cpu.IFF2() = peek(Offset_IFF2);

	const auto misc2 = peek(Offset_misc_2);
	cpu.IM() = misc2 & EightBit::Chip::Mask2;

	cpu.exx();
	cpu.exxAF();
}

void Z80File::loadMemoryCompressedV1(Board& board, size_t offset) const {
	const size_t position = board.ROM().size();
	const auto fileSize = size() - 4;
	loadCompressedBlock(board, offset, position, fileSize);
}

uint16_t Z80File::loadCompressedBlock(Board& board, uint16_t source) const {
	const auto length = peekWord(source);
	const auto block = peek(source + 2);
	loadCompressedBlock(board, source + 3, block * 0x4000, length);
	return length;
}

void Z80File::loadCompressedBlock(Board& board, uint16_t source, uint16_t destination, uint16_t length) const {
	int previous = 0x100;
	for (size_t i = source; i != length; ++i) {
		const auto current = peek(i);
		if (current == 0xed && previous == 0xed) {
			const uint8_t repeats = peek(++i);
			const uint8_t value = peek(++i);
			--destination;
			for (int j = 0; j < repeats; ++j)
				board.poke(destination++, value);
			previous = 0x100;
		} else {
			board.poke(destination++, current);
			previous = current;
		}
	}
}

void Z80File::loadMemoryUncompressed(Board& board, size_t offset) const {
	for (int i = 0; i < RamSize; ++i)
		board.poke(board.ROM().size() + i, peek(offset + i));
}

void Z80File::loadMemoryV1(Board& board) const {
	const bool compressed = (misc1() & EightBit::Chip::Bit5) != 0;
	if (compressed)
		loadMemoryCompressedV1(board, HeaderSizeV1);
	else
		loadMemoryUncompressed(board, HeaderSizeV1);
}

void Z80File::loadMemoryV2(Board& board) const {
	const auto extra = peekWord(Offset_additional_header_block_length) + 2;
	auto position = HeaderSizeV1 + extra + 2;
	while (position < size()) {
		const auto size = loadCompressedBlock(board, position);
		position += size;
	}
}

void Z80File::loadMemory(Board& board) const {
	switch (m_version) {
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
	board.ULA().setBorder((misc1() >> 1) & EightBit::Chip::Mask3);
}
