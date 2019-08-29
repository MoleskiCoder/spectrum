#include "stdafx.h"
#include "ColourPalette.h"

#include <SDL.h>

void ColourPalette::load(SDL_PixelFormat* hardware) {
	loadColour(hardware, Black, 0x00, 0x00, 0x00);
	loadColour(hardware, Blue, 0x00, 0x00, 0xd7);
	loadColour(hardware, Red, 0xd7, 0x00, 0x00);
	loadColour(hardware, Magenta, 0xd7, 0x00, 0xd7);
	loadColour(hardware, Green, 0x00, 0xd7, 0x00);
	loadColour(hardware, Cyan, 0x00, 0xd7, 0xd7);
	loadColour(hardware, Yellow, 0xd7, 0xd7, 0x00);
	loadColour(hardware, White, 0xd7, 0xd7, 0xd7);
}

void ColourPalette::loadColour(SDL_PixelFormat* hardware, size_t idx, Uint8 red, Uint8 green, Uint8 blue) {
	m_colours[idx] = ::SDL_MapRGBA(
		hardware,
		red,
		green,
		blue,
		SDL_ALPHA_OPAQUE);
	m_colours[idx + 8] = ::SDL_MapRGBA(
		hardware,
		red > 0 ? red + Bright : 0,
		green > 0 ? green + Bright : 0,
		blue > 0 ? blue + Bright : 0,
		SDL_ALPHA_OPAQUE);
}
