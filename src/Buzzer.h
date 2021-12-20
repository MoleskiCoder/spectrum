#pragma once

#include <cassert>
#include <algorithm>
#include <limits>
#include <vector>

#include <SDL.h>

#include <Device.h>
#include <SDLWrapper.h>

#include "AudioFile.h"

// Monophonic ZX Spectrum buzzer emulation
template<typename T>
class Buzzer final {
private:
	static const int AudioFrequency = 44'100;
	static const T LowLevel = std::numeric_limits<T>::min();
	static const T HighLevel = std::numeric_limits<T>::max();

	AudioFile<float> m_wav;

	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device = 0;

	const float m_sampleLength;

	std::vector<T> m_buffer;
	Uint32 m_bufferLength = 0;
	int m_lastSample = 0;
	T m_lastLevel = LowLevel;
	bool m_recording = false;

	[[nodiscard]] constexpr auto sampleLength() const noexcept { return m_sampleLength; }
	[[nodiscard]] constexpr auto recording() const noexcept { return m_recording; }

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

	void recordSamples() {
		for (auto sample : m_buffer) {
			const auto low = sample == LowLevel;
			const auto high = sample == HighLevel;
			assert(low || high);
			for (int channel = 0; channel < m_have.channels; ++channel)
				m_wav.samples[channel].push_back(low ? -.1f : .1f);
		}
	}

	auto maybeRecordSamples() {
		if (recording())
			recordSamples();
		return recording();
	}

public:
	Buzzer(float frameRate, int clockRate, SDL_AudioFormat format)
	: m_sampleLength(static_cast<float>(AudioFrequency) / static_cast<float>(clockRate)) {
	
		const auto samplesPerFrame = static_cast<float>(AudioFrequency) / frameRate + 1.0f;

		SDL_AudioSpec want;
		SDL_zero(want);
		want.freq = AudioFrequency;
		want.format = format;
		want.channels = 1;
		want.samples = static_cast<Uint16>(samplesPerFrame / want.channels);
	
		SDL_zero(m_have);
		m_device = ::SDL_OpenAudioDevice(nullptr, SDL_FALSE, &want, &m_have, 0);
		if (m_device == 0)
			Gaming::SDLWrapper::throwSDLException("Unable to open audio device");
		assert(m_device >= 2);

		// Given that there are no allowed changes, problems should have led to an
		// exception being thrown.  So the following must all be correct.
		assert(m_have.freq == want.freq);
		assert(m_have.format == want.format);
		assert(m_have.channels == want.channels);
		assert(m_have.samples == want.samples);
	
		m_buffer.resize(m_have.samples);
		m_bufferLength = static_cast<Uint32>(m_buffer.size() * sizeof(T));
	
		stop();
	}

	~Buzzer() noexcept {
		try {
			maybeStopRecording();
		} catch (...) {}
		::SDL_CloseAudioDevice(m_device);
	}

	void stop() noexcept { ::SDL_PauseAudioDevice(m_device, SDL_TRUE); }
	void start() noexcept {	::SDL_PauseAudioDevice(m_device, SDL_FALSE); }

	void startRecording() {
		assert(!recording());
		m_wav.samples.clear();
		m_wav.setNumChannels(m_have.channels);
		m_wav.setNumSamplesPerChannel(AudioFrequency);
		m_recording = true;
	}

	void stopRecording() {
		assert(recording());
		m_wav.save("spectrum.wav", AudioFileFormat::Wave);
		m_wav.samples.clear();
		m_recording = false;
	}

	auto maybeStartRecording() {
		const auto starting = !recording();
		if (starting)
			startRecording();
		return starting;
	}
	
	auto maybeStopRecording() {
		const auto stopping = recording();
		if (stopping)
			stopRecording();
		return stopping;
	}

	constexpr void buzz(EightBit::Device::PinLevel state, int cycle) {
		const T level = EightBit::Device::raised(state) ? HighLevel : LowLevel;
		buzz(level, sample(cycle));
	}

	void endFrame() {
		std::fill(m_buffer.begin() + m_lastSample, m_buffer.end(), m_lastLevel);
		const int returned = ::SDL_QueueAudio(m_device, m_buffer.data(), m_bufferLength);
		Gaming::SDLWrapper::verifySDLCall(returned, "Unable to queue buzzer audio: ");
		maybeRecordSamples();
		m_lastSample = 0;
	}
};
