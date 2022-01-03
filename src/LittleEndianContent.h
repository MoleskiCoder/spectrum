#pragma once

#include "Content.h"

class LittleEndianContent : public Content {
public:
	[[nodiscard]] EightBit::register16_t readWord(uint16_t position) final;
};
