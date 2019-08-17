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
	uint32_t m_borderColour = 0;
	bool m_flash = false;
	uint8_t m_frameCounter = 0;

	std::unordered_map<int, std::array<int, 5>> m_keyboardMapping;
	std::unordered_set<SDL_Keycode> m_keyboardRaw;

	Board& BUS() { return m_bus; }

	void initialiseKeyboardMapping();

	uint8_t findSelectedKeys(uint8_t row) const;

	void flash();

	void renderLeftHorizontalBorder(int y);
	void renderRightHorizontalBorder(int y);

	void renderActive(int absoluteY);

	void Board_ReadingPort(const uint8_t& event);
	void Board_WrittenPort(const uint8_t& event);
};
