#pragma once

#include "Content.h"

class LittleEndianContent final : public Content {
public:
	[[nodiscard]] EightBit::register16_t readWord(int position) final;
};
