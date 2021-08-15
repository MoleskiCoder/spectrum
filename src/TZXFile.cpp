#include "stdafx.h"

#include <iostream>


#include "TZXFile.h"

TZXFile::TZXFile(std::string path)
: Loader(path) {}

//

uint8_t TZXFile::readByte(int position) const {
	if (position < 0)
		throw std::runtime_error("Negative positions are not allowed");
	return m_contents.peek(position);
}

std::vector<uint8_t> TZXFile::readBytes(int position, int amount) const {

	if (amount <= 0)
		throw std::runtime_error("Amount to be read must be greater than zero");
	if ((position + amount) > m_contents.size())
		throw std::runtime_error("Not enough bytes in TZX file remaining");

	std::vector<uint8_t> returned(amount);
	for (int i = 0; i < amount; ++i)
		returned[i] = readByte(position + i);

	return returned;
}

std::vector<uint8_t> TZXFile::fetchBytes(int amount) {
	const auto returned = readBytes(m_position, amount);
	m_position += amount;
	return returned;
}

uint8_t TZXFile::fetchByte() {
	const auto bytes = fetchBytes(1);
	return bytes[0];
}

//

EightBit::register16_t TZXFile::readWord(int position) const {
	const auto bytes = readBytes(position, 2);
	return { bytes[0], bytes[1] };	// Little endian: low, high
}

std::vector<EightBit::register16_t> TZXFile::readWords(int position, int amount) const {
	std::vector<EightBit::register16_t> returned(amount);
	for (int i = 0; i < amount; ++i)
		returned[i] = readWord(position + i * 2);
	return returned;
}

std::vector<EightBit::register16_t> TZXFile::fetchWords(int amount) {
	const auto returned = readWords(m_position, amount);
	m_position += amount * 2;
	return returned;
}

EightBit::register16_t TZXFile::fetchWord() {
	const auto words = fetchWords(1);
	return words[0];
}

//

void TZXFile::readHeader() {

	const auto header = fetchBytes(10);
	const std::string signature((const char*)header.data(), 7);
	const auto eof = header[7];
	const auto tzx_major = header[8];
	const auto tzx_minor = header[9];

	if (signature != "ZXTape!")
		throw std::runtime_error("Unknown signature");
	if (eof != 0x1a)		// ^Z
		throw std::runtime_error("Unknown header signature EOF marker");

	std::cout << "TZX Major: " << (int)tzx_major << std::endl;
	std::cout << "TZX Minor: " << (int)tzx_minor << std::endl;
}

//

void TZXFile::readBlock() {

	const auto id = fetchByte();
	std::cout << "Block ID: " << std::hex << (int)id << std::endl;

	switch (id) {
	case 0x10:
		readStandardSpeedDataBlock();
		break;
	default:
		throw std::runtime_error("Unhandled block ID");
		break;
	}
}

//

void TZXFile::readStandardSpeedDataBlock() {
	const auto pause = fetchWord();
	std::cout << "Pause (ms): " << std::dec << pause.word << std::endl;
	const auto length = fetchWord();
	std::cout << "Length (bytes): " << std::dec << length.word << std::endl;
	auto bytes = fetchBytes(length.word);
	std::cout << "(Remaining (bytes): " << std::dec << (int)remaining() << ")" << std::endl;
}

//

void TZXFile::load(Board& board) {

	m_contents.load(path());
	m_position = 0;

	readHeader();

	while (!finished())
		readBlock();
}






