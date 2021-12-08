#pragma once

#include <vector>

#include <SDL.h>

#include <Device.h>

#include "WavWriter.h"

class Buzzer final {
public:
	Buzzer(float frameRate, int clockRate);
	~Buzzer() noexcept;

	void stop() noexcept;
	void start() noexcept;

	bool maybeStartRecording();
	bool maybeStopRecording();

	void buzz(EightBit::Device::PinLevel state, int cycle);
	void endFrame();

private:
	static const int AudioFrequency = 44100;

	WavWriter<Uint8> m_wav = { "spectrum.wav", 1, AudioFrequency };

	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device = 0;

	const int m_clockRate;
	const float m_sampleLength;

	std::vector<Uint8> m_buffer;
	Uint32 m_bufferLength = 0;
	int m_lastSample = 0;
	Uint8 m_lastLevel = 0;

	[[nodiscard]] constexpr auto clockRate() const noexcept { return m_clockRate; }
	[[nodiscard]] constexpr auto sampleLength() const noexcept { return m_sampleLength; }

	[[nodiscard]] int sample(int cycle) const noexcept;

	void buzz(Uint8 level, int sample);
};

