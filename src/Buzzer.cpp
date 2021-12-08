#include "stdafx.h"
#include "Buzzer.h"

#include <SDLWrapper.h>

#include <algorithm>

Buzzer::Buzzer(float frameRate, int clockRate)
: m_clockRate(clockRate),
  m_sampleLength(static_cast<float>(AudioFrequency) / static_cast<float>(clockRate)) {

	SDL_AudioSpec want;
	SDL_zero(want);
	want.freq = AudioFrequency;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.samples = AudioFrequency / frameRate + 1;

	SDL_zero(m_have);
	m_device = ::SDL_OpenAudioDevice(nullptr, SDL_FALSE, &want, &m_have, 0);
	if (m_device == 0)
		Gaming::SDLWrapper::throwSDLException("Unable to open audio device");

	assert(m_have.freq == want.freq);
	assert(m_have.format == want.format);
	assert(m_have.channels == want.channels);
	assert(m_have.samples == want.samples);

	m_buffer.resize(m_have.samples);
	m_bufferLength = static_cast<Uint32>(m_buffer.size() * sizeof(Uint8));

	stop();
}

Buzzer::~Buzzer() noexcept {
	maybeStopRecording();
	::SDL_CloseAudioDevice(m_device);
}

void Buzzer::stop() noexcept {
	::SDL_PauseAudioDevice(m_device, SDL_TRUE);
}

void Buzzer::start() noexcept {
	::SDL_PauseAudioDevice(m_device, SDL_FALSE);
}

bool Buzzer::maybeStartRecording() {
	return m_wav.maybeOpen();
}

bool Buzzer::maybeStopRecording() {
	return m_wav.maybeClose();
}

void Buzzer::buzz(EightBit::Device::PinLevel state, int cycle) {
	const Uint8 level = EightBit::Device::raised(state) ? 0x7f : 0x00;
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
	if (m_wav.started())
		m_wav.write(m_buffer.begin(), m_buffer.end());
	m_lastSample = 0;
}

int Buzzer::sample(int cycle) const noexcept {
	const auto sample = static_cast<float>(cycle) * sampleLength();
	return static_cast<int>(sample);
}
