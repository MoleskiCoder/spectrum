#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <SDL.h>

class Board;
class ColourPalette;

class Ula {
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

		BitmapAddress = 0x4000,
		AttributeAddress = 0x5800
	};

	Ula(const ColourPalette& palette, Board& bus);

	void initialise();
	void renderBlank(int y);
	void render(int absoluteY);

	void pokeKey(SDL_Keycode raw);
	void pullKey(SDL_Keycode raw);

	const std::vector<uint32_t>& pixels() const;

private:
	std::vector<uint32_t> m_pixels;
	const ColourPalette& m_palette;
	Board& m_bus;
	uint32_t m_borderColour;

	std::unordered_map<int, std::array<int, 5>> m_keyboardMapping;
	std::unordered_set<SDL_Keycode> m_keyboardRaw;

	void initialiseKeyboardMapping();

	uint8_t findSelectedKeys(uint8_t row) const;

	void renderLeftHorizontalBorder(int y);
	void renderRightHorizontalBorder(int y);

	void renderActive(int absoluteY);

	void Board_ReadingPort(const EightBit::PortEventArgs& event);
	void Board_WrittenPort(const EightBit::PortEventArgs& event);
};
