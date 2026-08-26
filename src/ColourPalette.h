#pragma once

#include "AbstractColourPalette.h"

#include <cstdint>

struct SDL_PixelFormatDetails;

class ColourPalette final : public AbstractColourPalette<uint32_t> {
public:
	ColourPalette() = default;

	void load(const SDL_PixelFormatDetails* hardware);

private:
	void loadColour(const SDL_PixelFormatDetails* hardware, size_t idx, Uint8 red, Uint8 green, Uint8 blue);
};
