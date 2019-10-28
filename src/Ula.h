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
	static const int UpperRasterBorder = 48;
	static const int ActiveRasterHeight = 192;
	static const int LowerRasterBorder = 56;

	static const int HorizontalRasterBorder = 48;
	static const int ActiveRasterWidth = 256;

	static const int BytesPerLine = ActiveRasterWidth / 8;

	static const int AttributeAddress = 0x1800;

public:
	static const int VerticalRetraceLines = 16;
	static const int RasterWidth = HorizontalRasterBorder * 2 + ActiveRasterWidth;
	static const int RasterHeight = UpperRasterBorder + ActiveRasterHeight + LowerRasterBorder;
	static const int TotalHeight = VerticalRetraceLines + RasterHeight;

	static const int CyclesPerSecond = 3500000;	// 3.5Mhz
	static constexpr float FramesPerSecond = 50.08f;

	Ula(const ColourPalette& palette, Board& bus);

	EightBit::Signal<int> Proceed;

	void renderLine(int y);

	void pokeKey(SDL_Keycode raw);
	void pullKey(SDL_Keycode raw);

	void setBorder(int border) {
		m_border = border;
		m_borderColour = m_palette.getColour(m_border, false);
	}

	const auto& pixels() const { return m_pixels; }

private:
	std::array<uint16_t, 256> m_scanLineAddresses;
	std::array<uint16_t, 256> m_attributeAddresses;
	std::array<uint32_t, RasterWidth * RasterHeight> m_pixels;
	const ColourPalette& m_palette;
	Board& m_bus;
	bool m_flash = false;
	uint8_t m_frameCounter : 4;
	uint32_t m_borderColour;

	// Output port information
	int m_border; // Bits 0 - 2
	PinLevel m_mic = PinLevel::Low; // Bit 3
	PinLevel m_speaker = PinLevel::Low; // Bit 4

	// Input port information
	PinLevel m_ear = PinLevel::Low;			// Bit 6

	std::unordered_map<uint8_t, std::array<int, 5>> m_keyboardMapping;
	std::unordered_set<SDL_Keycode> m_keyboardRaw;

	int m_frameCycles = 0;

	Board& BUS() { return m_bus; }

	int frameCycles() const { return m_frameCycles; }
	int& frameCycles() { return m_frameCycles; }

	void initialiseKeyboardMapping();

	uint8_t findSelectedKeys(uint8_t rows) const;

	bool usedPort(uint8_t port) const;
	void maybeReadingPort(uint8_t port);
	void readingPort(uint8_t port);
	void maybeWrittenPort(uint8_t port);
	void writtenPort(uint8_t port);

	void startFrame();

	void flash();

	void renderBlankLine(int y);
	void renderActiveLine(int y);

	void renderLeftHorizontalBorder(int y);
	void renderRightHorizontalBorder(int y);
	void renderHorizontalBorder(int x, int y, int width = HorizontalRasterBorder);

	void renderVRAM(int y);

	void Board_ReadingPort(const uint8_t& event);
	void Board_WrittenPort(const uint8_t& event);
};
