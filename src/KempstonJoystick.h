#pragma once

#include <cstdint>

#include "Joystick.h"

class Board;

class KempstonJoystick final : public Joystick {
public:
	KempstonJoystick(Board& motherboard);

	void pushUp() final;
	void pushDown() final;
	void pushLeft() final;
	void pushRight() final;
	void pushFire() final;

	void releaseUp() final;
	void releaseDown() final;
	void releaseLeft() final;
	void releaseRight() final;
	void releaseFire() final;

private:
	enum {
		Right	= 0b00000001,
		Left	= 0b00000010,
		Down	= 0b00000100,
		Up		= 0b00001000,
		Fire	= 0b00010000,
	};

	uint8_t m_contents = 0x00;
};

