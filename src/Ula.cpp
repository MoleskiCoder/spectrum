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
		 auto available = cycles() / 2;
		if (available > 0) {
			Proceed.fire(available);
			frameCycles() += available;
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
	BUS().buzzer().endFrame();
	frameCycles() = 0;
	if (++m_frameCounter == 0)
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
	m_keyboardMapping[1 << 0] = { SDLK_LSHIFT,	SDLK_z,			SDLK_x,		SDLK_c,		SDLK_v	};
	m_keyboardMapping[1 << 1] = { SDLK_a,		SDLK_s,			SDLK_d,		SDLK_f,		SDLK_g	};
	m_keyboardMapping[1 << 2] = { SDLK_q,		SDLK_w,			SDLK_e,		SDLK_r,		SDLK_t	};
	m_keyboardMapping[1 << 3] = { SDLK_1,		SDLK_2,			SDLK_3,     SDLK_4,		SDLK_5	};

	// Right side
	m_keyboardMapping[1 << 4] = { SDLK_0,		SDLK_9,			SDLK_8,		SDLK_7,		SDLK_6	};
	m_keyboardMapping[1 << 5] = { SDLK_p,		SDLK_o,			SDLK_i,		SDLK_u,		SDLK_y	};
	m_keyboardMapping[1 << 6] = { SDLK_RETURN,	SDLK_l,			SDLK_k,		SDLK_j,		SDLK_h	};
	m_keyboardMapping[1 << 7] = { SDLK_SPACE,	SDLK_RSHIFT,	SDLK_m,		SDLK_n,		SDLK_b	};
}

uint8_t Ula::findSelectedKeys(uint8_t rows) const {
	uint8_t returned = 0xff;
	for (int row = 0; row < 8; ++row) {
		const uint8_t current = 1 << row;
		if ((rows & current) == 0)
			continue;
		auto pKeys = m_keyboardMapping.find(current);
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
// 0 - 2	Border Color(0..7) (always with Bright = off)
// 3		MIC Output(CAS SAVE) (0 = On, 1 = Off)
// 4		Beep Output(ULA Sound)    (0 = Off, 1 = On)
// 5 - 7	Not used

// 128 64 32 16  8  4  2  U
//   7  6  5  4  3  2  1  0
//                  <----->	Border colour
//               -		    Mic output
//            -				Beep output
//   <----->				Not used

void Ula::writtenPort(const uint8_t port) {

	const auto value = BUS().ports().readOutputPort(port);

	m_mic = (value & Bit3) >> 3;
	m_speaker = (value & Bit4) >> 4;

	setBorder(value & Mask3);

	BUS().buzzer().buzz(m_speaker, frameCycles());
}

void Ula::maybeReadingPort(const uint8_t port) {
	if (ignoredPort(port))
		return;
	readingPort(port);
}

// 0 - 4	Keyboard Inputs(0 = Pressed, 1 = Released)
// 5		Not used
// 6		EAR Input(CAS LOAD)
// 7		Not used
// A8..A15	Keyboard Address Output(0 = Select)

// 128 64 32 16  8  4  2  U
//   7  6  5  4  3  2  1  0
//            <----------->	Keyboard
//         -				Not used
//      -					Ear input
//   -						Not used

void Ula::readingPort(const uint8_t port) {
	const auto portHigh = BUS().ADDRESS().high;
	const auto  selected = findSelectedKeys(~portHigh);
	const uint8_t value = selected | (m_ear << 6);
	BUS().ports().writeInputPort(port, value);
}

bool Ula::ignoredPort(const uint8_t port) const {
	return !!(port & Bit0);
}
