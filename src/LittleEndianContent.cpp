#include "stdafx.h"
#include "LittleEndianContent.h"

EightBit::register16_t LittleEndianContent::readWord(uint16_t position) {
	const auto bytes = readBytes(position, 2);
	return { bytes[0], bytes[1] };	// Little endian: low, high
}
