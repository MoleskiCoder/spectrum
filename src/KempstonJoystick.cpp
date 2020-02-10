#include "stdafx.h"
#include "KempstonJoystick.h"

KempstonJoystick::KempstonJoystick(Board& motherboard)
: Joystick(motherboard) {
	BUS().ReadingByte.connect([this](EightBit::EventArgs) {
		if (BUS().CPU().requestingIO()) {
			const auto port = BUS().ADDRESS().low;
			if (port == 0x1f)
				BUS().ports().writeInputPort(port, m_contents);
		}
	});
}

void KempstonJoystick::pushUp() {
	m_contents = EightBit::Chip::setBit(m_contents, Up);
}

void KempstonJoystick::pushDown() {
	m_contents = EightBit::Chip::setBit(m_contents, Down);
}

void KempstonJoystick::pushLeft() {
	m_contents = EightBit::Chip::setBit(m_contents, Left);
}

void KempstonJoystick::pushRight() {
	m_contents = EightBit::Chip::setBit(m_contents, Right);
}

void KempstonJoystick::pushFire() {
	m_contents = EightBit::Chip::setBit(m_contents, Fire);
}

void KempstonJoystick::releaseUp() {
	m_contents = EightBit::Chip::clearBit(m_contents, Up);
}

void KempstonJoystick::releaseDown() {
	m_contents = EightBit::Chip::clearBit(m_contents, Down);
}

void KempstonJoystick::releaseLeft() {
	m_contents = EightBit::Chip::clearBit(m_contents, Left);
}

void KempstonJoystick::releaseRight() {
	m_contents = EightBit::Chip::clearBit(m_contents, Right);
}

void KempstonJoystick::releaseFire() {
	m_contents = EightBit::Chip::clearBit(m_contents, Fire);
}
