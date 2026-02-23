#pragma once

#include <array>
#include <cassert>
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

	[[nodiscard]] constexpr auto colour(size_t index) const noexcept {
		assert(index < 16);
		return m_colours[index];
	}

	[[nodiscard]] constexpr auto colour(size_t index, bool bright) const noexcept {
		assert(index < 8);
		return colour(bright ? index + 8 : index);
	}

	void load(SDL_PixelFormat* hardware);

private:
	std::array<uint32_t, 16> m_colours = {};

	void loadColour(SDL_PixelFormat* hardware, size_t idx, Uint8 red, Uint8 green, Uint8 blue);
};
