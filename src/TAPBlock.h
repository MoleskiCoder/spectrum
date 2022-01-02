#pragma once

#include <cstdint>
#include <string>

#include <Rom.h>
#include <Register.h>

#include "LittleEndianContent.h"
#include "StandardToneSequence.h"

#if __cplusplus < 202002L
#	include <boost/coroutine2/all.hpp>
#endif

class TAPBlock final {
public:
	enum BlockFlag { Header = 0, Data = 0xff };

private:
	enum Type { ProgramBlock, NumberArrayBlock, CharacterArrayBlock, CodeFileBlock, UnknownBlock = 0xff };

	static const uint16_t ScreenAddress = 0x4000;
	static const int ScreenLength = 0x1b00;

	LittleEndianContent m_content;

	uint8_t m_flag = 128;	// Not a BlockFlag!

	// TAP header
	uint8_t m_blockType = Type::UnknownBlock;
	std::string m_headerFilename;
	EightBit::register16_t m_dataBlockLength = 0xffff;
	EightBit::register16_t m_headerParameter1 = 0xffff;
	EightBit::register16_t m_headerParameter2 = 0xffff;

	[[nodiscard]] constexpr auto& content() noexcept { return m_content; }

	void dumpHeaderInformation() const;
	void processHeader();

	void dumpDataInformation() const;
	void processData();

public:
	TAPBlock();
	TAPBlock(const LittleEndianContent& content);
	TAPBlock(const TAPBlock& rhs);
	TAPBlock& operator=(const TAPBlock& rhs);

	[[nodiscard]] constexpr const auto& content() const noexcept { return m_content; }

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
	[[nodiscard]] constexpr auto lineNumber() const noexcept { return headerParameter1(); }
	[[nodiscard]] constexpr auto variableArea() const noexcept { return headerParameter2(); }

	// code file block
	[[nodiscard]] constexpr auto codeAddress() const noexcept { return headerParameter1(); }
	[[nodiscard]] constexpr auto codeLength() const noexcept { return dataBlockLength(); }

	[[nodiscard]] constexpr auto screenShotBlockType() const noexcept {
		return isCodeFileBlock()
			&& (codeAddress() == ScreenAddress)
			&& (codeLength() == ScreenLength);
	}

	void process();

#if __cplusplus >= 202002L
	[[nodiscard]] auto generate() const { return StandardToneSequence().generate(*this); }
#else
	void generate(boost::coroutines2::coroutine<ToneSequence::pulse_t>::push_type& sink) const { StandardToneSequence().generate(*this, sink); }
#endif
};
