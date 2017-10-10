#pragma once

#include <stdexcept>
#include <string>
#include <array>

#include <SDL.h>

#include "Board.h"
#include "Ula.h"
#include "ColourPalette.h"

class Configuration;

class Computer {
public:

	static void throwSDLException(std::string failure) {
		throw std::runtime_error(failure + ::SDL_GetError());
	}

	static void verifySDLCall(int returned, std::string failure) {
		if (returned < 0) {
			throwSDLException(failure);
		}
	}

	Computer(const Configuration& configuration);

	void runLoop();
	void initialise();

private:
	enum {
		DisplayScale = 2,
		ScreenWidth = Ula::RasterWidth * DisplayScale,
		ScreenHeight = Ula::RasterHeight * DisplayScale,
	};

	const Configuration& m_configuration;
	ColourPalette m_colours;
	mutable Board m_board;

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	SDL_Texture* m_bitmapTexture;
	Uint32 m_pixelType;
	SDL_PixelFormat* m_pixelFormat;

	int m_fps;
	Uint32 m_startTicks;
	Uint32 m_frames;
	bool m_vsync;

	void drawFrame();

	void configureBackground() const;
	void createBitmapTexture();

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
