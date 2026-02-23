#include "stdafx.h"
#include "Ula.h"

#include <cassert>

#include <Chip.h>

#include "Board.h"
#include "TZXFile.h"

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
		if ((cycles() % 2) == 0) {
			// Is the CPU able to proceed?
			if (!maybeApplyContention())
				Proceed.fire();
		}
	});
}

void Ula::maybeFlash() noexcept {
	if (F() == 0)
		flash();
}

void Ula::flash() noexcept {
	flashing() = !flashing();
}

void Ula::renderLeftRasterBorder(const int y) {
	renderRasterBorder(0, y, LeftRasterBorder);
}

void Ula::renderRightRasterBorder(const int y) {
	renderRasterBorder(LeftRasterBorder + ActiveRasterWidth, y, RightRasterBorder);
}

void Ula::renderRasterBorder(const int x, const int y, const int width) {
	// The ZX Spectrum ULA, Chris Smith
	// Chapter 12 (Generating the Display), Border Generation
	assert(x % 8 == 0);
	assert(width % 8 == 0);
	const auto chunks = width / 8;
	auto offset = static_cast<size_t>(y) * RasterWidth + x;
	for (int chunk = 0; chunk < chunks; ++chunk) {
		const auto colour = m_borderColour;
		for (int pixel = 0; pixel < 8; ++pixel)
			setClockedPixel(offset++, colour);
	}
}

void Ula::initialiseVRAMAddresses() {
	auto line = 0;
	for (auto p = 0; p < 4; ++p) {
		for (auto y = 0; y < 8; ++y) {
			for (auto o = 0; o < 8; ++o, ++line) {
				scanLineAddresses()[line] = (p << 11) + (y << 5) + (o << 8);
				attributeAddresses()[line] = AttributeAddress + (((p << 3) + y) << 5);
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
	const auto bitmapAddressY = scanLineAddresses()[addressY];
	const auto attributeAddressY = attributeAddresses()[addressY];

	// Position in pixel render 
	const auto pixelBase = LeftRasterBorder + static_cast<size_t>(y) * RasterWidth;

	for (int byte = 0; byte < BytesPerLine; ++byte) {

		const auto bitmapAddress = bitmapAddressY + byte;
		const auto bitmap = BUS().VRAM().peek(bitmapAddress);

		const auto attributeAddress = attributeAddressY + byte;
		const auto attribute = BUS().VRAM().peek(attributeAddress);

		const auto ink = attribute & Mask3;
		const auto paper = (attribute >> 3) & Mask3;
		const auto bright = !!(attribute & Bit6);
		const auto flash = !!(attribute & Bit7);

		const auto background = palette().colour(flash && flashing() ? ink : paper, bright);
		const auto foreground = palette().colour(flash && flashing() ? paper : ink, bright);

		const auto byteX = byte << 3;
		for (int bit = 0; bit < 8; ++bit) {

			const auto pixel = bitmap & Chip::bit(bit);
			const auto x = (~bit & Mask3) | byteX;

			setClockedPixel(pixelBase + x, pixel ? foreground : background);
		}
	}

	m_accessingVRAM = false;
}

void Ula::setClockedPixel(const size_t offset, const uint32_t colour) {
	setPixel(offset, colour);
	tick();
}

void Ula::setPixel(const size_t offset, const uint32_t colour) noexcept {
	m_pixels[offset] = colour;
}

bool Ula::contended(const uint16_t address) noexcept {
	// Contended area is between 0x4000 (0100000000000000)
	//						and  0x7fff (0111111111111111)
	const auto mask = Bit15 | Bit14;
	const auto masked = address & mask;
	return masked == 0b0100000000000000;
}

bool Ula::maybeContend(const uint16_t address) noexcept {
	const bool hit = m_accessingVRAM && contended(address);
	if (hit)
		addContention(3);
	return hit;
}

bool Ula::maybeContend() noexcept {
	return maybeContend(BUS().ADDRESS().word);
}

void Ula::addContention(int cycles) noexcept {
	m_contention += 2 * cycles;
}

bool Ula::maybeApplyContention() noexcept {
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
	if (y == (ActiveRasterHeight + BottomRasterBorder))
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

	if (V() < ActiveRasterHeight)
		processActiveLine();

	else if (V() < (ActiveRasterHeight + BottomRasterBorder))
		processBottomBorder();

	else if (V() < (ActiveRasterHeight + BottomRasterBorder + VerticalRetraceLines))
		processVerticalSync();

	else if (V() < (RasterHeight + VerticalRetraceLines))
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
	BUS().sound().endFrame();
}

void Ula::resetF() noexcept {
	m_frameCounter = 0;
}

void Ula::incrementF() noexcept {
	++m_frameCounter;
	maybeFlash();
}

void Ula::resetV() noexcept {
	m_verticalCounter = 0;
	incrementF();
}

void Ula::incrementV() noexcept {
	++m_verticalCounter;
	resetC();
}

void Ula::resetC() noexcept {
	m_horizontalCounter = 0;
}

void Ula::incrementC() noexcept {
	++m_horizontalCounter;
}

void Ula::pokeKey(SDL_Keycode raw) {
	keyboardRaw().emplace(raw);
}

void Ula::pullKey(SDL_Keycode raw) noexcept {
	keyboardRaw().erase(raw);
}

void Ula::initialiseKeyboardMapping() {

	// Left side
	keyboardMapping()[bit(0)] = { SDLK_LSHIFT,  SDLK_z,         SDLK_x,     SDLK_c,     SDLK_v  };
	keyboardMapping()[bit(1)] = { SDLK_a,       SDLK_s,         SDLK_d,     SDLK_f,     SDLK_g  };
	keyboardMapping()[bit(2)] = { SDLK_q,       SDLK_w,         SDLK_e,     SDLK_r,     SDLK_t  };
	keyboardMapping()[bit(3)] = { SDLK_1,       SDLK_2,         SDLK_3,     SDLK_4,     SDLK_5  };

	// Right side
	keyboardMapping()[bit(4)] = { SDLK_0,       SDLK_9,         SDLK_8,     SDLK_7,     SDLK_6  };
	keyboardMapping()[bit(5)] = { SDLK_p,       SDLK_o,         SDLK_i,     SDLK_u,     SDLK_y  };
	keyboardMapping()[bit(6)] = { SDLK_RETURN,  SDLK_l,         SDLK_k,     SDLK_j,     SDLK_h  };
	keyboardMapping()[bit(7)] = { SDLK_SPACE,   SDLK_RSHIFT,    SDLK_m,     SDLK_n,     SDLK_b  };
}

uint8_t Ula::findSelectedKeys(uint8_t rows) const {
	uint8_t returned = Mask5;
	assert(keyboardMapping().size() == 8);
	for (int row = 0; row < keyboardMapping().size(); ++row) {
		const uint8_t current = bit(row);
		if ((rows & current) == 0)
			continue;
		auto pKeys = keyboardMapping().find(current);
		if (pKeys != keyboardMapping().cend()) {
			const auto& keys = pKeys->second;
			assert(keys.size() == 5);
			for (int column = 0; column < keys.size(); ++column) {
				if (keyboardRaw().contains(keys[column]))
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

// 128 64 32 16  8  4  2  U
//   7  6  5  4  3  2  1  0
//                  <-----> Border colour (0..7) (always with Bright = off)
//               -          MIC Output (CAS SAVE) (0 = On, 1 = Off)
//            -             Beep Output (ULA Sound)    (0 = Off, 1 = On)
//   <----->                Not used

void Ula::writtenPort(const uint8_t port) {

	const auto value = BUS().ports().readOutputPort(port);

	setBorder(value & Mask3);

	PinLevel mic = PinLevel::Low;
	match(mic, value & Bit3);

	PinLevel speaker = PinLevel::Low;
	match(speaker, value & Bit4);
	BUS().sound().buzz(speaker, frameCpuCycles());
}

void Ula::maybeReadingPort(const uint8_t port) {
	if (usedPort(port))
		readingPort(port);
}

// 128 64 32 16  8  4  2  U
//   7  6  5  4  3  2  1  0
//            <-----------> Keyboard inputs (0 = Pressed, 1 = Released)
//         -                Not used
//      -                   EAR input (CAS LOAD)
//   -                      Not used

// A8..A15 Keyboard Address Output (0 = Select)

void Ula::readingPort(const uint8_t port) {
	const auto portHigh = BUS().ADDRESS().high;
	const auto  selected = findSelectedKeys(~portHigh);
	const uint8_t value = selected | (raised(m_ear) ? bit(6) : 0);
	BUS().ports().writeInputPort(port, value);
}

void Ula::attachTZX(const std::string path) {
	tape().load(path);
}
