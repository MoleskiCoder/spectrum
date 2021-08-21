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
	[[nodiscard]] constexpr auto expecting() const noexcept { return m_expecting; }
	[[nodiscard]] constexpr auto expectingHeader() const noexcept { return expecting() == TAPFile::BlockFlag::Header; }
	[[nodiscard]] constexpr auto expectingData() const noexcept { return !expectingHeader(); }

	std::unique_ptr<TAPFile> m_headerTAP;
	[[nodiscard]] auto& headerTAP() noexcept { return *m_headerTAP; }

	EightBit::Rom m_contents;
	constexpr auto& contents() noexcept { return m_contents; }

	DataLoader m_loader = { m_contents };
	constexpr auto& loader() noexcept { return m_loader; }

	void readHeader();
	void readBlock(Board& board);

	void readStandardSpeedDataBlock(Board& board);

public:
	TZXFile(std::string path);

	void load(Board& board) override;
};
