#include "stdafx.h"
#include "ColourPalette.h"

#include <SDL.h>

void ColourPalette::load(SDL_PixelFormat* hardware) {
	m_colours[Black] = ::SDL_MapRGBA(hardware, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	m_colours[Blue] = ::SDL_MapRGBA(hardware, 0x00, 0x00, 0xd7, SDL_ALPHA_OPAQUE);
	m_colours[Red] = ::SDL_MapRGBA(hardware, 0xd7, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	m_colours[Magenta] = ::SDL_MapRGBA(hardware, 0xd7, 0x00, 0xd7, SDL_ALPHA_OPAQUE);
	m_colours[Green] = ::SDL_MapRGBA(hardware, 0x00, 0xd7, 0x00, SDL_ALPHA_OPAQUE);
	m_colours[Cyan] = ::SDL_MapRGBA(hardware, 0x00, 0xd7, 0xd7, SDL_ALPHA_OPAQUE);
	m_colours[Yellow] = ::SDL_MapRGBA(hardware, 0xd7, 0xd7, 0x00, SDL_ALPHA_OPAQUE);
	m_colours[White] = ::SDL_MapRGBA(hardware, 0xd7, 0xd7, 0xd7, SDL_ALPHA_OPAQUE);
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
