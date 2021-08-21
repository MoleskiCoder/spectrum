#include "stdafx.h"

#include <iostream>

#include "TAPFile.h"
#include "TZXFile.h"
#include "Board.h"

TZXFile::TZXFile(std::string path)
: m_path(path) {}

void TZXFile::readHeader() {

	const auto header = loader().fetchBytes(10);
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

boost::dynamic_bitset<> TZXFile::readBlock() {

	const auto id = loader().fetchByte();
	std::cout << "** Block ID: " << std::hex << (int)id << std::endl;

	switch (id) {
	case 0x10:
		return readStandardSpeedDataBlock();
	default:
		throw std::runtime_error("Unhandled block ID");
	}
}

boost::dynamic_bitset<> TZXFile::readStandardSpeedDataBlock() {

	const auto pause = loader().fetchWord();
	std::cout << "TZX: Pause (ms): " << std::dec << pause.word << std::endl;
	const auto length = loader().fetchWord();
	std::cout << "TZX: Length (bytes): " << std::dec << length.word << std::endl;
	auto bytes = loader().fetchBytes(length.word);
	std::cout << "TZX: (Remaining (bytes): " << std::dec << (int)loader().remaining() << ")" << std::endl;

	EightBit::Rom tap_data;
	tap_data.load(bytes);

	if (expectingHeader()) {

		// What's next?
		m_expecting = TAPFile::BlockFlag::Data;

		DataLoader tap_loader(tap_data);
		tap_loader.resetPosition();

		m_headerTAP = std::make_unique<TAPFile>(tap_loader);
		return headerTAP().processBlock();

	} else if (expectingData()) {

		// What's next?
		m_expecting = TAPFile::BlockFlag::Header;

		DataLoader tap_loader(tap_data);
		tap_loader.resetPosition();

		TAPFile tap(tap_loader);
		return tap.processBlock(headerTAP());

	} else {
		throw std::logic_error("Invalid block expectation!");
	}
}

std::vector<boost::dynamic_bitset<>> TZXFile::load() {

	m_contents.load(path());
	loader().resetPosition();

	readHeader();

	std::vector<boost::dynamic_bitset<>> data;
	while (!loader().finished())
		data.push_back(readBlock());
	return data;
}