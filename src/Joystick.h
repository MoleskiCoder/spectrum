#pragma once

#include "Expansion.h"

class Joystick : public Expansion {
public:
	Joystick(Board& motherboard);

	[[nodiscard]] Type expansionType() const noexcept final { return Type::JOYSTICK; }

	virtual void pushUp() = 0;
	virtual void pushDown() = 0;
	virtual void pushLeft() = 0;
	virtual void pushRight() = 0;
	virtual void pushFire() = 0;

	virtual void releaseUp() = 0;
	virtual void releaseDown() = 0;
	virtual void releaseLeft() = 0;
	virtual void releaseRight() = 0;
	virtual void releaseFire() = 0;
};
