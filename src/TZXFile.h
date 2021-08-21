#pragma once

#include <memory>
#include <string>

#include <Rom.h>
#include <Register.h>

#include "Loader.h"
#include "DataLoader.h"
#include "TAPFile.h"

class TZXFile final : public Loader {
private:
	static const uint16_t ScreenAddress = 0x4000;
	static const int ScreenLength = 0x1b00;

	TAPFile::BlockFlag m_expecting = TAPFile::BlockFlag::Header;
	[[nodiscard]] auto expecting() const { return m_expecting; }
	[[nodiscard]] auto expectingHeader() const { return expecting() == TAPFile::BlockFlag::Header; }
	[[nodiscard]] auto expectingData() const { return !expectingHeader(); }

	std::unique_ptr<TAPFile> m_headerTAP;
	[[nodiscard]] auto& headerTAP() { return *m_headerTAP; }

	EightBit::Rom m_contents;
	auto& contents() { return m_contents; }

	DataLoader m_loader = { m_contents };
	auto& loader() { return m_loader; }

	void readHeader();
	void readBlock(Board& board);

	void readStandardSpeedDataBlock(Board& board);

public:
	TZXFile(std::string path);

	void load(Board& board) override;
};
