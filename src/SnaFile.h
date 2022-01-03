#pragma once

#include <cstdint>
#include <string>

#include "LittleEndianContent.h"

class Board;

namespace EightBit {
	class Z80;
}

// https://worldofspectrum.org/faq/reference/formats.htm#File

class SnaFile final : public LittleEndianContent {
private:
	const std::string m_path;
	uint8_t m_border = 0xff;

	[[nodiscard]] constexpr auto border() const noexcept { return m_border; }
	[[nodiscard]] auto path() const { return m_path; }

	void loadRegisters(EightBit::Z80& cpu);
	void loadMemory(Board& board);

public:
	SnaFile(std::string path);

	void load(Board& board);
};
