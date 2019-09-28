#pragma once

#include "Expansion.h"

class KempstonJoystick final : public Expansion {
public:
	KempstonJoystick(Board& motherboard);

	virtual void raisePOWER() final;
private:
};

