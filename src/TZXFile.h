#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <co_generator_t.h>

#include "LittleEndianContent.h"
#include "TAPBlock.h"

class TZXFile final {
public:
	typedef std::vector<TAPBlock> blocks_t;
	typedef EightBit::co_generator_t<ToneSequence::amplitude_t> amplitude_generator_t;

private:
	static const uint16_t ScreenAddress = 0x4000;
	static const int ScreenLength = 0x1b00;

	std::string m_path;
	LittleEndianContent m_content;
	blocks_t m_blocks;
	bool m_playing = false;

	[[nodiscard]] auto path() const { return m_path; }
	[[nodiscard]] constexpr auto& content() noexcept { return m_content; }

	[[nodiscard]] constexpr auto& blocks() noexcept { return m_blocks; }

	void readHeader();

	[[nodiscard]] TAPBlock readBlock();

	[[nodiscard]] TAPBlock readStandardSpeedDataBlock();

public:
	TZXFile();

	void load(std::string path);

	[[nodiscard]] constexpr const auto& blocks() const noexcept { return m_blocks; }
	[[nodiscard]] constexpr auto unloaded() const noexcept { return blocks().empty(); }
	[[nodiscard]] constexpr auto loaded() const noexcept { return !unloaded(); }

	[[nodiscard]] amplitude_generator_t generate() const;

	[[nodiscard]] constexpr auto playing() const noexcept { return m_playing; }
	[[nodiscard]] constexpr auto stopped() const noexcept { return !playing(); }
	[[nodiscard]] constexpr auto& playing() noexcept { return m_playing; }
	constexpr void play(bool state = true) noexcept { playing() = state; }
	constexpr void stop() noexcept { play(false); }
};
