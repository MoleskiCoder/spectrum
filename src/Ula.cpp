#include "stdafx.h"
#include "Ula.h"

#include <Chip.h>
#include <cassert>

#include "Board.h"
#include "ColourPalette.h"

Ula::Ula(const ColourPalette& palette, Board& bus)
: m_palette(palette),
  m_bus(bus) {
}

void Ula::initialise() {
	initialiseKeyboardMapping();
	BUS().ports().ReadingPort.connect(std::bind(&Ula::Board_ReadingPort, this, std::placeholders::_1));
	BUS().ports().WrittenPort.connect(std::bind(&Ula::Board_WrittenPort, this, std::placeholders::_1));

	auto line = 0;
	for (auto p = 0; p < 4; ++p) {
		for (auto y = 0; y < 8; ++y) {
			for (auto o = 0; o < 8; ++o, ++line) {
				m_scanLineAddresses[line] = (p << 11) + (y << 5) + (o << 8);
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
		const auto bitmap = BUS().VRAM().peek(bitmapAddress);

		const auto attributeAddress = attributeAddressY + byte;
		const auto attribute = BUS().VRAM().peek(attributeAddress);

		const auto ink = attribute & EightBit::Chip::Mask3;
		const auto paper = (attribute >> 3) & EightBit::Chip::Mask3;
		const auto bright = !!(attribute & EightBit::Chip::Bit6);
		const auto flash = !!(attribute & EightBit::Chip::Bit7);

		const auto background = m_palette.getColour(flash && m_flash ? ink : paper, bright);
		const auto foreground = m_palette.getColour(flash && m_flash ? paper : ink, bright);

		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = bitmap & (1 << bit);
			const auto x = (~bit & EightBit::Chip::Mask3) | (byte << 3);

			m_pixels[pixelBase + x] = pixel ? foreground : background;
		}
	}
}

void Ula::render(const int absoluteY) {
	renderLeftHorizontalBorder(absoluteY);
	renderActive(absoluteY);
	renderRightHorizontalBorder(absoluteY);
}

void Ula::finishFrame() {
	if ((++m_frameCounter & EightBit::Chip::Mask4) == 0)
		flash();
	BUS().CPU().lowerINT();
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
	uint8_t returned = 0xff;
	auto pKeys = m_keyboardMapping.find(row);
	if (pKeys != m_keyboardMapping.cend()) {
		const auto& keys = pKeys->second;
		for (int column = 0; column < 5; ++column) {
			if (m_keyboardRaw.find(keys[column]) != m_keyboardRaw.cend())
				returned &= ~(1 << column);
		}
	}
	return returned;
}

void Ula::Board_ReadingPort(const uint8_t& port) {
	maybeReadingPort(port);
}

void Ula::Board_WrittenPort(const uint8_t& port) {
	maybeWrittenPort(port);
}

void Ula::maybeWrittenPort(const uint8_t port) {
	if (ignoredPort(port))
		return;
	writtenPort(port);
}

void Ula::writtenPort(const uint8_t port) {
	const auto value = BUS().ports().readOutputPort(port);
	m_borderColour = m_palette.getColour(value & EightBit::Chip::Mask3, false);
	m_mic = !!(value & EightBit::Chip::Bit3);
	m_speaker = !!(value & EightBit::Chip::Bit4);
}

void Ula::maybeReadingPort(const uint8_t port) {
	if (ignoredPort(port))
		return;
	readingPort(port);
}

void Ula::readingPort(const uint8_t port) {
	const auto portHigh = BUS().ADDRESS().high;
	m_selected = findSelectedKeys(portHigh);
	const uint8_t value = (m_selected & EightBit::Chip::Mask5) | (m_ear ? 1 << 6 : 0);
	BUS().ports().writeInputPort(port, value);
}

bool Ula::ignoredPort(const uint8_t port) const {
	return !!(port & EightBit::Chip::Bit0);
}