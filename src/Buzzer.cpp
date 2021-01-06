#include "stdafx.h"
#include "Buzzer.h"
#include "Ula.h"

#include <SDLWrapper.h>

#include <algorithm>

Buzzer::Buzzer() {

	SDL_zero(m_have);
	SDL_zero(m_want);

	m_want.freq = 44100;
	m_want.format = AUDIO_U8;
	m_want.channels = 1;

	m_device = ::SDL_OpenAudioDevice(nullptr, SDL_FALSE, &m_want, &m_have, 0);
	if (m_device == 0)
		Gaming::SDLWrapper::throwSDLException("Unable to open audio device");

	assert(m_have.format == AUDIO_U8);
	assert(m_have.channels == 1);

	m_buffer.resize(samplesPerFrame());
	m_bufferLength = static_cast<Uint32>(m_buffer.size() * sizeof(Uint8));

	stop();
}

Buzzer::~Buzzer() {
	::SDL_CloseAudioDevice(m_device);
}

void Buzzer::stop() {
	::SDL_PauseAudioDevice(m_device, SDL_TRUE);
}

void Buzzer::start() {
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

int Buzzer::samplesPerFrame() const {
	return static_cast<int>(m_have.freq / Ula::FramesPerSecond + 1);
}

int Buzzer::sample(int cycle) const {
	const float ratio = static_cast<float>(m_have.freq) / static_cast<float>(Ula::ClockRate);
	const auto sample = static_cast<float>(cycle) * ratio;
	return static_cast<int>(sample);
}
