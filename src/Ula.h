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

// http://www.worldofspectrum.org/faq/reference/48kreference.htm

class Ula final : public EightBit::ClockedChip {
private:
	static const int HorizontalRasterBorder = 48;
	static const int UpperRasterBorder = 48;
	static const int LowerRasterBorder = 56;

	static const int ActiveRasterWidth = 256;
	static const int ActiveRasterHeight = 192;

	static const int VerticalRetraceLines = 16;

	static const int BytesPerLine = ActiveRasterWidth / 8;
	static const int AttributeAddress = 0x1800;

public:
	static constexpr float FramesPerSecond = 50.08f;
	static const int CyclesPerSecond = 3500000;	// 3.5Mhz

	static const int RasterWidth = HorizontalRasterBorder * 2 + ActiveRasterWidth;
	static const int RasterHeight = UpperRasterBorder + ActiveRasterHeight + LowerRasterBorder;

public:
	Ula(const ColourPalette& palette, Board& bus);

	EightBit::Signal<int> Proceed;

	void renderLines();

	void pokeKey(SDL_Keycode raw);
	void pullKey(SDL_Keycode raw);

	void setBorder(int border) {
		m_borderColour = m_palette.getColour(border, false);
	}

	[[nodiscard]] const auto& pixels() const { return m_pixels; }

private:
	static const int TotalHeight = VerticalRetraceLines + RasterHeight;

	std::array<uint16_t, 256> m_scanLineAddresses;
	std::array<uint16_t, 256> m_attributeAddresses;
	std::array<uint32_t, RasterWidth * RasterHeight> m_pixels;
	const ColourPalette& m_palette;
	Board& m_bus;
	bool m_flash = false;
	unsigned m_frameCounter : 4;
	unsigned m_verticalCounter : 9;
	unsigned m_horizontalCounter : 9;
	uint32_t m_borderColour;

	// Input port information
	PinLevel m_ear = PinLevel::Low;			// Bit 6

	std::unordered_map<uint8_t, std::array<int, 5>> m_keyboardMapping;
	std::unordered_set<SDL_Keycode> m_keyboardRaw;

	int m_frameCycles = 0;	// Needed to generate sound timing

	[[nodiscard]] Board& BUS() { return m_bus; }

	void renderLine();

	// Frame counter, four bits
	[[nodiscard]] auto F() const { return m_frameCounter; }
	void resetF() { m_frameCounter = 0; }
	void incrementF() { ++m_frameCounter; }

	// Vertical line counter, nine bits
	[[nodiscard]] auto V() const { return m_verticalCounter; }
	void resetV() { m_verticalCounter = 0; }
	void incrementV() { ++m_verticalCounter; }

	// Horizontal pixel counter, nine bits
	[[nodiscard]] auto C() const { return m_horizontalCounter; }
	void resetC() { m_horizontalCounter = 0; }
	void incrementC() { ++m_horizontalCounter; }

	//		   ___________________________
	//		   __   __   __   __   __
	// VSync = V7 + V6 + V5 + V4 + V3 + V2
	[[nodiscard]] auto VSync() const { return (V() & 0b0011111100) == 0b0011111000; }

	[[nodiscard]] int frameCycles() const { return m_frameCycles; }
	[[nodiscard]] int& frameCycles() { return m_frameCycles; }

	void initialiseKeyboardMapping();

	[[nodiscard]] uint8_t findSelectedKeys(uint8_t rows) const;

	[[nodiscard]] bool usedPort(uint8_t port) const;
	void maybeReadingPort(uint8_t port);
	void readingPort(uint8_t port);
	void maybeWrittenPort(uint8_t port);
	void writtenPort(uint8_t port);

	void startFrame();

	void maybeFlash();
	void flash();

	void renderBlankLine(int y);
	void renderActiveLine(int y);

	void renderLeftHorizontalBorder(int y);
	void renderRightHorizontalBorder(int y);
	void renderHorizontalBorder(int x, int y, int width = HorizontalRasterBorder);

	void renderVRAM(int y);
};
