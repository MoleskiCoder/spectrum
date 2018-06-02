#pragma once

#include <array>
#include <cstdint>

struct SDL_PixelFormat;

class ColourPalette final {
public:
	enum {
		Bright = 0x28,
		Black = 0,
		Blue,
		Red,
		Magenta,
		Green,
		Cyan,
		Yellow,
		White
	};

	ColourPalette() = default;

	uint32_t getColour(size_t index, bool bright) const {
		assert(index < 8);
		return getColour(bright ? index + 8 : index);
	}

	uint32_t getColour(size_t index) const {
		assert(index < 16);
		return m_colours[index];
	}

	void load(SDL_PixelFormat* hardware);

private:
	std::array<uint32_t, 16> m_colours;

	void loadColour(SDL_PixelFormat* hardware, size_t idx, Uint8 red, Uint8 green, Uint8 blue);
};
