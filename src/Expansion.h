#pragma once

#include <Device.h>

#include "Board.h"

class Expansion : public EightBit::Device {
public:
	Expansion(Board& motherboard);

	enum class Type { JOYSTICK };

	[[nodiscard]] virtual Type expansionType() const noexcept = 0;

protected:
	[[nodiscard]] constexpr auto& BUS() noexcept { return m_motherboard; }

private:
	Board& m_motherboard;
};
