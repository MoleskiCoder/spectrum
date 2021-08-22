#include "stdafx.h"

#include <cassert>
#include <iostream>
//#include <bitset>

#include "TAPBlock.h"
#include "DataLoader.h"

void TAPBlock::dumpHeaderInformation() const {

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

//boost::dynamic_bitset<> TAPFile::emit() const {
//	const auto& contents = loader().contents();
//	const auto size = contents.size();
//	boost::dynamic_bitset<> returned(size * 8);
//	for (int i = 0; i < size; ++i) {
//		const auto byte = contents.peek(i);
//		const std::bitset<8> bits(byte);
//		for (int j = 0; j < 8; ++j) {
//			returned.set(i * j, bits.test(j));
//		}
//	}
//	return returned;
//}

void TAPBlock::processHeader() {

	m_blockType = loader().fetchByte();
	auto filename_data = loader().fetchBytes(10);
	m_headerFilename = std::string((const char*)filename_data.data(), 10);
	m_dataBlockLength = loader().fetchWord();
	m_headerParameter1 = loader().fetchWord();
	m_headerParameter2 = loader().fetchWord();

	dumpHeaderInformation();
}

void TAPBlock::dumpDataInformation() const {
	std::cout << "TAP: Data" << std::endl;
}

void TAPBlock::processData() {
	dumpDataInformation();
}

void TAPBlock::process() {

	m_flag = loader().fetchByte();

	if (isDataBlock())
		processData();
	else if (isHeaderBlock())
		processHeader();
	else
		throw std::out_of_range("Unexpected block flag");

	loader().lock();
	m_block = loader().contents();
}

TAPBlock::TAPBlock() {}

TAPBlock::TAPBlock(const DataLoader& loader)
: m_loader(loader) {}

TAPBlock::TAPBlock(const TAPBlock& rhs)
: m_loader(rhs.m_loader),
  m_block(rhs.m_block),
  m_flag(rhs.m_flag),
  m_blockType(rhs.m_blockType),
  m_headerFilename(rhs.m_headerFilename),
  m_dataBlockLength(rhs.m_dataBlockLength),
  m_headerParameter1(rhs.m_headerParameter1),
  m_headerParameter2(rhs.m_headerParameter2) {}

TAPBlock& TAPBlock::operator=(const TAPBlock& rhs) {
	if (this != &rhs) {
		m_loader = rhs.m_loader;
		m_block = rhs.m_block;
		m_flag = rhs.m_flag;
		m_blockType = rhs.m_blockType;
		m_headerFilename = rhs.m_headerFilename;
		m_dataBlockLength = rhs.m_dataBlockLength;
		m_headerParameter1 = rhs.m_headerParameter1;
		m_headerParameter2 = rhs.m_headerParameter2;
	}
	return *this;
}
