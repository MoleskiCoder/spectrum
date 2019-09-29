#include "stdafx.h"
#include "Computer.h"
#include "Configuration.h"
#include "Joystick.h"

Computer::Computer(const Configuration& configuration)
: m_configuration(configuration),
  m_board(m_colours, configuration) {
}

const uint32_t* Computer::pixels() const {
	return BUS().ULA().pixels().data();
}

void Computer::raisePOWER() {
	Game::raisePOWER();
	m_colours.load(pixelFormat().get());
	BUS().initialise();
	BUS().raisePOWER();
}

void Computer::lowerPOWER() {
	BUS().lowerPOWER();
	Game::lowerPOWER();
}

void Computer::plug(std::shared_ptr<Expansion> expansion) {
	BUS().plug(expansion);
}

void Computer::plug(const std::string& path) {
	BUS().plug(path);
}

void Computer::loadSna(const std::string& path) {
	BUS().loadSna(path);
}

void Computer::loadZ80(const std::string& path) {
	BUS().loadZ80(path);
}

void Computer::runVerticalBlank() {
	BUS().runVerticalBlank();
}

void Computer::runRasterLines() {
	BUS().runRasterLines();
}

void Computer::handleKeyDown(SDL_Keycode key) {
	BUS().ULA().pokeKey(key);
}

void Computer::handleKeyUp(SDL_Keycode key) {
	BUS().ULA().pullKey(key);
}

void Computer::handleJoyButtonDown(const SDL_JoyButtonEvent event) {
	const auto joystick = (Joystick*)BUS().expansion(0).get();
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_A:
		joystick->pushFire();
		break;
	}
}

void Computer::handleJoyButtonUp(const SDL_JoyButtonEvent event) {
	const auto joystick = (Joystick*)BUS().expansion(0).get();
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_A:
		joystick->releaseFire();
		break;
	}
}

void Computer::handleControllerButtonDown(SDL_ControllerButtonEvent event) {
	const auto joystick = (Joystick*)BUS().expansion(0).get();
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		joystick->pushUp();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		joystick->pushDown();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		joystick->pushLeft();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		joystick->pushRight();
		break;
	}
}

void Computer::handleControllerButtonUp(SDL_ControllerButtonEvent event) {
	const auto joystick = (Joystick*)BUS().expansion(0).get();
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		joystick->releaseUp();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		joystick->releaseDown();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		joystick->releaseLeft();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		joystick->releaseRight();
		break;
	}
}
