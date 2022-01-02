#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Register.h>

#include "LittleEndianContent.h"
#include "TAPBlock.h"

#if __cplusplus >= 202002L
#   include <co_generator_t.h>
#else
#	include <boost/coroutine2/all.hpp>
#endif

class TZXFile final {
public:
	typedef std::vector<TAPBlock> blocks_t;

#if __cplusplus >= 202002L
	typedef EightBit::co_generator_t<ToneSequence::amplitude_t> amplitude_generator_t;
#else
	typedef boost::coroutines2::coroutine<ToneSequence::amplitude_t>::push_type amplitude_push_t;
	typedef boost::coroutines2::coroutine<ToneSequence::amplitude_t>::pull_type amplitude_pull_t;
#endif

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

#if __cplusplus >= 202002L
	[[nodiscard]] constexpr auto unloaded() const noexcept { return blocks().empty(); }
	[[nodiscard]] constexpr auto loaded() const noexcept { return !unloaded(); }
#else
	[[nodiscard]] auto unloaded() const noexcept { return blocks().empty(); }
	[[nodiscard]] auto loaded() const noexcept { return !unloaded(); }
#endif

#if __cplusplus >= 202002L
	[[nodiscard]] amplitude_generator_t generate() const;
#else
	void generate(amplitude_push_t& sink) const;
#endif

	[[nodiscard]] constexpr auto playing() const noexcept { return m_playing; }
	[[nodiscard]] constexpr auto stopped() const noexcept { return !playing(); }
	[[nodiscard]] constexpr auto& playing() noexcept { return m_playing; }
	constexpr void play(bool state = true) noexcept { playing() = state; }
	constexpr void stop() noexcept { play(false); }
};
