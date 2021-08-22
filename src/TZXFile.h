#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Rom.h>
#include <Register.h>

#include "DataLoader.h"
#include "TAPBlock.h"

class TZXFile final {
private:
	static const uint16_t ScreenAddress = 0x4000;
	static const int ScreenLength = 0x1b00;

	const std::string m_path;
	EightBit::Rom m_contents;
	DataLoader m_loader;

	[[nodiscard]] auto path() const { return m_path; }
	[[nodiscard]] constexpr auto& contents() noexcept { return m_contents; }
	[[nodiscard]] constexpr auto& loader() noexcept { return m_loader; }

	void readHeader();

	[[nodiscard]] TAPBlock readBlock();

	[[nodiscard]] TAPBlock readStandardSpeedDataBlock();

public:
	TZXFile(std::string path);

	[[nodiscard]] std::vector<TAPBlock> load();
};
