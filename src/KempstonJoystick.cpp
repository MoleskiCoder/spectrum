#include "stdafx.h"
#include "KempstonJoystick.h"

KempstonJoystick::KempstonJoystick(Board& motherboard)
: Expansion(motherboard) {

	BUS().ports().ReadingPort.connect([this](uint8_t port) {
		if (port == 0x1f) {
		}
	});
}

void KempstonJoystick::raisePOWER() {
}
