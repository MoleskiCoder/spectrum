#pragma once

#include <SDL.h>

class Buzzer final {
public:
	Buzzer();
	~Buzzer();

	void initialise();
	void buzz(bool state, int cycle);
	void endFrame();

private:
	SDL_AudioSpec m_want;
	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device = 0;

	std::vector<bool> m_buffer;
	int m_lastSample = 0;
	bool m_lastState = false;

	int samplesPerFrame() const;
	int sample(int cycle) const;
};

