#include "stdafx.h"

#include <memory>
#include <chrono>

#include "Configuration.h"
#include "Computer.h"
#include "KempstonJoystick.h"
#include "Interface2Joystick.h"

void loadROM(const Configuration& configuration, Computer& computer) {

	auto romDirectory = configuration.getRomDirectory();

	//computer.plug(romDirectory + "\\G12R_ROM.bin");	// Planetoids (Asteroids)
	//computer.plug(romDirectory + "\\G24R_ROM.bin");	// Horace and the Spiders
	//computer.plug(romDirectory + "\\G9R_ROM.bin");	// Space Raiders (Space Invaders)
	//computer.plug(romDirectory + "\\Jet Pac (1983)(Sinclair Research)(GB).rom");	// JetPac
	//computer.plug(romDirectory + "\\Pssst (1983)(Sinclair Research)(GB).rom"); // Pssst
	//computer.plug(romDirectory + "\\Knight Lore (1984)(Ultimate Play The Game)(GB)[b].rom"); // Knight Lore, 32k ROM??

	//computer.plug(romDirectory + "\\System_Test_ROM.bin");	// Sinclair test ROM by Dr. Ian Logan
	//computer.plug(romDirectory + "\\Release-v0.37\\testrom.bin");
	//computer.plug(romDirectory + "\\smart\\ROMs\\DiagROM.v41");
	//computer.plug(romDirectory + "\\DiagROM.v56");
	//computer.plug(romDirectory + "\\VMM-TEST.ROM");
}

void loadProgram(const Configuration& configuration, Computer& computer) {

	auto programDirectory = configuration.getProgramDirectory();

	//computer.loadZ80(programDirectory + "\\Alien 8 (1985)(Ultimate).z80");	// Works
	//computer.loadSna(programDirectory + "\\ant_attack.sna");	// 3D ant attack, works

	//computer.loadZ80(programDirectory + "\\Ballblazer (1985)(Activision).z80");	// Not 48k spectrum

	//computer.loadZ80(programDirectory + "\\Boulder Dash (1984)(First Star Software).z80");	// Works

	//computer.loadZ80(programDirectory + "\\Cobra (1986)(Ocean Software).z80");	// Shows title screen, then crashes
	//computer.loadZ80(programDirectory + "\\Cobra (1986)(Ocean Software)[a2].z80");	// Shows menu screen, then crashes

	//computer.loadSna(programDirectory + "\\ELITE.SNA");	// freeze
	//computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird).z80");	// spins for a second, then freezes
	//computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird)[a].z80");	// z80 v2
	//computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird)[a2].z80");	// flashes, then freeze/garbage
	//computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird)[a3].z80");	// freeze

	//computer.loadZ80(programDirectory + "\\Head Over Heels (1987)(Ocean Software).z80");	// v2, freeze

	//computer.loadZ80(programDirectory + "\\HELCHOP.Z80");
	//computer.loadZ80(programDirectory + "\\Helichopper_1985_Firebird_Software.z80");
	//computer.loadZ80(programDirectory + "\\Helichopper_1985_Firebird_Software_a.z80");
	//computer.loadZ80(programDirectory + "\\HALLSTHI.z80");

	//computer.loadZ80(programDirectory + "\\Jetpac_1983_Ultimate_Play_The_Game_16K.z80");	// Doesn't exist
	//computer.loadZ80(programDirectory + "\\Jetpac (1983)(Ultimate Play The Game)[a][16K].z80");
	//computer.loadTZX(programDirectory + "\\jetpac.tzx");
	//computer.loadZ80(programDirectory + "\\JetPac.z80");
	//computer.loadSna(programDirectory + "\\JetPac.sna");

	//computer.loadZ80(programDirectory + "\\Knight Lore (1984)(Ultimate).z80");	// Works

	//computer.loadTZX(programDirectory + "\\Manic Miner.tzx");

	//computer.loadZ80(programDirectory + "\\R-Type (1988)(Activision).z80");		// v3, not for 48K spectrum
	//computer.loadZ80(programDirectory + "\\Rommels_Revenge_1983_Crystal_Computing.z80");	// Goes through menus, crashes in game

	//computer.loadSna(programDirectory + "\\zexall.sna");


	// Utilities
	//computer.loadZ80(programDirectory + "\\Spectrum Musicmaker (1983)(Robert Newman).z80");	// Works
	//computer.loadZ80(programDirectory + "\\Spectrum Sound FX (1983)(Marolli Soft).z80");	// v3
	//computer.loadZ80(programDirectory + "\\Sounds 2 (19xx)(The Champ).z80");	// Not for 48k Spectrums
	//computer.loadZ80(programDirectory + "\\Sound Demo 01 (1992)(Theo Devil).z80");	// Not for 48k Spectrums
	//computer.loadZ80(programDirectory + "\\iotbs2.z80");	// Not for 48k Spectrums

	// Demos

	//computer.loadZ80(programDirectory + "\\TFF4.Z80");
	//computer.loadZ80(programDirectory + "\\BABY.Z80");	// Plays sampled "I don't care"
	//computer.loadZ80(programDirectory + "\\ATARI2.Z80");	// hangs
	//computer.loadZ80(programDirectory + "\\HEDGEHOG.Z80");	// Not for 48K spectrum
}

void testTapeLoading(const Configuration& configuration) {

	auto programDirectory = configuration.getProgramDirectory();

	const auto start_time = std::chrono::steady_clock::now();
	auto count = 0ULL;

	TZXFile tape;
	tape.load(programDirectory + "\\Manic Miner.tzx");

	ToneSequence::amplitude_t highest = ToneSequence::amplitude_t::Low;

	auto generator = tape.generate();
	while (generator) {
		const auto level = generator();
		++count;
	}

	//Buzzer<Uint8> beep = { Ula::FramesPerSecond, Ula::CpuClockRate, AUDIO_U8 };
	//auto generator = tape.generate();
	//auto frameCycles = 0;
	//while (generator) {
	//	const auto level = generator();
	//	beep.buzz(level, frameCycles);
	//	if (++frameCycles == (Ula::TotalFrameClocks / 2)) {
	//		frameCycles = 0;
	//		beep.endFrame();
	//		//std::cout << ".";
	//	}
	//	++count;
	//}


	const auto finish_time = std::chrono::steady_clock::now();
	const auto elapsed_time = finish_time - start_time;
	const auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_time).count();

	std::cout.imbue(std::locale(""));
	std::cout
		<< "Retrieved " << count << " amplitudes. "
		<< "Elapsed time: " << seconds << " seconds. "
		<< int(count / seconds) << " amplitudes per second."
		<< std::endl;
}

int main(int, char*[])
{
	Configuration configuration;

#ifdef _DEBUG
	//configuration.setDebugMode(true);
	//configuration.setProfileMode(true);
	//configuration.setDrawGraphics(true);
#endif

	auto computer = std::make_unique<Computer>(configuration);

	computer->plug(std::make_shared<KempstonJoystick>(computer->BUS()));
	computer->plug(std::make_shared<Interface2Joystick>(computer->BUS()));

	computer->raisePOWER();

	try {
		loadROM(configuration, *computer);
	} catch (const std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "Problem loading ZX Spectrum ROM: %s", error.what());
		return 2;
	}

	try {
		loadProgram(configuration, *computer);
	} catch (const std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "Problem loading ZX Spectrum program: %s", error.what());
		return 2;
	}

	//try {
	//	testTapeLoading(configuration);
	//} catch (const std::exception& error) {
	//	::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "Problem testing tape loading: %s", error.what());
	//	return 2;
	//}

	try {
		computer->runLoop();
	} catch (const std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "Problem running ZX Spectrum: %s", error.what());
		return 2;
	}

	return 0;
}
