#pragma once

#include <cassert>
#include <algorithm>
#include <limits>
#include <vector>

#include <SDL3/SDL.h>

#include <Device.h>
#include <Wrapper.h>

#include "WavWriter.h"

// Monophonic ZX Spectrum buzzer emulation
template<typename T>
class Buzzer final {
private:
	static const int AudioFrequency = 44'100;
	static const T LowLevel = std::numeric_limits<T>::min();
	static const T HighLevel = std::numeric_limits<T>::max();

	WavWriter<T, float> m_wav = { "spectrum.wav", 1, AudioFrequency, LowLevel, HighLevel, -.1f, .1f };

	std::shared_ptr<SDL_AudioStream> m_stream;

	const float m_sampleLength;

	std::vector<T> m_buffer;
	Uint32 m_bufferLength = 0;
	int m_lastSample = 0;
	T m_lastLevel = LowLevel;

	[[nodiscard]] constexpr auto sampleLength() const noexcept { return m_sampleLength; }

	[[nodiscard]] constexpr auto sample(int cycle) const noexcept {
		const auto sample = static_cast<float>(cycle) * sampleLength();
		return static_cast<int>(sample);
	}

	constexpr void buzz(T level, int sample) {
		assert(sample >= m_lastSample);
		std::fill(m_buffer.begin() + m_lastSample, m_buffer.begin() + sample, m_lastLevel);
		m_lastSample = sample;
		m_lastLevel = level;
	}

public:
	Buzzer(float frameRate, int clockRate, SDL_AudioFormat format)
	: m_sampleLength(static_cast<float>(AudioFrequency) / static_cast<float>(clockRate)) {
	
		const auto samplesPerFrame = static_cast<float>(AudioFrequency) / frameRate + 1.0f;

		SDL_AudioSpec want;
		want.freq = AudioFrequency;
		want.format = format;
		want.channels = 1;
	
		m_stream.reset(::SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want, nullptr, nullptr), ::SDL_DestroyAudioStream);
		Gaming::Wrapper::maybeThrowException(m_stream.get(), "Unable to open audio device stream");

		m_buffer.resize((uint64_t)samplesPerFrame);
		m_bufferLength = static_cast<Uint32>(m_buffer.size() * sizeof(T));
	
		stop();
	}

	~Buzzer() noexcept {
		try {
			maybeStopRecording();
		} catch (...) {}
		m_stream.reset();
	}

	void stop() noexcept {
		const auto success = ::SDL_PauseAudioStreamDevice(m_stream.get());
		Gaming::Wrapper::maybeThrowException(success, "Unable to pause audio device stream");
	}
	
	void start() noexcept {
		const auto success = ::SDL_ResumeAudioStreamDevice(m_stream.get());
		Gaming::Wrapper::maybeThrowException(success, "Unable to resume audio device stream");
	}

	auto maybeStartRecording() {
		return m_wav.maybeStartRecording();
	}

	auto maybeStopRecording() {
		return m_wav.maybeStopRecording();
	}

	constexpr void buzz(EightBit::Device::PinLevel state, int cycle) {
		const T level = EightBit::Device::raised(state) ? HighLevel : LowLevel;
		buzz(level, sample(cycle));
	}

	void endFrame() {
		std::fill(m_buffer.begin() + m_lastSample, m_buffer.end(), m_lastLevel);
		const auto success = SDL_PutAudioStreamData(m_stream.get(), m_buffer.data(), m_bufferLength);
		Gaming::Wrapper::maybeThrowException(success, "Unable to put buzzer audio");
		m_wav.maybeRecordSamples(m_buffer.begin(), m_buffer.end());
		m_lastSample = 0;
	}
};
