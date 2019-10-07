#pragma once

#include "Joystick.h"

class Interface2Joystick final : public Joystick {
public:
	Interface2Joystick(Board& motherBoard);

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
};

