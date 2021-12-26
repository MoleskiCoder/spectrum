#pragma once

#include <cstdint>
#include <vector>
#include <span>

#include <Rom.h>
#include <Register.h>

class Content : public EightBit::Rom {
private:
	int m_position = -1;
	bool m_locked = false;

public:
	Content();
	Content(const Content& rhs);
	Content& operator=(const Content& rhs);

	[[nodiscard]] constexpr auto& position() noexcept { return m_position; }
	[[nodiscard]] constexpr auto position() const noexcept { return m_position; }
	[[nodiscard]] auto remaining() const noexcept { return size() - position(); }
	[[nodiscard]] auto finished() const noexcept { return remaining() <= 0; }

	constexpr void resetPosition() noexcept { position() = 0; }

	[[nodiscard]] constexpr auto& locked() noexcept { return m_locked; }
	[[nodiscard]] constexpr auto locked() const noexcept { return m_locked; }
	[[nodiscard]] constexpr auto unlocked() const noexcept { return !locked(); }
	[[nodiscard]] constexpr void lock(bool locking = true) noexcept { locked() = locking; }
	[[nodiscard]] constexpr void unlock() noexcept { lock(true); }

	void move(int amount = 1);

	[[nodiscard]] uint8_t readByte(int position);
	[[nodiscard]] std::span<uint8_t> readBytes(int position, int amount);

	[[nodiscard]] std::span<uint8_t> fetchBytes(int amount);
	[[nodiscard]] uint8_t fetchByte();

	[[nodiscard]] virtual EightBit::register16_t readWord(int position) = 0;
	[[nodiscard]] std::vector<EightBit::register16_t> readWords(int position, int amount);

	[[nodiscard]] std::vector<EightBit::register16_t> fetchWords(int amount);
	[[nodiscard]] EightBit::register16_t fetchWord();
};
