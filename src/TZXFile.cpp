#include "stdafx.h"

//#include <iostream>

#include "TAPBlock.h"
#include "TZXFile.h"
#include "Board.h"

#if __cplusplus < 202002L
#   include <boost/bind.hpp>
#endif

TZXFile::TZXFile() {}

void TZXFile::readHeader() {

	const auto header = content().fetchBytes(10);
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

TAPBlock TZXFile::readBlock() {

	const auto id = content().fetchByte();
	std::cout << "** Block ID: " << std::hex << (int)id << std::endl;

	switch (id) {
	case 0x10:
		return readStandardSpeedDataBlock();
	default:
		throw std::runtime_error("Unhandled block ID");
	}
}

TAPBlock TZXFile::readStandardSpeedDataBlock() {

	const auto pause = content().fetchWord();
	std::cout << "TZX: Pause (ms): " << std::dec << pause.word << std::endl;
	const auto length = content().fetchWord();
	std::cout << "TZX: Length (bytes): " << std::dec << length.word << std::endl;
	auto bytes = content().fetchBytes(length.word);
	std::cout << "TZX: (Remaining (bytes): " << std::dec << (int)content().remaining() << ")" << std::endl;

	LittleEndianContent block;
	block.load(bytes.begin(), bytes.end());
	block.resetPosition();

	TAPBlock tap(block);
	tap.process();

	return tap;
}

void TZXFile::load(std::string path) {

	content().load(path);
	content().resetPosition();

	readHeader();

	blocks().clear();
	while (!content().finished())
		blocks().push_back(readBlock());
}

#if __cplusplus >= 202002L

TZXFile::amplitude_generator_t TZXFile::generate() const {
	for (const auto& block : blocks()) {
		auto generator = block.generate();
		while (generator) {
			const auto [ level, length ] = generator();
			for (int i = 0; i < length; ++i)
				co_yield level;
		}
	}
}

#else

void TZXFile::generate(amplitude_push_t& sink) const {
	for (const auto& block : blocks()) {
		ToneSequence::pulse_pull_t pulses(boost::bind(&TAPBlock::generate, &block, _1));
		for (const auto& pulse : pulses) {
			const auto& [level, length] = pulse;
			for (int i = 0; i < length; ++i)
				sink(level);
		}
	}
}

#endif