#include "stdafx.h"
#include "Computer.h"
#include "Configuration.h"

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

void Computer::runRasterLines() {
	BUS().runFrame();
}

void Computer::handleKeyDown(SDL_Keycode key) {
	BUS().ULA().pokeKey(key);
}

void Computer::handleKeyUp(SDL_Keycode key) {
	BUS().ULA().pullKey(key);
}
