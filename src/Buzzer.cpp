#include "stdafx.h"
#include "Buzzer.h"

#include <SDLWrapper.h>

#include <algorithm>

Buzzer::Buzzer(float frameRate, int clockRate)
: m_clockRate(clockRate),
  m_sampleLength(static_cast<float>(AudioFrequency) / static_cast<float>(clockRate))
{
	const auto samplesPerFrame = static_cast<int>(AudioFrequency / frameRate + 1);
		
	SDL_AudioSpec want;
	SDL_zero(want);
	want.freq = AudioFrequency;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.samples = samplesPerFrame;

	SDL_zero(m_have);

	m_device = ::SDL_OpenAudioDevice(nullptr, SDL_FALSE, &want, &m_have, 0);
	if (m_device == 0)
		Gaming::SDLWrapper::throwSDLException("Unable to open audio device");

	assert(m_have.freq == AudioFrequency);
	assert(m_have.format == AUDIO_U8);
	assert(m_have.channels == 1);

	m_buffer.resize(samplesPerFrame);
	m_bufferLength = static_cast<Uint32>(m_buffer.size() * sizeof(Uint8));

	stop();
}

Buzzer::~Buzzer() noexcept {
	::SDL_CloseAudioDevice(m_device);
}

void Buzzer::stop() noexcept {
	::SDL_PauseAudioDevice(m_device, SDL_TRUE);
}

void Buzzer::start() noexcept {
	::SDL_PauseAudioDevice(m_device, SDL_FALSE);
}

void Buzzer::buzz(EightBit::Device::PinLevel state, int cycle) {
	const Uint8 level = EightBit::Device::raised(state) ? 0xff : 0x00;
	buzz(level, sample(cycle));
}

void Buzzer::buzz(Uint8 level, int sample) {
	assert(sample >= m_lastSample);
	std::fill(m_buffer.begin() + m_lastSample, m_buffer.begin() + sample, m_lastLevel);
	m_lastSample = sample;
	m_lastLevel = level;
}

void Buzzer::endFrame() {
	std::fill(m_buffer.begin() + m_lastSample, m_buffer.end(), m_lastLevel);
	const int returned = ::SDL_QueueAudio(m_device, m_buffer.data(), m_bufferLength);
	Gaming::SDLWrapper::verifySDLCall(returned, "Unable to queue buzzer audio: ");
	m_lastSample = 0;
}

int Buzzer::sample(int cycle) const noexcept {
	const auto sample = static_cast<float>(cycle) * sampleLength();
	return static_cast<int>(sample);
}
