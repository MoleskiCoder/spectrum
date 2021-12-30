#include "stdafx.h"
#include "Computer.h"
#include "Configuration.h"
#include "Joystick.h"

Computer::Computer(Configuration& configuration)
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

void Computer::plug(const std::string path) {
	BUS().plug(path);
}

void Computer::loadSna(const std::string path) {
	BUS().loadSna(path);
}

void Computer::loadZ80(const std::string path) {
	BUS().loadZ80(path);
}

void Computer::loadTZX(const std::string path) {
	BUS().attachTZX(path);
}

void Computer::playTape() {
	std::cout << "Playing tape." << std::endl;
	BUS().playTape();
}

void Computer::stopTape() {
	std::cout << "Stopping tape." << std::endl;
	BUS().stopTape();
}

void Computer::toggleDebugMode() {
	std::cout << "Toggling debug mode." << std::endl;
	BUS().toggleDebugMode();
}

void Computer::toggleProfileMode() {
	std::cout << "Toggling profile mode." << std::endl;
	BUS().toggleProfileMode();
}

void Computer::runRasterLines() {
	BUS().renderLines();
}

bool Computer::handleKeyDown(SDL_Keycode key) {
	auto handled = Game::handleKeyDown(key);
	if (!handled) {
		switch (key) {
		case SDLK_F7:
		case SDLK_F8:
		case SDLK_F10:
		case SDLK_F11:
			handled = true;
			break;
		}
		BUS().ULA().pokeKey(key);
	}
	return handled;
}

bool Computer::handleKeyUp(SDL_Keycode key) {
	auto handled = Game::handleKeyUp(key);
	if (!handled) {
		switch (key) {
		case SDLK_F7:
			toggleProfileMode();
			handled = true;
			break;
		case SDLK_F8:
			toggleDebugMode();
			handled = true;
			break;
		case SDLK_F10:
			playTape();
			handled = true;
			break;
		case SDLK_F11:
			stopTape();
			handled = true;
			break;
		}
		BUS().ULA().pullKey(key);
	}
	return handled;
}

bool Computer::handleJoyButtonDown(const SDL_JoyButtonEvent event) {
	handleJoyButtonDown(joysticks(), event);
	return true;
}

bool Computer::handleJoyButtonUp(const SDL_JoyButtonEvent event) {
	handleJoyButtonUp(joysticks(), event);
	return true;
}

bool Computer::handleControllerButtonDown(SDL_ControllerButtonEvent event) {
	handleControllerButtonDown(joysticks(), event);
	return true;
}

bool Computer::handleControllerButtonUp(SDL_ControllerButtonEvent event) {
	handleControllerButtonUp(joysticks(), event);
	return true;
}

void Computer::handleJoyButtonDown(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_A:
		for (auto joystick : joysticks)
			joystick->pushFire();
		break;
	}
}

void Computer::handleJoyButtonUp(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_A:
		for (auto joystick : joysticks)
			joystick->releaseFire();
		break;
	}
}

void Computer::handleControllerButtonDown(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		for (auto joystick : joysticks)
			joystick->pushUp();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		for (auto joystick : joysticks)
			joystick->pushDown();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		for (auto joystick : joysticks)
			joystick->pushLeft();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		for (auto joystick : joysticks)
			joystick->pushRight();
		break;
	}
}

void Computer::handleControllerButtonUp(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		for (auto joystick : joysticks)
			joystick->releaseUp();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		for (auto joystick : joysticks)
			joystick->releaseDown();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		for (auto joystick : joysticks)
			joystick->releaseLeft();
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		for (auto joystick : joysticks)
			joystick->releaseRight();
		break;
	}
}

std::vector<Joystick*> Computer::joysticks() {
	std::vector<Joystick*> returned;
	for (size_t i = 0; i != BUS().numberOfExpansions(); ++i) {
		auto expansion = BUS().expansion(i);
		if (expansion->expansionType() == Expansion::Type::JOYSTICK) {
			auto joystick = (Joystick*)expansion.get();
			returned.push_back(joystick);
		}
	}
	return returned;
}
