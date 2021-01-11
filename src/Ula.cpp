#include "stdafx.h"
#include "Ula.h"

#include <Chip.h>
#include <cassert>

#include "Board.h"

Ula::Ula(const ColourPalette& palette, Board& bus)
: m_palette(palette),
  m_bus(bus) {

	initialiseKeyboardMapping();
	initialiseVRAMAddresses();

	BUS().CPU().LoweringRD.connect([this](EightBit::EventArgs) {
		maybeContend();
	});

	BUS().CPU().LoweringWR.connect([this](EightBit::EventArgs) {
		maybeContend();
	});

	BUS().CPU().ReadingIO.connect([this](EightBit::EventArgs) {
		maybeReadingPort(BUS().ADDRESS().low);
	});

	BUS().CPU().WrittenIO.connect([this](EightBit::EventArgs) {
		maybeWrittenPort(BUS().ADDRESS().low);
	});

	RaisedPOWER.connect([this](EightBit::EventArgs) {
		resetF();
		resetV();
		resetC();
		setBorder(0);
		flashing() = false;
	});

	Ticked.connect([this](EightBit::EventArgs) {
		incrementC();
		if (((cycles() % 2) == 0) && !maybeApplyContention())
			Proceed.fire();
	});
}

void Ula::maybeFlash() {
	if (F() == 0)
		flash();
}

void Ula::flash() {
	flashing() = !flashing();
}

void Ula::renderLeftRasterBorder(const int y) {
	renderRasterBorder(0, y, LeftRasterBorder);
}

void Ula::renderRightRasterBorder(const int y) {
	renderRasterBorder(LeftRasterBorder + ActiveRasterWidth, y, RightRasterBorder);
}

void Ula::renderRasterBorder(int x, int y, int width) {
	const size_t begin = y * RasterWidth + x;
	for (int i = 0; i < width; ++i)
		setClockedPixel(begin + i, m_borderColour);
}

void Ula::initialiseVRAMAddresses() {
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

void Ula::renderVRAM(const int y) {

	assert(y >= 0);
	assert(y < RasterHeight);

	m_accessingVRAM = true;

	// Position in VRAM
	const auto addressY = y - TopRasterBorder;
	assert(addressY < ActiveRasterHeight);
	const auto bitmapAddressY = m_scanLineAddresses[addressY];
	const auto attributeAddressY = m_attributeAddresses[addressY];

	// Position in pixel render 
	const auto pixelBase = LeftRasterBorder + y * RasterWidth;

	for (int byte = 0; byte < BytesPerLine; ++byte) {

		const auto bitmapAddress = bitmapAddressY + byte;
		const auto bitmap = BUS().VRAM().peek(bitmapAddress);

		const auto attributeAddress = attributeAddressY + byte;
		const auto attribute = BUS().VRAM().peek(attributeAddress);

		const auto ink = attribute & Mask3;
		const auto paper = (attribute >> 3) & Mask3;
		const auto bright = !!(attribute & Bit6);
		const auto flash = !!(attribute & Bit7);

		const auto background = m_palette.colour(flash && flashing() ? ink : paper, bright);
		const auto foreground = m_palette.colour(flash && flashing() ? paper : ink, bright);

		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = bitmap & Chip::bit(bit);
			const auto x = (~bit & Mask3) | (byte << 3);

			setClockedPixel(pixelBase + x, pixel ? foreground : background);
		}
	}

	m_accessingVRAM = false;
}

void Ula::setClockedPixel(const size_t offset, const uint32_t colour) {
	setPixel(offset, colour);
	tick();
}

void Ula::setPixel(const size_t offset, const uint32_t colour) {
	m_pixels[offset] = colour;
}

bool Ula::contended(const uint16_t address) {
	// Contended area is between 0x4000 (0100000000000000)
	//						and  0x7fff (0111111111111111)
	const auto mask = Bit15 | Bit14;
	const auto masked = address & mask;
	return masked == 0b0100000000000000;
}

bool Ula::maybeContend(const uint16_t address) {
	const bool hit = m_accessingVRAM && contended(address);
	if (hit)
		addContention(3);
	return hit;
}

bool Ula::maybeContend() {
	return maybeContend(BUS().ADDRESS().word);
}

void Ula::addContention(int cycles) {
	m_contention += 2 * cycles;
}

bool Ula::maybeApplyContention() {
	const auto apply = contention() > 0;
	if (apply)
		--m_contention;
	return apply;
}

void Ula::processActiveLine() {
	processActiveLine(V() + TopRasterBorder);
}

void Ula::processActiveLine(const int y) {
	renderVRAM(y);
	renderRightRasterBorder(y);
	tick(HorizontalRetraceClocks);
	renderLeftRasterBorder(y);
}

void Ula::processBottomBorder() {
	processBorder(V() + TopRasterBorder);
}

void Ula::processVerticalSync() {
	processVerticalSync(V());
}

void Ula::processVerticalSync(const int y) {
	if (y == 248)
		lower(BUS().CPU().INT());
	tick(ActiveRasterWidth);
	tick(RightRasterBorder);
	tick(HorizontalRetraceClocks);
	tick(LeftRasterBorder);
}

void Ula::processTopBorder() {
	processBorder(V() - VerticalRetraceLines - TopRasterBorder - ActiveRasterHeight);
}

void Ula::processBorder(int y) {
	renderRasterBorder(LeftRasterBorder, y, ActiveRasterWidth);
	renderRightRasterBorder(y);
	tick(HorizontalRetraceClocks);
	renderLeftRasterBorder(y);
}

void Ula::renderLine() {

	assert(C() == 0);

	if (V() < 192)
		processActiveLine();

	else if (V() < 248)
		processBottomBorder();

	else if (V() < 256)
		processVerticalSync();

	else if (V() < 312)
		processTopBorder();

	assert(C() == TotalHorizontalClocks);
	incrementV();
}

void Ula::renderLines() {
	assert(V() == 0);
	for (int i = 0; i < TotalHeight; ++i)
		renderLine();
	assert(V() == TotalHeight);
	resetV();
}

void Ula::resetF() {
	m_frameCounter = 0;
}

void Ula::incrementF() {
	++m_frameCounter;
	maybeFlash();
}

void Ula::resetV() {
	BUS().sound().endFrame();
	m_verticalCounter = 0;
	incrementF();
}

void Ula::incrementV() {
	++m_verticalCounter;
	resetC();
}

void Ula::resetC() {
	m_horizontalCounter = 0;
}

void Ula::incrementC() {
	++m_horizontalCounter;
}

void Ula::pokeKey(SDL_Keycode raw) {
	m_keyboardRaw.emplace(raw);
}

void Ula::pullKey(SDL_Keycode raw) {
	m_keyboardRaw.erase(raw);
}

void Ula::initialiseKeyboardMapping() {

	// Left side
	m_keyboardMapping[bit(0)] = { SDLK_LSHIFT,	SDLK_z,			SDLK_x,		SDLK_c,		SDLK_v	};
	m_keyboardMapping[bit(1)] = { SDLK_a,		SDLK_s,			SDLK_d,		SDLK_f,		SDLK_g	};
	m_keyboardMapping[bit(2)] = { SDLK_q,		SDLK_w,			SDLK_e,		SDLK_r,		SDLK_t	};
	m_keyboardMapping[bit(3)] = { SDLK_1,		SDLK_2,			SDLK_3,     SDLK_4,		SDLK_5	};

	// Right side
	m_keyboardMapping[bit(4)] = { SDLK_0,		SDLK_9,			SDLK_8,		SDLK_7,		SDLK_6	};
	m_keyboardMapping[bit(5)] = { SDLK_p,		SDLK_o,			SDLK_i,		SDLK_u,		SDLK_y	};
	m_keyboardMapping[bit(6)] = { SDLK_RETURN,	SDLK_l,			SDLK_k,		SDLK_j,		SDLK_h	};
	m_keyboardMapping[bit(7)] = { SDLK_SPACE,	SDLK_RSHIFT,	SDLK_m,		SDLK_n,		SDLK_b	};
}

uint8_t Ula::findSelectedKeys(uint8_t rows) const {
	uint8_t returned = 0xff;
	for (int row = 0; row < 8; ++row) {
		const uint8_t current = bit(row);
		if ((rows & current) == 0)
			continue;
		auto pKeys = m_keyboardMapping.find(current);
		if (pKeys != m_keyboardMapping.cend()) {
			const auto& keys = pKeys->second;
			for (int column = 0; column < 5; ++column) {
				if (m_keyboardRaw.find(keys[column]) != m_keyboardRaw.cend())
					returned &= ~bit(column);
			}
		}
	}
	return returned;
}

void Ula::maybeWrittenPort(const uint8_t port) {
	if (usedPort(port))
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

	setBorder(value & Mask3);

	PinLevel mic = PinLevel::Low;
	match(mic, value & Bit3);

	PinLevel speaker = PinLevel::Low;
	match(speaker, value & Bit4);
	BUS().sound().buzz(speaker, frameCycles());
}

void Ula::maybeReadingPort(const uint8_t port) {
	if (usedPort(port))
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
	const uint8_t value = selected | (raised(m_ear) ? bit(6) : 0);
	BUS().ports().writeInputPort(port, value);
}

bool Ula::usedPort(const uint8_t port) const {
	return (port & Bit0) == 0;
}
