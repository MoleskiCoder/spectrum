#pragma once

#include <cstdint>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include <SDL.h>

class Board;
class ColourPalette;

// http://www.worldofspectrum.org/faq/reference/48kreference.htm

class Ula final {
public:
	enum {
		VerticalRetraceLines = 16,
		UpperRasterBorder = 48,
		ActiveRasterHeight = 192,
		LowerRasterBorder = 56,
		RasterHeight = UpperRasterBorder + ActiveRasterHeight + LowerRasterBorder,
		TotalHeight = VerticalRetraceLines + RasterHeight,

		LeftRasterBorder = 48,
		ActiveRasterWidth = 256,
		RightRasterBorder = 48,
		RasterWidth = LeftRasterBorder + ActiveRasterWidth + RightRasterBorder,

		BytesPerLine = ActiveRasterWidth / 8,

		HorizontalBorderCycles = 24,
		HorizontalFlybackCycles = 48,
		HorizontalActiveCycles = 128,
		CyclesPerLine = HorizontalBorderCycles * 2 + HorizontalFlybackCycles + HorizontalActiveCycles,

		CyclesPerFrame = TotalHeight * CyclesPerLine,
		CyclesPerSecond = 3500000,	// 3.5Mhz

		AttributeAddress = 0x1800
	};

	static constexpr float FramesPerSecond = 50.08;

	Ula(const ColourPalette& palette, Board& bus);

	void initialise();

	void renderLine(int y);

	void pokeKey(SDL_Keycode raw);
	void pullKey(SDL_Keycode raw);

	const auto& pixels() const { return m_pixels; }

private:
	std::array<uint16_t, 256> m_scanLineAddresses;
	std::array<uint16_t, 256> m_attributeAddresses;
	std::array<uint32_t, RasterWidth * RasterHeight> m_pixels;
	const ColourPalette& m_palette;
	Board& m_bus;
	bool m_flash = false;
	uint8_t m_frameCounter = 0;
	uint32_t m_borderColour;

	// Output port information
	uint8_t m_border : 3;		// Bits 0 - 2
	uint8_t m_mic : 1;			// Bit 3
	uint8_t m_speaker : 1;		// Bit 4

	// Input port information
	uint8_t m_selected : 5;		// Bits 0 - 4 
	uint8_t m_ear : 1;			// Bit 6

	std::unordered_map<int, std::array<int, 5>> m_keyboardMapping;
	std::unordered_set<SDL_Keycode> m_keyboardRaw;

	Board& BUS() { return m_bus; }

	void initialiseKeyboardMapping();

	uint8_t findSelectedKeys(uint8_t row) const;

	bool ignoredPort(uint8_t port) const;
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

	void renderVRAM(int y);

	void Board_ReadingPort(const uint8_t& event);
	void Board_WrittenPort(const uint8_t& event);
};
