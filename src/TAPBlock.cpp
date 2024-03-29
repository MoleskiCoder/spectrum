#include "stdafx.h"

#include <cassert>
#include <iostream>

#include "TAPBlock.h"
#include "Content.h"

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

void TAPBlock::processHeader() {

	m_blockType = content().fetchByte();
	auto filename_data = content().fetchBytes(10);
	m_headerFilename = std::string((const char*)filename_data.data(), 10);
	m_dataBlockLength = content().fetchWord();
	m_headerParameter1 = content().fetchWord();
	m_headerParameter2 = content().fetchWord();

	dumpHeaderInformation();
}

void TAPBlock::dumpDataInformation() const {
	std::cout << "TAP: Data" << std::endl;
}

void TAPBlock::processData() {
	dumpDataInformation();
}

void TAPBlock::process() {

	m_flag = content().fetchByte();

	if (isDataBlock())
		processData();
	else if (isHeaderBlock())
		processHeader();
	else
		throw std::out_of_range("Unexpected block flag");

	content().lock();
}

TAPBlock::TAPBlock() {}

TAPBlock::TAPBlock(const LittleEndianContent& content)
: m_content(content) {}

TAPBlock::TAPBlock(const TAPBlock& rhs)
: m_content(rhs.m_content),
  m_flag(rhs.m_flag),
  m_blockType(rhs.m_blockType),
  m_headerFilename(rhs.m_headerFilename),
  m_dataBlockLength(rhs.m_dataBlockLength),
  m_headerParameter1(rhs.m_headerParameter1),
  m_headerParameter2(rhs.m_headerParameter2) {}

TAPBlock& TAPBlock::operator=(const TAPBlock& rhs) {
	if (this != &rhs) {
		m_content = rhs.m_content;
		m_flag = rhs.m_flag;
		m_blockType = rhs.m_blockType;
		m_headerFilename = rhs.m_headerFilename;
		m_dataBlockLength = rhs.m_dataBlockLength;
		m_headerParameter1 = rhs.m_headerParameter1;
		m_headerParameter2 = rhs.m_headerParameter2;
	}
	return *this;
}
