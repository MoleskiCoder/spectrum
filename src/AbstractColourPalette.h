#pragma once

#include <array>
#include <cassert>
#include <cstdint>

template <class ColourT>
class AbstractColourPalette {
protected:
	std::array<ColourT, 16> m_colours = {};

	AbstractColourPalette() = default;

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


	[[nodiscard]] constexpr auto colour(size_t index) const noexcept {
		assert(index < 16);
		return m_colours[index];
	}

	[[nodiscard]] constexpr auto colour(size_t index, bool bright) const noexcept {
		assert(index < 8);
		return colour(bright ? index + 8 : index);
	}
};
