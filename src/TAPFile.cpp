#include "stdafx.h"

#include <cassert>
#include <iostream>
#include <bitset>

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

boost::dynamic_bitset<> TAPFile::emit() const {
	const auto& contents = loader().contents();
	const auto size = contents.size();
	boost::dynamic_bitset<> returned(size * 8);
	for (int i = 0; i < size; ++i) {
		const auto byte = contents.peek(i);
		const std::bitset<8> bits(byte);
		for (int j = 0; j < 8; ++j) {
			returned.set(i * j, bits.test(j));
		}
	}
	return returned;
}

boost::dynamic_bitset<> TAPFile::processHeader() {

	m_blockType = loader().fetchByte();
	auto filename_data = loader().fetchBytes(10);
	m_headerFilename = std::string((const char*)filename_data.data(), 10);
	m_dataBlockLength = loader().fetchWord();
	m_headerParameter1 = loader().fetchWord();
	m_headerParameter2 = loader().fetchWord();

	dumpHeaderInformation();

	loader().lock();
	return emit();
}

void TAPFile::dumpDataInformation() const {
	std::cout << "TAP: Data" << std::endl;
}

boost::dynamic_bitset<> TAPFile::processData() {
	dumpDataInformation();
	loader().lock();
	return emit();
}

boost::dynamic_bitset<> TAPFile::processBlock() {
	m_flag = loader().fetchByte();
	if (!isHeaderBlock())
		throw std::out_of_range("Unexpected block flag (should have been a header block)");
	return processHeader();
}

boost::dynamic_bitset<> TAPFile::processBlock(const TAPFile& header) {

	m_flag = loader().fetchByte();
	if (!isDataBlock())
		throw std::out_of_range("Unexpected block flag (should have been a data block)");

	blockType() = header.blockType();
	headerFilename() = header.headerFilename();
	dataBlockLength() = header.dataBlockLength();
	headerParameter1() = header.headerParameter1();
	headerParameter2() = header.headerParameter2();

	return processData();
}

TAPFile::TAPFile(DataLoader& loader)
: m_loader(loader) {}
