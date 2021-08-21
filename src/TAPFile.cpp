#include "stdafx.h"

#include <cassert>
#include <iostream>

#include "Board.h"
#include "TAPFile.h"
#include "DataLoader.h"

void TAPFile::dumpHeaderInformation() const {

	std::cout << "TAP: Header" << std::endl;

	std::cout << "TAP: type " << std::dec << " (" << (int)blockType() << ") : ";
	switch (blockType()) {
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
	case Type::CodeFileBlock:
		if (screenShotBlockType())
			std::cout << "Screen shot" << std::endl;
		else
			std::cout << "Code file" << std::endl;
		break;
	}

	std::cout << "TAP: Filename: " << headerFilename() << std::endl;
	std::cout << "TAP: Length of data block: " << std::dec << (int)dataBlockLength() << std::endl;

}

void TAPFile::processHeader() {
	m_blockType = loader().fetchByte();
	auto filename_data = loader().fetchBytes(10);
	m_headerFilename = std::string((const char*)filename_data.data(), 10);
	m_dataBlockLength = loader().fetchWord();
	m_headerParameter1 = loader().fetchWord();
	m_headerParameter2 = loader().fetchWord();
	loader().lock();
	dumpHeaderInformation();
}

void TAPFile::processData(Board& board) {

	std::cout << "TAP: Data" << std::endl;

	auto data = loader().fetchBytes(dataBlockLength());

	switch (blockType()) {
	case Type::ProgramBlock:
		break;
	case Type::NumberArrayBlock:
		throw std::out_of_range("NumberArrayBlock is not handled");
		break;
	case Type::CharacterArrayBlock:
		throw std::out_of_range("CharacterArrayBlock is not handled");
		break;
	case Type::CodeFileBlock:
		for (size_t i = 0; i != data.size(); ++i)
			board.poke(codeAddress() + i, data[i]);
		break;
	default:
		throw std::out_of_range("Unknown block type");
		break;
	}
}

void TAPFile::processBlock() {
	m_flag = loader().fetchByte();
	if (!isHeaderBlock())
		throw std::out_of_range("Unexpected block flag (should have been a header block)");
	processHeader();
}

void TAPFile::processBlock(const TAPFile& header, Board& board) {

	m_flag = loader().fetchByte();
	if (!isDataBlock())
		throw std::out_of_range("Unexpected block flag (should have been a data block)");

	blockType() = header.blockType();
	headerFilename() = header.headerFilename();
	dataBlockLength() = header.dataBlockLength();
	headerParameter1() = header.headerParameter1();
	headerParameter2() = header.headerParameter2();

	processData(board);
}

TAPFile::TAPFile(DataLoader& loader)
: m_loader(loader) {}
