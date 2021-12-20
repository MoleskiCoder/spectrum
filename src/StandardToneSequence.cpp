#include "stdafx.h"
#include "StandardToneSequence.h"

StandardToneSequence::StandardToneSequence() {
	pauseTime() = PauseTime;
	oneBitTonePulseLength() = OneBitTonePulseLength;
	zeroBitTonePulseLength() = ZeroBitTonePulseLength;
	pilotTonePulseLength() = PilotTonePulseLength;
	headerPilotTonePulses() = HeaderPilotTonePulses;
	dataPilotTonePulses() = DataPilotTonePulses;
	firstSyncTonePulseLength() = FirstSyncTonePulseLength;
	secondSyncTonePulseLength() = SecondSyncTonePulseLength;
}