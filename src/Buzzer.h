#pragma once

#include "AbstractBuzzer.h"

#include <memory>

#include <SDL3/SDL.h>

#include <Wrapper.h>

// SDL3 ZX Spectrum buzzer implementation (concrete derived class)
class Buzzer final : public AbstractBuzzer<Uint8> {

	using base = AbstractBuzzer<Uint8>;

private:
	static const int AudioFrequency = 44'100;

	SDL_AudioFormat m_format;

	std::shared_ptr<SDL_AudioStream> m_stream;

protected:
	void playBuffer() override {
		clear();    // Avoid audio "drift"
		const auto success = SDL_PutAudioStreamData(m_stream.get(), m_buffer.data(), (int)m_buffer.size());
		Gaming::Wrapper::maybeThrowException(success, "Unable to put buzzer audio");
	}

	void flush() override {
		const auto success = SDL_FlushAudioStream(m_stream.get());
		Gaming::Wrapper::maybeThrowException(success, "Unable to flush audio data");
	}

	void clear() override {
		const auto remaining = SDL_GetAudioStreamAvailable(m_stream.get());
		Gaming::Wrapper::maybeThrowException(remaining != -1, "Unable to find how many audio stream bytes are available");
		if (remaining > 0) {
			SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "Clearing {%d} bytes of left over audio data", remaining);
			const auto success = SDL_ClearAudioStream(m_stream.get());
			Gaming::Wrapper::maybeThrowException(success, "Unable to clear audio data");
		}
	}

	void stop() override {
		const auto success = ::SDL_PauseAudioStreamDevice(m_stream.get());
		Gaming::Wrapper::maybeThrowException(success, "Unable to pause audio device stream");
	}

	void start() override {
		const auto success = ::SDL_ResumeAudioStreamDevice(m_stream.get());
		Gaming::Wrapper::maybeThrowException(success, "Unable to resume audio device stream");
	}

public:
	Buzzer(SDL_AudioFormat format = SDL_AUDIO_U8)
	: base(AudioFrequency),
	  m_format(format)
	{}

	void initialise() override {

		SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Audio frequency: {%d}", AudioFrequency);
		SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "CPU Clock rate: {%d}", m_clockRate);
		SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Sample length: {%f}", sampleLength());
		SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Cycles per sample: {%f}", cyclesPerSample());

		const auto samplesPerFrame = static_cast<float>(AudioFrequency) / m_frameRate + 1.0f;
		
		SDL_AudioSpec want;
		want.freq = AudioFrequency;
		want.format = m_format;
		want.channels = 1;
		
		m_stream.reset(::SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want, nullptr, nullptr), ::SDL_DestroyAudioStream);
		Gaming::Wrapper::maybeThrowException(m_stream.get(), "Unable to open audio device stream");
		
		m_buffer.resize((uint64_t)base::samplesPerFrame());
		
		SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Samples per frame: {%f}", base::samplesPerFrame());
		SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Samples per frame (cast): {%ld}", (long)base::samplesPerFrame());

		start();
	}

	void terminate() override {
		stop();
		m_stream.reset();
	}
};
