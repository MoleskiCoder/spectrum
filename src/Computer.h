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
	void plug(const std::string& path);
	void loadSna(const std::string& path);
	void loadZ80(const std::string& path);

	Board& BUS() { return m_board; }
	const Board& BUS() const { return m_board; }

protected:
	virtual float fps() const final { return Ula::FramesPerSecond; }
	virtual bool useVsync() const final { return false; }
	virtual int displayScale() const noexcept final { return 2; }
	virtual int rasterWidth() const noexcept final { return Ula::RasterWidth; }
	virtual int rasterHeight() const noexcept final { return Ula::RasterHeight; }
	virtual std::string title() const noexcept final { return "Spectrum"; }

	virtual const uint32_t* pixels() const override;

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

	std::vector<Joystick*> joysticks();

	void handleJoyButtonDown(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event);
	void handleJoyButtonUp(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event);
	void handleControllerButtonDown(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event);
	void handleControllerButtonUp(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event);
};
