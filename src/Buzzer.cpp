#include "stdafx.h"
#include "Buzzer.h"
#include "Computer.h"
#include "Ula.h"

#include <algorithm>

#include <iostream>


Buzzer::Buzzer() {

	SDL_zero(m_have);
	SDL_zero(m_want);

	m_want.freq = 44100;
	m_want.format = AUDIO_S8;
	m_want.channels = 1;
	m_want.samples = 4096;
}

Buzzer::~Buzzer() {
	::SDL_CloseAudioDevice(m_device);
}

void Buzzer::initialise() {
	m_device = ::SDL_OpenAudioDevice(NULL, 0, &m_want, &m_have, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (m_device == 0)
		Gaming::SDLWrapper::throwSDLException("Unable to open audio device");
	::SDL_PauseAudioDevice(m_device, false);
}

void Buzzer::on(const int cycle) {
	backFill(cycle);
	m_last = 127;
}

void Buzzer::off(const int cycle) {
	backFill(cycle);
	m_last = -128;
}

int Buzzer::convertCycle2Sample(int cycle) {
	const float sampleFrequency = (float)m_have.freq;
	const float cycleFrequency = Ula::CyclesPerSecond;
	const auto cyclesPerSample = cycleFrequency / sampleFrequency;
	return (int)((float)cycle / cyclesPerSample);
}

void Buzzer::backFill(const int cycle) {

	const int currentSample = convertCycle2Sample(cycle);
	const int previousSample = m_sample;

	const int length = currentSample - previousSample;
	// XXXX ???
	if (length < 0)
		return;
	assert(length > 0);

	std::vector<Sint8> m_buffer(length, m_last);	// Signed eight bit samples

	Gaming::SDLWrapper::verifySDLCall(::SDL_QueueAudio(m_device, m_buffer.data(), length), "xxxx");

	m_sample = previousSample;
}
