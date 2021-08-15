#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <Game.h>

#include "Board.h"
#include "ColourPalette.h"
#include "Joystick.h"

class Configuration;
class Expansion;

class Computer final : public Gaming::Game {
public:
	Computer(const Configuration& configuration);

	void raisePOWER() override;
	void lowerPOWER() override;

	void plug(std::shared_ptr<Expansion> expansion);
	void plug(std::string path);
	void loadSna(std::string path);
	void loadZ80(std::string path);
	void loadTZX(std::string path);

	[[nodiscard]] Board& BUS() { return m_board; }
	[[nodiscard]] const Board& BUS() const { return m_board; }

protected:
	[[nodiscard]] float fps() const noexcept final { return Ula::FramesPerSecond; }
	[[nodiscard]] bool useVsync() const noexcept final { return true; }
	[[nodiscard]] int displayScale() const noexcept final { return 2; }
	[[nodiscard]] int rasterWidth() const noexcept final { return Ula::RasterWidth; }
	[[nodiscard]] int rasterHeight() const noexcept final { return Ula::RasterHeight; }
	[[nodiscard]] std::string title() const noexcept final { return "Spectrum"; }

	[[nodiscard]] const uint32_t* pixels() const final;

	void runRasterLines() final;

	bool handleKeyDown(SDL_Keycode key) final;
	bool handleKeyUp(SDL_Keycode key) final;

	bool handleJoyButtonDown(SDL_JoyButtonEvent event) final;
	bool handleJoyButtonUp(SDL_JoyButtonEvent event) final;

	bool handleControllerButtonDown(SDL_ControllerButtonEvent event) final;
	bool handleControllerButtonUp(SDL_ControllerButtonEvent event) final;

private:
	const Configuration& m_configuration;
	ColourPalette m_colours;
	Board m_board;

	[[nodiscard]] std::vector<Joystick*> joysticks();

	void handleJoyButtonDown(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event);
	void handleJoyButtonUp(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event);
	void handleControllerButtonDown(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event);
	void handleControllerButtonUp(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event);
};
