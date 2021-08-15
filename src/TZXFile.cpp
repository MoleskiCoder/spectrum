#include "stdafx.h"

#include <cassert>
#include <iostream>

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

void TZXFile::processTapHeader(DataLoader& loader) {

	std::cout << "TAP: Header" << std::endl;

	m_tapBlockType = loader.fetchByte();
	auto filename_data = loader.fetchBytes(10);
	m_dataBlockLength = loader.fetchWord();
	m_tapHeaderParameter1 = loader.fetchWord();
	m_tapHeaderParameter2 = loader.fetchWord();

	std::cout << "TAP: type " << std::dec << " (" << (int)m_tapBlockType << ") : ";
	switch (m_tapBlockType) {
	case Type::ProgramBlock:
		std::cout << "Program" << std::endl;
		if (lineNumber() < 0x8000)
			std::cout << "TAP: LINE: " << std::dec << (int)lineNumber() << std::endl;
		std::cout << "TAP: Variable area: " << std::dec << (int)variableArea() << std::endl;
		break;
	case Type::NumberArrayBlock:
		std::cout << "Number array" << std::endl;
		break;
	case Type::CharacterArrayBlock:
		std::cout << "Character array" << std::endl;
		break;
	case Type::CodeFileBlock: {
			//assert(parameter2.word == 0x8000);
			if (screenShotBlockType())
				std::cout << "Screen shot" << std::endl;
			else
				std::cout << "Code file" << std::endl;
		}
		break;
	}

	m_tapHeaderFilename = std::string((const char*)filename_data.data(), 10);

	std::cout << "TAP: Filename: " << m_tapHeaderFilename << std::endl;
	std::cout << "TAP: Length of data block: " << std::dec << (int)m_dataBlockLength.word << std::endl;
}

void TZXFile::processTapData(DataLoader& loader, Board& board) {

	std::cout << "TAP: Data" << std::endl;

	if (screenShotBlockType()) {

		auto screen_data = loader.fetchBytes(ScreenLength);

		for (int i = 0; i < ScreenLength; ++i)
			board.poke(ScreenAddress + i, screen_data[i]);
	}

}

void TZXFile::processTapBlock(const EightBit::Rom& data, Board& board) {
	DataLoader loader(data);
	loader.resetPosition();
	auto flag = loader.fetchByte();
	switch (flag) {
	case BlockFlag::Header:
		processTapHeader(loader);
		break;
	case BlockFlag::Data:
		processTapData(loader, board);
		break;
	default:
		assert(false && "Unknown TAP block flag");
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
	processTapBlock(tap_data, board);
}

void TZXFile::load(Board& board) {

	m_contents.load(path());
	loader().resetPosition();

	readHeader();

	while (!loader().finished())
		readBlock(board);
}