#pragma once

#include <cstdint>
#include <vector>

#include <Rom.h>
#include <Register.h>

class DataLoader final {
private:
	const EightBit::Rom& m_contents;
	int m_position = -1;
	bool m_locked = false;

public:
	DataLoader(const EightBit::Rom& rom);

	const auto& contents() const { return m_contents; }

	auto& position() { return m_position; }
	auto position() const { return m_position; }
	auto remaining() const { return contents().size() - position(); }
	auto finished() const { return remaining() <= 0; }

	void resetPosition() { m_position = 0; }

	[[nodiscard]] auto& locked() noexcept { return m_locked; }
	[[nodiscard]] auto locked() const noexcept { return m_locked; }
	[[nodiscard]] auto unlocked() const noexcept { return !locked(); }
	[[nodiscard]] void lock(bool locking = true) noexcept { locked() = locking; }
	[[nodiscard]] void unlock() noexcept { lock(true); }

	void move(int amount = 1);

	uint8_t readByte(int position) const;
	std::vector<uint8_t> readBytes(int position, int amount) const;

	std::vector<uint8_t> fetchBytes(int amount);
	uint8_t fetchByte();

	EightBit::register16_t readWord(int position) const;
	std::vector<EightBit::register16_t> readWords(int position, int amount) const;

	std::vector<EightBit::register16_t> fetchWords(int amount);
	EightBit::register16_t fetchWord();
};