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

bool Computer::handleKeyDown(SDL_Keycode key) {
	const auto handled = Game::handleKeyDown(key);
	if (!handled)
		BUS().ULA().pokeKey(key);
	return true;
}

bool Computer::handleKeyUp(SDL_Keycode key) {
	const auto handled = Game::handleKeyUp(key);
	if (!handled)
		BUS().ULA().pullKey(key);
	return true;
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
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->pushFire(); });
		break;
	}
}

void Computer::handleJoyButtonUp(std::vector<Joystick*> joysticks, SDL_JoyButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_A:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->releaseFire(); });
		break;
	}
}

void Computer::handleControllerButtonDown(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->pushUp(); });
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->pushDown(); });
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->pushLeft(); });
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->pushRight(); });
		break;
	}
}

void Computer::handleControllerButtonUp(std::vector<Joystick*> joysticks, SDL_ControllerButtonEvent event) {
	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->releaseUp(); });
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->releaseDown(); });
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->releaseLeft(); });
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		std::for_each(joysticks.cbegin(), joysticks.cend(), [](Joystick* joystick) { joystick->releaseRight(); });
		break;
	}
}

std::vector<Joystick*> Computer::joysticks() {
	std::vector<Joystick*> returned;
	for (size_t i = 0; i != BUS().numberOfExpansions(); ++i) {
		auto expansion = BUS().expansion(i);
		if (expansion->expansionType() == Expansion::JOYSTICK) {
			auto joystick = (Joystick*)expansion.get();
			returned.push_back(joystick);
		}
	}
	return returned;
}
