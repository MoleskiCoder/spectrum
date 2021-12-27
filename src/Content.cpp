#include "stdafx.h"

#include <iostream>

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

void Content::move(int amount) {
	if (locked())
		throw new std::logic_error("Content has been locked");
	position() += amount;
}

uint8_t Content::readByte(int position) const {
	if (position < 0)
		throw std::runtime_error("Negative positions are not allowed");
	return peek(position);
}

std::vector<uint8_t> Content::readBytes(int position, int amount) const {

	if (amount <= 0)
		throw std::runtime_error("Amount to be read must be greater than zero");
	if ((position + amount) > size())
		throw std::runtime_error("Not enough bytes in content remaining");

	std::vector<uint8_t> returned(amount);
	for (int i = 0; i < amount; ++i)
		returned[i] = readByte(position + i);

	return returned;
}

std::vector<uint8_t> Content::fetchBytes(int amount) {
	const auto returned = readBytes(position(), amount);
	move(amount);
	return returned;
}

uint8_t Content::fetchByte() {
	const auto bytes = fetchBytes(1);
	return bytes[0];
}

std::vector<EightBit::register16_t> Content::readWords(int position, int amount) const {
	std::vector<EightBit::register16_t> returned(amount);
	for (int i = 0; i < amount; ++i)
		returned[i] = readWord(position + i * 2);
	return returned;
}

std::vector<EightBit::register16_t> Content::fetchWords(int amount) {
	const auto returned = readWords(position(), amount);
	move(amount * sizeof(uint16_t));
	return returned;
}

EightBit::register16_t Content::fetchWord() {
	const auto words = fetchWords(1);
	return words[0];
}
