#pragma once

#include "ToneSequence.h"

class StandardToneSequence final : public ToneSequence {
private:
	static const int PilotTonePulseLength = 2168;	// T states
	static const int HeaderPilotTonePulses = 8063;	// Pulses
	static const int DataPilotTonePulses = 3223;	// Pulses

	static const int FirstSyncTonePulseLength = 667;	// T states
	static const int SecondSyncTonePulseLength = 735;	// T states

	static const int ZeroBitTonePulseLength = 855;	// T states
	static const int OneBitTonePulseLength = 1710;	// T states

	static const int PostBlockPause = 1000;	// ms

	static const int PauseTime = 3'500'000; // One second in T-states is the clock speed of the CPU!

public:
	StandardToneSequence();
};

