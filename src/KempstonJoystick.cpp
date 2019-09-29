#include "stdafx.h"
#include "KempstonJoystick.h"

#include <iostream>

KempstonJoystick::KempstonJoystick(Board& motherboard)
: Joystick(motherboard) {

	BUS().ports().ReadingPort.connect([this](uint8_t port) {
		if (port == 0x1f)
			BUS().ports().writeInputPort(port, m_contents);
	});
}

void KempstonJoystick::pushUp() {
	m_contents |= Up;
}

void KempstonJoystick::pushDown() {
	m_contents |= Down;
}

void KempstonJoystick::pushLeft() {
	m_contents |= Left;
}

void KempstonJoystick::pushRight() {
	m_contents |= Right;
}

void KempstonJoystick::pushFire() {
	m_contents |= Fire;
}

void KempstonJoystick::releaseUp() {
	m_contents &= ~Up;
}

void KempstonJoystick::releaseDown() {
	m_contents &= ~Down;
}

void KempstonJoystick::releaseLeft() {
	m_contents &= ~Left;
}

void KempstonJoystick::releaseRight() {
	m_contents &= ~Right;
}

void KempstonJoystick::releaseFire() {
	m_contents &= ~Fire;
}
