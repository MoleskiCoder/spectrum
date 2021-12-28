#pragma once

#include <string>

#include "SnapshotFile.h"

class Board;

namespace EightBit {
	class Z80;
}

// https://worldofspectrum.org/faq/reference/formats.htm#File

class SnaFile final : public SnapshotFile {
private:
	const static size_t RamSize = (32 + 16) * 1024;

	uint8_t m_border = 0xff;

	[[nodiscard]] constexpr auto border() const noexcept { return m_border; }

protected:
	void loadRegisters(EightBit::Z80& cpu) final;
	void loadMemory(Board& board) final;

public:
	SnaFile(std::string path);

	void load(Board& board) final;
};
