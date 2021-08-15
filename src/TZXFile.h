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
	enum Type { Program, NumberArray, CharacterArray, CodeFile };

	EightBit::Rom m_contents;
	DataLoader m_loader = { m_contents };

	auto& contents() { return m_contents; }
	auto& loader() { return m_loader; }

	void processTapHeader(DataLoader& loader);
	void processTapData(DataLoader& loader);
	void processTapBlock(const EightBit::Rom& data);

	void readHeader();
	void readBlock();

	void readStandardSpeedDataBlock();

public:
	TZXFile(std::string path);

	void load(Board& board) override;
};
