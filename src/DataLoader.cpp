#include "stdafx.h"

#include <iostream>

#include "DataLoader.h"

DataLoader::DataLoader(const EightBit::Rom& rom)
: m_contents(rom) {}

void DataLoader::move(int amount) {
	if (locked())
		throw new std::logic_error("Data loader has been locked");
	position() += amount;
}

uint8_t DataLoader::readByte(int position) const {
	if (position < 0)
		throw std::runtime_error("Negative positions are not allowed");
	return contents().peek(position);
}

std::vector<uint8_t> DataLoader::readBytes(int position, int amount) const {

	if (amount <= 0)
		throw std::runtime_error("Amount to be read must be greater than zero");
	if ((position + amount) > contents().size())
		throw std::runtime_error("Not enough bytes in data remaining");

	std::vector<uint8_t> returned(amount);
	for (int i = 0; i < amount; ++i)
		returned[i] = readByte(position + i);

	return returned;
}

std::vector<uint8_t> DataLoader::fetchBytes(int amount) {
	const auto returned = readBytes(position(), amount);
	move(amount);
	return returned;
}

uint8_t DataLoader::fetchByte() {
	const auto bytes = fetchBytes(1);
	return bytes[0];
}

EightBit::register16_t DataLoader::readWord(int position) const {
	const auto bytes = readBytes(position, 2);
	return { bytes[0], bytes[1] };	// Little endian: low, high
}

std::vector<EightBit::register16_t> DataLoader::readWords(int position, int amount) const {
	std::vector<EightBit::register16_t> returned(amount);
	for (int i = 0; i < amount; ++i)
		returned[i] = readWord(position + i * 2);
	return returned;
}

std::vector<EightBit::register16_t> DataLoader::fetchWords(int amount) {
	const auto returned = readWords(position(), amount);
	move(amount * sizeof(uint16_t));
	return returned;
}

EightBit::register16_t DataLoader::fetchWord() {
	const auto words = fetchWords(1);
	return words[0];
}
