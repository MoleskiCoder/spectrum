#pragma once

#include <Device.h>

#include "Board.h"

class Expansion : public EightBit::Device {
public:
	Expansion(Board& motherboard);

protected:
	Board& BUS() { return m_motherboard; }

private:
	Board& m_motherboard;
};
