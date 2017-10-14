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

void Ula::renderBlank(const int y) {
	std::fill_n(
		m_pixels.begin() + y * RasterWidth,
		(int)RasterWidth,
		m_borderColour);
}

void Ula::renderLeftHorizontalBorder(const int y) {
	std::fill_n(
		m_pixels.begin() + y * RasterWidth,
		(int)LeftRasterBorder,
		m_borderColour);
}

void Ula::renderRightHorizontalBorder(const int y) {
	std::fill_n(
		m_pixels.begin() + y * RasterWidth + LeftRasterBorder + ActiveRasterWidth,
		(int)RightRasterBorder,
		m_borderColour);
}

void Ula::renderActive(const int absoluteY) {

	assert(absoluteY < RasterHeight);

	const auto y = absoluteY - UpperRasterBorder;
	assert((y < 192) && (y >= 0));

	const auto high = y >> 6;
	assert((high < 3) && (high >= 0));

	auto medium = (y >> 3) & EightBit::Processor::Mask3;
	assert((medium < 8) && (medium >= 0));

	const auto low = y & EightBit::Processor::Mask3;
	assert((low < 8) && (low >= 0));

	const auto bitmapAddressY =
		BitmapAddress
		| (high << 11)
		| (low << 8)
		| (medium << 5);

	const auto attributeAddressY = AttributeAddress + (y % 32);

	const auto pixelBase = LeftRasterBorder + absoluteY * RasterWidth;

	for (int byte = 0; byte < BytesPerLine; ++byte) {

		const auto bitmapAddress = bitmapAddressY + byte;
		const auto bitmap = m_bus.peek(bitmapAddress);

		const auto attributeAddress = attributeAddressY + byte;
		const auto attribute = m_bus.peek(attributeAddress);

		const auto ink = attribute & EightBit::Processor::Mask3;
		const auto paper = (attribute >> 3) & EightBit::Processor::Mask3;
		const auto bright = !!(attribute & EightBit::Processor::Bit6);
		const auto flash = !!(attribute & EightBit::Processor::Bit7);

		const auto background = m_palette.getColour(paper, bright);
		const auto foreground = m_palette.getColour(ink, bright);

		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = bitmap & (1 << bit);
			const auto x = (~bit & EightBit::Processor::Mask3) | (byte << 3);

			m_pixels[pixelBase + x] = pixel ? foreground : background;
		}
	}
}

void Ula::render(const int absoluteY) {
	renderLeftHorizontalBorder(absoluteY);
	renderActive(absoluteY);
	renderRightHorizontalBorder(absoluteY);
}

void Ula::Board_WrittenPort(const EightBit::PortEventArgs& event) {
	const auto port = event.getPort();
	const auto value = m_bus.ports().readOutputPort(port);
	switch (port) {
	case 0xfe:
		m_borderColour = m_palette.getColour(value, false);
		break;
	}
}
