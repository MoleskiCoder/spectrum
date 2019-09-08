#include "stdafx.h"
#include "Ula.h"

#include <Chip.h>
#include <cassert>

#include "Board.h"
#include "ColourPalette.h"

Ula::Ula(const ColourPalette& palette, Board& bus)
: m_palette(palette),
  m_bus(bus) {

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

	RaisedPOWER.connect([this](EightBit::EventArgs) {
		m_frameCounter = 0;
		m_borderColour = 0;
		m_flash = false;
	});

	Ticked.connect([this](EightBit::EventArgs) {
		const auto available = cycles() / 2;
		if (available > 0) {
			proceed(cycles() / 2);
			resetCycles();
		}
	});
}

void Ula::flash() {
	m_flash = !m_flash;
}

void Ula::renderBlankLine(const int y) {
	renderHorizontalBorder(0, y, RasterWidth);
}

void Ula::renderLeftHorizontalBorder(const int y) {
	renderHorizontalBorder(0, y);
}

void Ula::renderRightHorizontalBorder(const int y) {
	renderHorizontalBorder(HorizontalRasterBorder + ActiveRasterWidth, y);
}

void Ula::renderHorizontalBorder(int x, int y, int width) {
	const size_t begin = y * RasterWidth + x;
	for (int i = 0; i < width; ++i) {
		m_pixels[begin + i] = m_borderColour;
		tick();
	}
}

void Ula::renderVRAM(const int y) {

	assert(y >= 0);
	assert(y < ActiveRasterHeight);

	// Position in VRAM
	const auto bitmapAddressY = m_scanLineAddresses[y];
	const auto attributeAddressY = m_attributeAddresses[y];

	// Position in pixel render 
	const auto pixelBase = HorizontalRasterBorder + (y + UpperRasterBorder) * RasterWidth;

	for (int byte = 0; byte < BytesPerLine; ++byte) {

		const auto bitmapAddress = bitmapAddressY + byte;
		const auto bitmap = BUS().VRAM().peek(bitmapAddress);

		const auto attributeAddress = attributeAddressY + byte;
		const auto attribute = BUS().VRAM().peek(attributeAddress);

		const auto ink = attribute & Mask3;
		const auto paper = (attribute >> 3) & Mask3;
		const auto bright = !!(attribute & Bit6);
		const auto flash = !!(attribute & Bit7);

		const auto background = m_palette.getColour(flash && m_flash ? ink : paper, bright);
		const auto foreground = m_palette.getColour(flash && m_flash ? paper : ink, bright);

		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = bitmap & (1 << bit);
			const auto x = (~bit & Mask3) | (byte << 3);

			m_pixels[pixelBase + x] = pixel ? foreground : background;

			tick();
		}
	}
}

void Ula::renderActiveLine(const int y) {
	renderLeftHorizontalBorder(y);
	renderVRAM(y - UpperRasterBorder);
	renderRightHorizontalBorder(y);
}

void Ula::renderLine(const int y) {

	// Start of vertical retrace
	if (y == 0)
		startFrame();

	// Vertical retrace?
	if ((y & ~Mask4) == 0) {
		tick(RasterWidth);
		return;
	}

	// Upper border
	if ((y & ~Mask6) == 0)
		renderBlankLine(y - VerticalRetraceLines);

	// Rendered from Spectrum VRAM
	else if ((y & ~Mask8) == 0)
		renderActiveLine(y - VerticalRetraceLines);

	// Lower border
	else
		renderBlankLine(y - VerticalRetraceLines);
}

void Ula::startFrame() {
	if ((++m_frameCounter & Mask4) == 0)
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

	m_mic = (value & Bit3) >> 3;
	m_speaker = (value & Bit4) >> 4;

	setBorder(value & Mask3);

	BUS().buzzer().buzz(m_speaker, BUS().frameCycles());
}

void Ula::maybeReadingPort(const uint8_t port) {
	if (ignoredPort(port))
		return;
	readingPort(port);
}

void Ula::readingPort(const uint8_t port) {
	const auto portHigh = BUS().ADDRESS().high;
	m_selected = findSelectedKeys(portHigh);
	const uint8_t value = m_selected | (m_ear << 6);
	BUS().ports().writeInputPort(port, value);
}

bool Ula::ignoredPort(const uint8_t port) const {
	return !!(port & Bit0);
}

void Ula::proceed(int cycles) {
	Proceed.fire(cycles);
}
