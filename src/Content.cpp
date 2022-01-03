#include "stdafx.h"
#include "Content.h"

Content::Content() {}

Content::Content(const Content& rhs)
: EightBit::Rom(rhs),
  m_position(rhs.m_position),
  m_locked(rhs.m_locked) {}

Content& Content::operator=(const Content& rhs) {
	if (this != &rhs) {
		EightBit::Rom::operator=(rhs);
		m_position = rhs.m_position;
		m_locked = rhs.m_locked;
	}
	return *this;
}

bool Content::operator==(const Content& rhs) const {
	return
		Rom::operator==(rhs)
		&& locked() == rhs.locked()
		&& position() == rhs.position();
}

std::vector<EightBit::register16_t> Content::readWords(uint16_t position, uint16_t amount) {
	std::vector<EightBit::register16_t> returned(amount);
	for (uint16_t i = 0; i < amount; ++i)
		returned[i] = readWord(position + i * 2);
	return returned;
}

std::vector<EightBit::register16_t> Content::fetchWords(uint16_t amount) {
	const auto returned = readWords(position(), amount);
	move(amount * sizeof(uint16_t));
	return returned;
}

EightBit::register16_t Content::fetchWord() {
	const auto words = fetchWords(1);
	return words[0];
}
