#pragma once

#include <stdexcept>
#include <string>
#include <array>

#include <SDL.h>

#include "Board.h"
#include "Ula.h"
#include "ColourPalette.h"

class Configuration;

class Computer final {
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

	void plug(const std::string& path);
	void loadSna(const std::string& path);
	void loadZ80(const std::string& path);

private:
	enum {
		DisplayScale = 2,
		ScreenWidth = Ula::RasterWidth * DisplayScale,
		ScreenHeight = Ula::RasterHeight * DisplayScale,
	};

	const Configuration& m_configuration;
	ColourPalette m_colours;
	Board m_board;

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	SDL_Texture* m_bitmapTexture = nullptr;
	Uint32 m_pixelType = SDL_PIXELFORMAT_ARGB8888;
	SDL_PixelFormat* m_pixelFormat = nullptr;

	Uint32 m_startTicks = 0UL;
	Uint32 m_frames = 0UL;

	void drawFrame();

	void configureBackground() const;
	void createBitmapTexture();

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
