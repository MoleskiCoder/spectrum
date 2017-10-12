#include "stdafx.h"
#include "Ula.h"

#include <Processor.h>
#include <cassert>

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
	m_bus.ports().WrittenPort.connect(std::bind(&Ula::Board_WrittenPort, this, std::placeholders::_1));
}

void Ula::renderBlank(int y) {
	std::fill_n(m_pixels.begin() + y * RasterWidth, (int)RasterWidth, m_borderColour);
}

void Ula::renderLeftHorizontalBorder(int y) {
	std::fill_n(
		m_pixels.begin() + y * RasterWidth,
		(int)LeftRasterBorder,
		m_borderColour);
}

void Ula::renderRightHorizontalBorder(int y) {
	std::fill_n(
		m_pixels.begin() + y * RasterWidth + LeftRasterBorder + ActiveRasterWidth,
		(int)LeftRasterBorder,
		m_borderColour);
}

void Ula::renderActive(int absoluteY) {

	auto y = absoluteY - UpperRasterBorder;

	auto verticalBlock = y / 64;
	assert((verticalBlock < 3) && (verticalBlock >= 0));

	auto verticalCharacterLine = y % 64 / 8;
	assert((verticalCharacterLine < 8) && (verticalCharacterLine >= 0));

	auto verticalScanLine = y % 64 % 8;
	assert((verticalScanLine < 8) && (verticalScanLine >= 0));

	const auto bytesPerLine = ActiveRasterWidth / 8;

	const auto addressY =
		BitmapAddress
		| verticalBlock << 11
		| verticalScanLine << 8
		| verticalCharacterLine << 5;

	for (int byte = 0; byte < bytesPerLine; ++byte) {

		auto bitmapAddress = addressY + byte;
		auto row = m_bus.peek(bitmapAddress);

		auto attributeAddress = AttributeAddress + byte + y % 24;
		auto attribute = m_bus.peek(attributeAddress);

		auto ink = attribute & EightBit::Processor::Mask3;
		auto paper = (attribute >> 3) & EightBit::Processor::Mask3;
		auto bright = !!(attribute & EightBit::Processor::Bit6);
		auto flash = !!(attribute & EightBit::Processor::Bit7);

		auto background = m_palette.getColour(paper, bright);
		auto foreground = m_palette.getColour(ink, bright);

		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = row & (1 << bit);
			const auto x = (7 - bit) + byte * 8;

			m_pixels[x + LeftRasterBorder + (y + UpperRasterBorder) * RasterWidth] = pixel ? foreground : background;
		}
	}
}

void Ula::render(int absoluteY) {
	renderLeftHorizontalBorder(absoluteY);
	renderActive(absoluteY);
	renderRightHorizontalBorder(absoluteY);
}

void Ula::Board_WrittenPort(const EightBit::PortEventArgs& event) {
	auto port = event.getPort();
	switch (port) {
	case 0xfe:
		m_borderColour = m_palette.getColour(m_bus.ports().readOutputPort(port), false);
		break;
	default:
		break;
	}
}
