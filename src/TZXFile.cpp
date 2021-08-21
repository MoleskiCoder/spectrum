#include "stdafx.h"

#include <cassert>
#include <iostream>

#include "TAPFile.h"
#include "TZXFile.h"
#include "Board.h"

TZXFile::TZXFile(std::string path)
: Loader(path) {}

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

void TZXFile::readBlock(Board& board) {

	const auto id = loader().fetchByte();
	std::cout << "** Block ID: " << std::hex << (int)id << std::endl;

	switch (id) {
	case 0x10:
		readStandardSpeedDataBlock(board);
		break;
	default:
		throw std::runtime_error("Unhandled block ID");
		break;
	}
}

void TZXFile::readStandardSpeedDataBlock(Board& board) {

	const auto pause = loader().fetchWord();
	std::cout << "TZX: Pause (ms): " << std::dec << pause.word << std::endl;
	const auto length = loader().fetchWord();
	std::cout << "TZX: Length (bytes): " << std::dec << length.word << std::endl;
	auto bytes = loader().fetchBytes(length.word);
	std::cout << "TZX: (Remaining (bytes): " << std::dec << (int)loader().remaining() << ")" << std::endl;

	EightBit::Rom tap_data;
	tap_data.load(bytes);

	if (expectingHeader()) {

		DataLoader tap_loader(tap_data);
		tap_loader.resetPosition();

		m_headerTAP = std::make_unique<TAPFile>(tap_loader);
		headerTAP().processBlock();

		m_expecting = TAPFile::BlockFlag::Data;

	} else if (expectingData()) {

		DataLoader tap_loader(tap_data);
		tap_loader.resetPosition();

		TAPFile tap(tap_loader);
		tap.processBlock(headerTAP(), board);

		m_expecting = TAPFile::BlockFlag::Header;

	} else {
		throw std::logic_error("Invalid block expectation!");
	}
}

void TZXFile::load(Board& board) {

	m_contents.load(path());
	loader().resetPosition();

	readHeader();

	while (!loader().finished())
		readBlock(board);
}