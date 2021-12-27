#pragma once

#include <cstdint>
#include <string>

#include "LittleEndianContent.h"
#include "Loader.h"

namespace EightBit {
	class Z80;
}

class SnapshotFile : public Loader {
private:
	LittleEndianContent m_content;

protected:
	SnapshotFile(std::string path);

	virtual void examineHeaders();
	virtual void loadRegisters(EightBit::Z80& cpu) const = 0;
	virtual void loadMemory(Board& board) const = 0;

	[[nodiscard]] const LittleEndianContent& content() const { return m_content; }
	[[nodiscard]] LittleEndianContent& content() { return m_content; }

	void read();

	[[nodiscard]] uint8_t peek(uint16_t offset) const;
	[[nodiscard]] uint16_t peekWord(uint16_t offset) const;

	[[nodiscard]] auto size() const { return m_content.size(); }

public:
	void load(Board& board) override;
};

