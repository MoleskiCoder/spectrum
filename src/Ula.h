#pragma once

#include <cstdint>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include <SDL.h>

class Board;
class ColourPalette;

class Ula final {
public:
	enum {
		UpperRasterBorder = 64,
		ActiveRasterHeight = 192,
		LowerRasterBorder = 56,

		LeftRasterBorder = 64,
		ActiveRasterWidth = 256,
		RightRasterBorder = 64,

		BytesPerLine = ActiveRasterWidth / 8,

		RasterWidth = LeftRasterBorder + ActiveRasterWidth + RightRasterBorder,
		RasterHeight = UpperRasterBorder + ActiveRasterHeight + LowerRasterBorder,

		CyclesPerSecond = 3500000,	// 3.5Mhz
		FramesPerSecond = 50,
		CyclesPerFrame = CyclesPerSecond / FramesPerSecond,

		HorizontalDrawCycles = 128,
		HorizontalBlankCycles = 96,
		HorizontalCyclesTotal = HorizontalDrawCycles + HorizontalBlankCycles,

		AttributeAddress = 0x1800
	};

	Ula(const ColourPalette& palette, Board& bus);

	void initialise();

	void finishFrame();

	void renderBlank(int y);
	void render(int absoluteY);

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

	// Output port information
	uint32_t m_borderColour = 0;	// Bits 0 - 2, converted from palette
	bool m_mic;						// Bit 3
	bool m_speaker;					// Bit 4

	// Input port information
	uint8_t m_selected;				// Bits 0 - 4 
	bool m_ear = false;				// Bit 6

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

	void flash();

	void renderLeftHorizontalBorder(int y);
	void renderRightHorizontalBorder(int y);

	void renderActive(int absoluteY);

	void Board_ReadingPort(const uint8_t& event);
	void Board_WrittenPort(const uint8_t& event);
};
