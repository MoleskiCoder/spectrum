#include "stdafx.h"
#include "Ula.h"

#include <Processor.h>

#include "Board.h"
#include "ColourPalette.h"

Ula::Ula(const ColourPalette& palette, Board& bus)
: m_palette(palette),
  m_bus(bus) {
}

const std::vector<uint32_t>& Ula::pixels() const {
	return m_pixels;
}

void Ula::initialise() {
	m_pixels.resize(RasterWidth * RasterHeight);
}

void Ula::render(int y) {

	auto verticalBlock = y / 64;
	assert((verticalBlock < 3) && (verticalBlock >= 0));

	auto verticalCharacterLine = y % 64 / 8;
	assert((verticalCharacterLine < 8) && (verticalCharacterLine >= 0));

	auto verticalScanLine = y % 64 % 8;
	assert((verticalScanLine < 8) && (verticalScanLine >= 0));

	const auto bytesPerLine = RasterWidth / 8;

	const auto addressY =
		BitmapAddress
		| verticalBlock << 11
		| verticalScanLine << 8
		| verticalCharacterLine << 5;

	for (int byte = 0; byte < bytesPerLine; ++byte) {

		auto address = addressY + byte;
		auto row = m_bus.peek(address);

		auto bright = false;
		auto pen = ColourPalette::Black;
		auto paper = ColourPalette::Cyan;

		auto background = m_palette.getColour(paper, bright);
		auto foreground = m_palette.getColour(pen, bright);

		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = row & (1 << bit);
			const auto x = (7 - bit) + byte * 8;

			m_pixels[x + y * RasterWidth] = pixel ? foreground : background;
		}
	}
}