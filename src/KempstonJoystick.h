#pragma once

#include "Expansion.h"

class KempstonJoystick final : public Expansion {
public:
	KempstonJoystick(Board& motherboard);

	void raisePOWER() final;
	void lowerPOWER() final;
};

