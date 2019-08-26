#pragma once

#include <SDL.h>

class Buzzer final {
public:
	Buzzer();
	~Buzzer();

	void initialise();

	void on(int cycle);
	void off(int cycle);

	void buzz(bool state, int cycle) {
		state ? on(cycle) : off(cycle);
	}

private:
	void backFill(int cycle);

	int convertCycle2Sample(int cycle);

	SDL_AudioSpec m_want;
	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device = 0;

	int m_sample = 0;	// cycle is input position (from ULA)
	int m_position = 0;	// position is output position (to SDL audio)
	bool m_ready = false;	// The buffer has a valid set of samples
	Sint8 m_last = 0;
};

