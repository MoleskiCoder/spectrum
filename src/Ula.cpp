#include "stdafx.h"
#include "Ula.h"

#include <Processor.h>
#include <cassert>

#include "Board.h"
#include "ColourPalette.h"

Ula::Ula(const ColourPalette& palette, Board& bus)
: m_palette(palette),
  m_bus(bus),
  m_flash(false) {
}

const std::vector<uint32_t>& Ula::pixels() const {
	return m_pixels;
}

void Ula::initialise() {
	m_pixels.resize(RasterWidth * RasterHeight);
	initialiseKeyboardMapping();
	m_bus.ports().ReadingPort.connect(std::bind(&Ula::Board_ReadingPort, this, std::placeholders::_1));
	m_bus.ports().WrittenPort.connect(std::bind(&Ula::Board_WrittenPort, this, std::placeholders::_1));

	auto line = 0;
	for (auto p = 0; p < 4; ++p) {
		for (auto y = 0; y < 8; ++y) {
			for (auto o = 0; o < 8; ++o, ++line) {
				m_scanLineAddresses[line] = BitmapAddress + (p << 11) + (y << 5) + (o << 8);
				m_attributeAddresses[line] = AttributeAddress + (((p << 3) + y) << 5);
			}
		}
	}
}

void Ula::flash() {
	m_flash = !m_flash;
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
	assert(y < ActiveRasterHeight);

	const auto bitmapAddressY = m_scanLineAddresses[y];
	const auto attributeAddressY = m_attributeAddresses[y];

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

		const auto background = m_palette.getColour(flash && m_flash ? ink : paper, bright);
		const auto foreground = m_palette.getColour(flash && m_flash ? paper : ink, bright);

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

void Ula::pokeKey(SDL_Keycode raw) {
	m_keyboardRaw.emplace(raw);
}

void Ula::pullKey(SDL_Keycode raw) {
	m_keyboardRaw.erase(raw);
}

void Ula::initialiseKeyboardMapping() {

	// Left side
	m_keyboardMapping[0xF7] = { SDLK_1,		SDLK_2,     SDLK_3,     SDLK_4,		SDLK_5,		};
	m_keyboardMapping[0xFB] = { SDLK_q,		SDLK_w,		SDLK_e,		SDLK_r,		SDLK_t		};
	m_keyboardMapping[0xFD] = { SDLK_a,     SDLK_s,     SDLK_d,		SDLK_f,		SDLK_g		};
	m_keyboardMapping[0xFE] = { SDLK_LSHIFT,SDLK_z,		SDLK_x,		SDLK_c,		SDLK_v		};

	// Right side
	m_keyboardMapping[0xEF] = { SDLK_0,		SDLK_9,		SDLK_8,		SDLK_7,		SDLK_6		};
	m_keyboardMapping[0xDF] = { SDLK_p,     SDLK_o,     SDLK_i,		SDLK_u,		SDLK_y		};
	m_keyboardMapping[0xBF] = { SDLK_RETURN,SDLK_l,		SDLK_k,		SDLK_j,		SDLK_h		};
	m_keyboardMapping[0x7F] = { SDLK_SPACE,	SDLK_RSHIFT,SDLK_m,		SDLK_n,		SDLK_b		};
}

uint8_t Ula::findSelectedKeys(uint8_t row) const {
	auto pKeys = m_keyboardMapping.find(row);
	if (pKeys == m_keyboardMapping.cend())
		throw std::runtime_error("Invalid keyboard row selected.");
	uint8_t returned = 0xff;
	const auto& keys = pKeys->second;
	for (int column = 0; column < 5; ++column) {
		if (m_keyboardRaw.find(keys[column]) != m_keyboardRaw.cend())
			returned &= ~(1 << column);
	}
	return returned;
}

void Ula::Board_ReadingPort(const EightBit::PortEventArgs& event) {

	const auto port = event.getPort();
	const auto ignored = !!(port & EightBit::Processor::Bit0);
	if (ignored)
		return;

	const auto portHigh = m_bus.ADDRESS().high;
	auto selected = findSelectedKeys(portHigh);
	m_bus.ports().writeInputPort(port, selected & EightBit::Processor::Mask5);
}

void Ula::Board_WrittenPort(const EightBit::PortEventArgs& event) {
	const auto port = event.getPort();
	const auto ignored = !!(port & EightBit::Processor::Bit0);
	if (ignored)
		return;

	const auto value = m_bus.ports().readOutputPort(port);
	m_borderColour = m_palette.getColour(value & EightBit::Processor::Mask3, false);
}
