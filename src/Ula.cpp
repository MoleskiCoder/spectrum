#include "stdafx.h"
#include "Ula.h"
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

void Ula::render(int scanLine) {
}