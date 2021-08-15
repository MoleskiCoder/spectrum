#include "stdafx.h"

#include <cassert>
#include <iostream>

#include "TZXFile.h"

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

void TZXFile::readBlock() {

	const auto id = loader().fetchByte();
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

void TZXFile::processTapHeader(DataLoader& loader) {

	std::cout << "TAP: Header" << std::endl;

	auto type = loader.fetchByte();
	auto filename_data = loader.fetchBytes(10);
	auto data_block_length = loader.fetchWord();
	auto parameter1 = loader.fetchWord();
	auto parameter2 = loader.fetchWord();

	std::cout << "TAP: type " << std::dec << " (" << (int)type << ") : ";
	switch (type) {
	case Type::Program:
		std::cout << "Program" << std::endl;
		{
			auto lineNumber = parameter1.word;
			if (lineNumber < 32768)
				std::cout << "TAP: LINE: " << std::dec << (int)lineNumber << std::endl;
			auto variableArea = parameter2.word;
			std::cout << "TAP: Variable area: " << std::dec << (int)variableArea << std::endl;
		}
		break;
	case Type::NumberArray:
		std::cout << "Number array" << std::endl;
		break;
	case Type::CharacterArray:
		std::cout << "Character array" << std::endl;
		break;
	case Type::CodeFile: {
			const auto address = parameter1.word;
			const auto codeLength = data_block_length;
			const auto screen = (address == 16384) && (codeLength == 6912);
			//assert(parameter2.word == 32768);
			if (screen)
				std::cout << "Screen shot" << std::endl;
			else
				std::cout << "Code file" << std::endl;
		}
		break;
	}

	const std::string filename((const char*)filename_data.data(), 10);
	std::cout << "TAP: Filename: " << filename << std::endl;

	//std::cout << "TAP: Length of data block: " << std::dec << (int)data_block_length.word << std::endl;

	//std::cout << "TAP: parameter 1: " << std::dec << (int)parameter1.word << std::endl;
	//std::cout << "TAP: parameter 2: " << std::dec << (int)parameter2.word << std::endl;
}

void TZXFile::processTapData(DataLoader& loader) {

	std::cout << "TAP: Data" << std::endl;

}

void TZXFile::processTapBlock(const EightBit::Rom& data) {
	DataLoader loader(data);
	loader.resetPosition();
	auto flag = loader.fetchByte();
	switch (flag) {
	case BlockFlag::Header:
		processTapHeader(loader);
		break;
	case BlockFlag::Data:
		processTapData(loader);
		break;
	default:
		assert(false && "Unknown TAP block flag");
		break;
	}
}

void TZXFile::readStandardSpeedDataBlock() {

	const auto pause = loader().fetchWord();
	std::cout << "TZX: Pause (ms): " << std::dec << pause.word << std::endl;
	const auto length = loader().fetchWord();
	std::cout << "TZX: Length (bytes): " << std::dec << length.word << std::endl;
	auto bytes = loader().fetchBytes(length.word);
	std::cout << "TZX: (Remaining (bytes): " << std::dec << (int)loader().remaining() << ")" << std::endl;

	EightBit::Rom tap_data;
	tap_data.load(bytes);
	processTapBlock(tap_data);
}

void TZXFile::load(Board& board) {

	m_contents.load(path());
	loader().resetPosition();

	readHeader();

	while (!loader().finished())
		readBlock();
}