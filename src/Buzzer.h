#pragma once

#include <vector>

#include <SDL.h>

#include <Device.h>

class Buzzer final {
public:
	Buzzer();
	~Buzzer();

	void initialise();
	void buzz(EightBit::Device::PinLevel state, int cycle);
	void endFrame();

private:
	SDL_AudioSpec m_want;
	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device = 0;

	std::vector<Uint8> m_buffer;
	int m_lastSample = 0;
	Uint8 m_lastState = 0;

	int samplesPerFrame() const;
	int sample(int cycle) const;

	void buzz(Uint8 value, int sample);
};

