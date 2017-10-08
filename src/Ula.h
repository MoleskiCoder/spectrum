#pragma once

#include <vector>
#include <cstdint>

class Board;
class ColourPalette;

class Ula {
public:
	enum {
		UpperBorder = 64,
		LowerBorder = 56,
		RasterWidth = 256,
		RasterHeight = 192,
		TotalLineCount = UpperBorder + RasterHeight + LowerBorder,

		CyclesPerSecond = 3500000,	// 3.5Mhz
		FramesPerSecond = 50,
		CyclesPerFrame = CyclesPerSecond / FramesPerSecond,

		HorizontalDrawCycles = 128,
		HorizontalBlankCycles = 96,
		HorizontalCyclesTotal = HorizontalDrawCycles + HorizontalBlankCycles,
	};

	Ula(const ColourPalette& palette, Board& bus);

	void initialise();
	void render(int scanLine);

	const std::vector<uint32_t>& pixels() const;

private:
	std::vector<uint32_t> m_pixels;
	const ColourPalette& m_palette;
	Board& m_bus;
};
