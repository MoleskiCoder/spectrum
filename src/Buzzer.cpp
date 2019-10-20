#include "stdafx.h"
#include "Buzzer.h"
#include "Computer.h"
#include "Ula.h"

#include <SDLWrapper.h>

#include <algorithm>

#include <iostream>

Buzzer::Buzzer() {

	SDL_zero(m_have);
	SDL_zero(m_want);

	m_want.freq = 44100;
	m_want.format = AUDIO_U8;
	m_want.channels = 1;
}

Buzzer::~Buzzer() {
	::SDL_CloseAudioDevice(m_device);
}

void Buzzer::initialise() {
	m_device = ::SDL_OpenAudioDevice(NULL, 0, &m_want, &m_have, 0);
	if (m_device == 0)
		Gaming::SDLWrapper::throwSDLException("Unable to open audio device");

	std::cout << "Sound frequency: " << m_have.freq << std::endl;
	std::cout << "Sound format: " << std::hex << m_have.format << std::dec << std::endl;
	std::cout << "Sound channels: " << (int)m_have.channels << std::endl;
	std::cout << "Sound samples: " << m_have.samples << std::endl;

	m_buffer.resize(samplesPerFrame() + 1);

	::SDL_PauseAudioDevice(m_device, false);
}

void Buzzer::buzz(bool state, int cycle) {
	const auto currentSample = sample(cycle);
	if (m_lastSample > currentSample)
		m_lastSample = 0;
	std::fill(m_buffer.begin() + m_lastSample, m_buffer.begin() + currentSample, m_lastState);
	m_lastSample = currentSample;
	m_lastState = state;
}

void Buzzer::endFrame() {

	assert(m_have.format == AUDIO_U8);
	assert(m_have.channels == 1);

	const auto length = m_buffer.size();

	std::vector<uint8_t> audio(length, 0);
	for (int i = 0; i < length; ++i) {
		const auto current = m_buffer.at(i);
		audio.at(i) = current ? 0xff : 0x00;
	}

	const int returned = ::SDL_QueueAudio(m_device, audio.data(), length * sizeof(uint8_t));
	Gaming::SDLWrapper::verifySDLCall(returned, "Unable to queue buzzer audio: ");

	std::fill(m_buffer.begin(), m_buffer.end(), false);
}

int Buzzer::samplesPerFrame() const {
	return m_have.freq / Ula::FramesPerSecond;
}

int Buzzer::sample(int cycle) const {
	const float ratio = (float)m_have.freq / (float)Ula::CyclesPerSecond;
	return (float)cycle * ratio;
}
