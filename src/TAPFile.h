#pragma once

#include <cstdint>
#include <string>

#include <Register.h>

#include "Loader.h"
#include "DataLoader.h"

class TAPFile final {
public:
	enum BlockFlag { Header = 0, Data = 0xff };

private:
	enum Type { ProgramBlock, NumberArrayBlock, CharacterArrayBlock, CodeFileBlock, UnknownBlock = 0xff };

	static const uint16_t ScreenAddress = 0x4000;
	static const int ScreenLength = 0x1b00;

	DataLoader& m_loader;

	uint8_t m_flag = 128;	// Not a BlockFlag!

	// TAP header
	uint8_t m_blockType = Type::UnknownBlock;
	std::string m_headerFilename;
	EightBit::register16_t m_dataBlockLength = 0xffff;
	EightBit::register16_t m_headerParameter1 = 0xffff;
	EightBit::register16_t m_headerParameter2 = 0xffff;

	auto& loader() { return m_loader; }

	void dumpHeaderInformation() const;
	void processHeader();
	void processData(Board& board);

public:
	TAPFile(DataLoader& loader);

	// Block interpretation

	[[nodiscard]] constexpr auto flag() const noexcept { return m_flag; }
	[[nodiscard]] constexpr auto isHeaderBlock() const noexcept { return flag() == BlockFlag::Header; }
	[[nodiscard]] constexpr auto isDataBlock() const noexcept { return flag() == BlockFlag::Data; }

	// Header interpretation

	[[nodiscard]] constexpr auto& blockType() noexcept { return m_blockType; }
	[[nodiscard]] constexpr auto blockType() const noexcept { return m_blockType; }
	[[nodiscard]] constexpr auto isProgramBlock() const noexcept { return blockType() == Type::ProgramBlock; }
	[[nodiscard]] constexpr auto isNumberArrayBlock() const noexcept { return blockType() == Type::NumberArrayBlock; }
	[[nodiscard]] constexpr auto isCharacterArrayBlock() const noexcept { return blockType() == Type::CharacterArrayBlock; }
	[[nodiscard]] constexpr auto isCodeFileBlock() const noexcept { return blockType() == Type::CodeFileBlock; }

	[[nodiscard]] auto& headerFilename() { return m_headerFilename; }
	[[nodiscard]] auto headerFilename() const { return m_headerFilename; }
	[[nodiscard]] constexpr auto& dataBlockLength() noexcept { return m_dataBlockLength.word; }
	[[nodiscard]] constexpr auto dataBlockLength() const noexcept { return m_dataBlockLength.word; }
	[[nodiscard]] constexpr auto& headerParameter1() noexcept { return m_headerParameter1.word; }
	[[nodiscard]] constexpr auto headerParameter1() const noexcept { return m_headerParameter1.word; }
	[[nodiscard]] constexpr auto& headerParameter2() noexcept { return m_headerParameter2.word; }
	[[nodiscard]] constexpr auto headerParameter2() const noexcept { return m_headerParameter2.word; }

	// Program block
	[[nodiscard]] auto lineNumber() const { return headerParameter1(); }
	[[nodiscard]] auto variableArea() const { return headerParameter2(); }

	// code file block
	[[nodiscard]] auto codeAddress() const { return headerParameter1(); }
	[[nodiscard]] auto codeLength() const { return dataBlockLength(); }

	[[nodiscard]] auto screenShotBlockType() const {
		return isCodeFileBlock()
			&& (codeAddress() == ScreenAddress)
			&& (codeLength() == ScreenLength);
	}

	void processBlock();	// Expects a header block
	void processBlock(const TAPFile& header, Board& board);	// Expects a data block
};
