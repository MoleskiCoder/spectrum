#pragma once

#include <array>
#include <string>
#include <cstdint>

#include <Chip.h>
#include <Register.h>

#include "LittleEndianContent.h"

namespace EightBit {
	class Z80;
}

class Board;

// https://www.worldofspectrum.org/faq/reference/z80format.htm

class Z80File final : public LittleEndianContent {
private:
	enum HardwareMode { k48, k48_if1, kSamRam, k128, k128_if1 };

	const static int Impossible8 = 0x100; // impossible value for a byte
	const static int Impossible16 = 0x10000; // impossible value for a word

	const std::string m_path;
	int m_version = 0;	// Illegal, by default!
	uint8_t m_misc1 = 0;
	uint8_t m_misc2 = 0;
	EightBit::register16_t m_additionalHeaderLength = { 0 };
	uint8_t m_hardwareMode = 0;
	uint8_t m_emulationMode = 0;

	std::array<int, 4> m_window = { Impossible8, Impossible8, Impossible8, Impossible8 };

	const std::array<int, 12> m_block_addresses_48k = {
		0,				// 0	(48K ROM)
		Impossible16,	// 1	(Interface I, Disciple or Plus D ROM)
		Impossible16,	// 2
		Impossible16,	// 3
		0x8000,			// 4
		0xc000,			// 5
		Impossible16,	// 6
		Impossible16,	// 7
		0x4000,			// 8
		Impossible16,	// 9
		Impossible16,	// 10
		Impossible16,	// 11	(Multiface ROM)
	};


	constexpr void reset_window() noexcept {
		m_window = { Impossible8, Impossible8, Impossible8, Impossible8 };
	}

	[[nodiscard]] constexpr static bool compressed_window(const std::array<int, 4>& window) noexcept {
		return (window[0] == 0xed) && (window[1] == 0xed);
	}

	[[nodiscard]] constexpr bool compressed_window() const noexcept {
		return compressed_window(m_window);
	}

	[[nodiscard]] constexpr static bool finished_window(const std::array<int, 4>& window) noexcept {
		const std::array<int, 4> compare = { 0x00, 0xed, 0xed, 0x00 };
		return window == compare;
	}

	[[nodiscard]] constexpr bool finished_window() const noexcept {
		return finished_window(m_window);
	}

	constexpr static void adjust(std::array<int, 4>& window, int current) noexcept {
		for (int i = 2; i >= 0; --i)
			window[i + 1] = window[i];
		window[0] = current;
	}

	constexpr void adjust_window(int current) noexcept {
		adjust(m_window, current);
	}

	[[nodiscard]] constexpr auto fetchByteWindowed() noexcept {
		const auto current = fetchByte();
		adjust_window(current);
		return current;
	}

	void loadMemoryV1(Board& board);
	void loadMemoryCompressedV1(Board& board);
	void loadMemoryUncompressed(Board& board);

	void loadMemoryV2(Board& board);
	void loadMemoryCompressedV2(Board& board);

	[[nodiscard]] constexpr auto version() const noexcept { return m_version; }

	[[nodiscard]] constexpr auto misc1() const noexcept { return m_misc1; }
	[[nodiscard]] constexpr auto refresh_high() const noexcept { return misc1() & EightBit::Chip::Mask1; }
	[[nodiscard]] constexpr auto border() const noexcept { return (misc1() >> 1) & EightBit::Chip::Mask3; }
	[[nodiscard]] constexpr auto compressed() const noexcept { return (misc1() & EightBit::Chip::Bit5) != 0; }	// Only valid for V1

	[[nodiscard]] constexpr auto misc2() const noexcept { return m_misc2; }
	[[nodiscard]] constexpr auto im() const noexcept { return misc2() & EightBit::Chip::Mask2; }

	[[nodiscard]] constexpr auto additionalHeaderLength() const noexcept { return m_additionalHeaderLength; }

	[[nodiscard]] constexpr auto hardwareMode() const noexcept { return m_hardwareMode; }

	[[nodiscard]] auto path() const { return m_path; }

	void loadRegisters(EightBit::Z80& cpu);
	void loadMemory(Board& board);

public:
	Z80File(std::string path);

	void load(Board& board);
};
