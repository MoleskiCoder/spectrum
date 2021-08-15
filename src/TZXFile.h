#pragma once

#include <memory>
#include <string>

#include <Rom.h>
#include <Register.h>

#include "Loader.h"
#include "DataLoader.h"

class TZXFile final : public Loader {
private:
	enum BlockFlag { Header = 0, Data = 0xff };
	enum Type { ProgramBlock, NumberArrayBlock, CharacterArrayBlock, CodeFileBlock, UnknownBlock = 0xff };

	static const uint16_t ScreenAddress = 0x4000;
	static const int ScreenLength = 0x1b00;

	EightBit::Rom m_contents;
	DataLoader m_loader = { m_contents };

	// TAP header
	uint8_t m_tapBlockType = Type::UnknownBlock;
	std::string m_tapHeaderFilename;
	EightBit::register16_t m_dataBlockLength = 0xffff;
	EightBit::register16_t m_tapHeaderParameter1 = 0xffff;
	EightBit::register16_t m_tapHeaderParameter2 = 0xffff;

	// Program block
	auto lineNumber() const { return m_tapHeaderParameter1.word; }
	auto variableArea() const { return m_tapHeaderParameter2.word; }

	// code file block
	auto codeAddress() const { return m_tapHeaderParameter1.word; }
	auto codeLength() const { return m_dataBlockLength.word; }

	auto screenShotBlockType() const {
		return
			   (m_tapBlockType == Type::CodeFileBlock)
			&& (codeAddress() == ScreenAddress)
			&& (codeLength() == ScreenLength);

	}

	auto& contents() { return m_contents; }
	auto& loader() { return m_loader; }

	void processTapHeader(DataLoader& loader);
	void processTapData(DataLoader& loader, Board& board);
	void processTapBlock(const EightBit::Rom& data, Board& board);

	void readHeader();
	void readBlock(Board& board);

	void readStandardSpeedDataBlock(Board& board);

public:
	TZXFile(std::string path);

	void load(Board& board) override;
};
