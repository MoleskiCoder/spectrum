#pragma once

#include <string>
#include <cstdint>

#include "SnapshotFile.h"

class Board;

// https://www.worldofspectrum.org/faq/reference/z80format.htm

class Z80File final : public SnapshotFile {
private:

	// V1 Header block

	const static size_t Offset_A = 0;
	const static size_t Offset_F = 1;
	const static size_t Offset_BC = 2;
	const static size_t Offset_HL = 4;
	const static size_t Offset_PC = 6;
	const static size_t Offset_SP = 8;
	const static size_t Offset_I = 10;
	const static size_t Offset_R = 11; // Bit 7 is not significant!

	// Bit 0	 : Bit 7 of the R - register
	// Bit 1 - 3 : Border colour
	// Bit 4     : 1 = Basic SamRom switched in
	// Bit 5     : 1 = Block of data is compressed
	// Bit 6 - 7 : No meaning
	const static size_t Offset_misc_1 = 12;

	const static size_t Offset_DE = 13;
	const static size_t Offset_BC_ = 15;
	const static size_t Offset_DE_ = 17;
	const static size_t Offset_HL_ = 19;
	const static size_t Offset_A_ = 21;
	const static size_t Offset_F_ = 22;
	const static size_t Offset_IY = 23;
	const static size_t Offset_IX = 25;
	const static size_t Offset_IFF1 = 27;
	const static size_t Offset_IFF2 = 28;

	// Bit 0 - 1 : Interrupt mode(0, 1 or 2)
	// Bit 2     : 1 = Issue 2 emulation
	// Bit 3     : 1 = Double interrupt frequency
	// Bit 4 - 5 : 1 = High video synchronisation
	//             3 = Low video synchronisation
	//             0, 2 = Normal
	// Bit 6 - 7 : 0 = Cursor / Protek / AGF joystick
	//             1 = Kempston joystick
	//             2 = Sinclair 2 Left joystick(or user
	//	               defined, for version 3.z80 files)
	//             3 = Sinclair 2 Right joystick
	const static size_t Offset_misc_2 = 29;

	const static size_t HeaderSizeV1 = Offset_misc_2 + 1;

	// V2 Header block

	//   * 30      2       Length of additional header block (see below)
	const static size_t Offset_additional_header_block_length = 30;

	//   * 32      2       Program counter
	const static size_t Offset_additional_PC = 32;

	//   * 34      1       Hardware mode (see below)
	const static size_t Offset_additional_hardware_mode = 34;

	//   * 35      1       If in SamRam mode, bitwise state of 74ls259.
	//                     For example, bit 6=1 after an OUT 31,13 (=2*6+1)
	//                     If in 128 mode, contains last OUT to 7ffd
	const static size_t Offset_additional_128_out_7ffd = 35;

	//   * 36      1       Contains 0FF if Interface I rom paged
	const static size_t Offset_additional_interface_I_paged = 36;

	//   * 37      1       Bit 0: 1 if R register emulation on
	//                     Bit 1: 1 if LDIR emulation on
	const static size_t Offset_additional_R_LDIR_emulation_on = 37;

	//   * 38      1       Last OUT to fffd (soundchip register number)
	const static size_t Offset_additional_last_fffd_out = 38;

	//   * 39      16      Contents of the sound chip registers
	const static size_t Offset_additional_sound_chip_registers = 39;


	// V3 Header block

	//     55      2       Low T state counter
	//     57      1       Hi T state counter
	//     58      1       Flag byte used by Spectator (QL spec. emulator)
	//                     Ignored by Z80 when loading, zero when saving
	//     59      1       0FF if MGT Rom paged
	//     60      1       0FF if Multiface Rom paged. Should always be 0.
	//     61      1       0FF if 0-8191 is ROM, 0 if RAM
	//     62      1       0FF if 8192-16383 is ROM, 0 if RAM
	//     63      10      5x keyboard mappings for user defined joystick
	//     73      10      5x ascii word: keys corresponding to mappings above
	//     83      1       MGT type: 0=Disciple+Epson,1=Disciple+HP,16=Plus D
	//     84      1       Disciple inhibit button status: 0=out, 0ff=in
	//     85      1       Disciple inhibit flag: 0=rom pageable, 0ff=not

	const static size_t RamSize = (32 + 16) * 1024;

	int m_version = 0;	// Illegal, by default!

	[[nodiscard]] uint8_t misc1() const;

	void loadMemoryV1(Board& board) const;
	void loadMemoryV2(Board& board) const;
	void loadMemoryCompressedV1(Board& board, size_t offset) const;
	void loadMemoryUncompressed(Board& board, size_t offset) const;

	uint16_t loadCompressedBlock(Board& board, uint16_t source) const;
	void loadCompressedBlock(Board& board, uint16_t source, uint16_t destination, uint16_t length) const;

protected:
	void examineHeaders() override;
	void loadRegisters(EightBit::Z80& cpu) const final;
	void loadMemory(Board& board) const final;

public:
	Z80File(std::string path);

	virtual void load(Board& board) final;
};
