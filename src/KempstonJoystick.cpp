#include "stdafx.h"
#include "KempstonJoystick.h"

#include <iostream>

KempstonJoystick::KempstonJoystick(Board& motherboard)
: Expansion(motherboard) {

	BUS().ports().ReadingPort.connect([this](uint8_t port) {
		if (port == 0x1f) {
		}
	});
}

void KempstonJoystick::raisePOWER() {
	std::cout << "Kempston joystick: power on" << std::endl;
}

void KempstonJoystick::lowerPOWER() {
	std::cout << "Kempston joystick: power off" << std::endl;
}
