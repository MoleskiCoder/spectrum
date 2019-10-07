#include "stdafx.h"
#include "Interface2Joystick.h"

Interface2Joystick::Interface2Joystick(Board& motherboard)
: Joystick(motherboard) {}

void Interface2Joystick::pushUp() {
	BUS().ULA().pokeKey(SDLK_4);
}

void Interface2Joystick::pushDown() {
	BUS().ULA().pokeKey(SDLK_3);
}

void Interface2Joystick::pushLeft() {
	BUS().ULA().pokeKey(SDLK_1);
}

void Interface2Joystick::pushRight() {
	BUS().ULA().pokeKey(SDLK_2);
}

void Interface2Joystick::pushFire() {
	BUS().ULA().pokeKey(SDLK_5);
}

void Interface2Joystick::releaseUp() {
	BUS().ULA().pullKey(SDLK_4);
}

void Interface2Joystick::releaseDown() {
	BUS().ULA().pullKey(SDLK_3);
}

void Interface2Joystick::releaseLeft() {
	BUS().ULA().pullKey(SDLK_1);
}

void Interface2Joystick::releaseRight() {
	BUS().ULA().pullKey(SDLK_2);
}

void Interface2Joystick::releaseFire() {
	BUS().ULA().pullKey(SDLK_5);
}
