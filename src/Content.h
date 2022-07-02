#pragma once

#include <cassert>
#include <cstdint>
#include <span>
#include <vector>

#include <Chip.h>
#include <Rom.h>
#include <Register.h>


class Content : public EightBit::Rom {
private:
	uint16_t m_position = EightBit::Chip::Mask16;
	bool m_locked = false;

public:
	Content();
	Content(const Content& rhs);
	Content& operator=(const Content& rhs);
	bool operator==(const Content& rhs) const;

	[[nodiscard]] constexpr auto& position() noexcept { return m_position; }
	[[nodiscard]] constexpr auto position() const noexcept { return m_position; }
	[[nodiscard]] auto remaining() const noexcept { return size() - position(); }
	[[nodiscard]] auto finished() const noexcept { return remaining() <= 0; }

	constexpr void resetPosition() noexcept { position() = 0; }

	[[nodiscard]] constexpr auto& locked() noexcept { return m_locked; }
	[[nodiscard]] constexpr auto locked() const noexcept { return m_locked; }
	[[nodiscard]] constexpr auto unlocked() const noexcept { return !locked(); }
	[[nodiscard]] constexpr void lock(bool locking = true) noexcept { locked() = locking; }
	[[nodiscard]] constexpr void unlock() noexcept { lock(false); }

	constexpr void move(int amount = 1) noexcept {
		assert(!locked());
		position() += amount;
	}

	[[nodiscard]] constexpr auto readBytes(uint16_t position, uint16_t amount) noexcept {
		return std::span<uint8_t>(BYTES().data() + position, amount);
	}

	[[nodiscard]] constexpr auto fetchBytes(uint16_t amount) noexcept {
		const auto bytes = readBytes(position(), amount);
		move(amount);
		return bytes;
	}

	[[nodiscard]] constexpr auto readBytes() noexcept {
		return readBytes(0, size());
	}

	[[nodiscard]] constexpr auto readByte(uint16_t position) noexcept {
		const auto bytes = readBytes(position, 1);
		return bytes[0];
	}

	[[nodiscard]] constexpr auto fetchByte() noexcept {
		const auto bytes = fetchBytes(1);
		return bytes[0];
	}

	[[nodiscard]] virtual EightBit::register16_t readWord(uint16_t position) noexcept = 0;
	[[nodiscard]] std::vector<EightBit::register16_t> readWords(uint16_t position, uint16_t amount);

	[[nodiscard]] std::vector<EightBit::register16_t> fetchWords(uint16_t amount);
	[[nodiscard]] EightBit::register16_t fetchWord();
};
