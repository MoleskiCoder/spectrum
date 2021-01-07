#pragma once

#include <cstdint>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include <SDL.h>

#include <ClockedChip.h>
#include <Signal.h>

#include "ColourPalette.h"

class Board;

/*
Not sure if it's the right approach, but I've modified the ULA class to be clocked
per pixel at half the ZX Spectrum crystal rate (14MHz).  So, one pixel equals
two Z80 T-states at 7MHz. The CPU is clocked at half the ULA clock, i.e. 3.5MHz.

I've left a couple of calculations in place in the private portion of the ULA class,
just so I can see any discrepencies, if/when they arise.  So far seems to be working
out.  It's interesting to see there is a slight mismatch in the ULA speed vs PAL
standard, which apparently accounts for ZX Spectrum 16/48K pixel jitter in real life!

						+--------------------------------------------+
						+............................................+		^		^
						+............................................+		|		|
						+..............vertical blank................+	   8px		|
						+............................................+		|		|
						+............................................+		V		|
						+--------------------------------------------+				|
						|                                            |		^		|
						|                                            |		|		|
						|             top vertical border            |	   56px		|
  horizontal			|                                            |		|		|
    sync				|                                            |		V		|
<---28px--->			|          +----------------------+          |				|
						|          |                      |          |		^		|
						|          |      display         |          |		|		|
						|          |        area          |          |		|		|
     horizontal blank	|          |                      |          |		|		|
<---------96px--------->|<--48px-->|<-------256px-------->|<--48px-->|	  192px	  312px
						|          |                      |          |		|		|
						|          |                      |          |		|		|
						|          |                      |          |		|		|
						|          |                      |          |		V		|
						|          +----------------------+          |				|
						|                                            |		^		|
						|                                            |		|		|
						|           bottom vertical border           |	   56px		|
						|                                            |		|		|
						|                                            |		V		V
						+--------------------------------------------+
*/

// http://www.worldofspectrum.org/faq/reference/48kreference.htm

class Ula final : public EightBit::ClockedChip {
private:
	static const int HorizontalRasterBorder = 48;
	static const int VerticalRasterBorder = 56;

	static const int ActiveRasterWidth = 256;
	static const int ActiveRasterHeight = 192;

	static const int HorizontalSyncClocks = 28;
	static const int HorizontalRetraceClocks = 96;
	static const int HorizontalRetraceRemainingClocks = HorizontalRetraceClocks - HorizontalSyncClocks;
	static const int VerticalRetraceLines = 8;

	static const int BytesPerLine = ActiveRasterWidth / 8;
	static const int AttributeAddress = 0x1800;

public:
	static constexpr float FramesPerSecond = 50.08f;
	static const int ClockRate = 7000000; // 7Mhz

	static const int RasterWidth = HorizontalRasterBorder * 2 + ActiveRasterWidth;
	static const int RasterHeight = VerticalRasterBorder * 2 + ActiveRasterHeight;

public:
	Ula(const ColourPalette& palette, Board& bus);

	EightBit::Signal<EightBit::EventArgs> Proceed;

	void renderLines();

	void pokeKey(SDL_Keycode raw);
	void pullKey(SDL_Keycode raw);

	void setBorder(int border) {
		m_borderColour = m_palette.colour(border, false);
	}

	[[nodiscard]] const auto& pixels() const { return m_pixels; }

private:
	static const int TotalHeight = VerticalRetraceLines + RasterHeight;
	static const int TotalHorizontalClocks = HorizontalRetraceClocks + RasterWidth;
	static const int TotalFrameClocks = TotalHeight * TotalHorizontalClocks;
	static constexpr float CalculatedClockFrequency = TotalFrameClocks * FramesPerSecond;

	std::array<uint16_t, 256> m_scanLineAddresses;
	std::array<uint16_t, 256> m_attributeAddresses;
	std::array<uint32_t, RasterWidth * RasterHeight> m_pixels;
	const ColourPalette& m_palette;
	Board& m_bus;
	bool m_flashing = false;
	unsigned m_frameCounter : 4;
	unsigned m_verticalCounter : 9;
	unsigned m_horizontalCounter : 9;
	uint32_t m_borderColour = 0;
	int m_contention = 0;
	bool m_accessingVRAM = false;

	// Input port information
	PinLevel m_ear = PinLevel::Low;			// Bit 6

	std::unordered_map<uint8_t, std::array<int, 5>> m_keyboardMapping;
	std::unordered_set<SDL_Keycode> m_keyboardRaw;

	int m_frameCycles = 0;	// Needed to generate sound timing

	[[nodiscard]] Board& BUS() { return m_bus; }

	void renderLine();

	// Frame counter, four bits
	[[nodiscard]] auto F() const { return m_frameCounter; }

	void resetF();
	void incrementF();

	// Vertical line counter, nine bits
	[[nodiscard]] auto V() const { return m_verticalCounter; }

	void resetV();
	void incrementV();

	// Horizontal pixel counter, nine bits
	[[nodiscard]] auto C() const { return m_horizontalCounter; }

	void resetC();
	void incrementC();

	[[nodiscard]] auto frameCycles() const {
		return TotalHorizontalClocks * V() + C();
	}

	void initialiseKeyboardMapping();

	[[nodiscard]] uint8_t findSelectedKeys(uint8_t rows) const;

	[[nodiscard]] bool usedPort(uint8_t port) const;
	void maybeReadingPort(uint8_t port);
	void readingPort(uint8_t port);
	void maybeWrittenPort(uint8_t port);
	void writtenPort(uint8_t port);

	void maybeFlash();
	void flash();
	[[nodiscard]] auto& flashing() { return m_flashing; }

	[[nodiscard]] auto withinVerticalRetrace() const { return (V() & ~Mask3) == 0; }
	[[nodiscard]] auto withinUpperBorder() const { return (V() & ~Mask6) == 0; }
	[[nodiscard]] auto withinActiveArea() const { return (V() & ~Mask8) == 0; }

	void renderBlankLine(int y);
	void renderActiveLine(int y);

	void renderLeftHorizontalBorder(int y);
	void renderRightHorizontalBorder(int y);
	void renderHorizontalBorder(int x, int y, int width = HorizontalRasterBorder);

	void renderVRAM(int y);

	// Paint a pixel and tick the ULA clock
	void setClockedPixel(size_t offset, uint32_t colour);

	// Paint a pixel
	void setPixel(size_t offset, uint32_t colour);

	// Address contention?

	[[nodiscard]] static bool contended(uint16_t address);
	bool maybeContend(uint16_t address);
	bool maybeContend();
	void addContention(int cycles);
	[[nodiscard]] auto contention() const { return m_contention; }
	bool maybeApplyContention();
};
