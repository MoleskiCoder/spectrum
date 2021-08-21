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

	[[nodiscard]] constexpr const auto& contents() const noexcept { return m_contents; }

	[[nodiscard]] constexpr auto& position() noexcept { return m_position; }
	[[nodiscard]] constexpr auto position() const noexcept { return m_position; }
	[[nodiscard]] auto remaining() const noexcept { return contents().size() - position(); }
	[[nodiscard]] auto finished() const noexcept { return remaining() <= 0; }

	constexpr void resetPosition() noexcept { m_position = 0; }

	[[nodiscard]] constexpr auto& locked() noexcept { return m_locked; }
	[[nodiscard]] constexpr auto locked() const noexcept { return m_locked; }
	[[nodiscard]] constexpr auto unlocked() const noexcept { return !locked(); }
	[[nodiscard]] constexpr void lock(bool locking = true) noexcept { locked() = locking; }
	[[nodiscard]] constexpr void unlock() noexcept { lock(true); }

	void move(int amount = 1);

	[[nodiscard]] uint8_t readByte(int position) const;
	[[nodiscard]] std::vector<uint8_t> readBytes(int position, int amount) const;

	[[nodiscard]] std::vector<uint8_t> fetchBytes(int amount);
	[[nodiscard]] uint8_t fetchByte();

	[[nodiscard]] EightBit::register16_t readWord(int position) const;
	[[nodiscard]] std::vector<EightBit::register16_t> readWords(int position, int amount) const;

	[[nodiscard]] std::vector<EightBit::register16_t> fetchWords(int amount);
	[[nodiscard]] EightBit::register16_t fetchWord();
};