#pragma once

#include <array>
#include <string>
#include <cstdint>

#include "SnapshotFile.h"

class Board;

// https://www.worldofspectrum.org/faq/reference/z80format.htm

class Z80File final : public SnapshotFile {
private:
	const static int Impossible = 0x100; // impossible value for a byte

	int m_version = 0;	// Illegal, by default!
	uint8_t m_misc1 = 0;
	uint8_t m_misc2 = 0;
	std::array<int, 4> m_window = { Impossible, Impossible, Impossible, Impossible };

	constexpr void reset_window() noexcept {
		m_window = { Impossible, Impossible, Impossible, Impossible };
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

	[[nodiscard]] auto fetchByteWindowed() noexcept {
		const auto current = fetchByte();
		adjust_window(current);
		return current;
	}

	void loadMemoryV1(Board& board);
	void loadMemoryCompressedV1(Board& board);
	void loadMemoryUncompressed(Board& board);

	[[nodiscard]] constexpr auto version() const noexcept { return m_version; }

	[[nodiscard]] constexpr auto misc1() const noexcept { return m_misc1; }
	[[nodiscard]] constexpr auto refresh_high() const noexcept { return misc1() & EightBit::Chip::Mask1; }
	[[nodiscard]] constexpr auto border() const noexcept { return (misc1() >> 1) & EightBit::Chip::Mask3; }
	[[nodiscard]] constexpr auto compressed() const noexcept { return (misc1() & EightBit::Chip::Bit5) != 0; }	// Only valid for V1

	[[nodiscard]] constexpr auto misc2() const noexcept { return m_misc2; }
	[[nodiscard]] constexpr auto im() const noexcept { return misc2() & EightBit::Chip::Mask2; }

protected:
	void loadRegisters(EightBit::Z80& cpu) final;
	void loadMemory(Board& board) final;

public:
	Z80File(std::string path);

	void load(Board& board) final;
};
