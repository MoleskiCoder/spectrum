#include "stdafx.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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
	//computer.plug(romDirectory + "\\diagrom\\DiagROMv.171");
	computer.plug(romDirectory + "\\diagrom\\DiagROMv.173");

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

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	Configuration configuration;
	auto* computer = new Computer(configuration);

    computer->raisePOWER();

    computer->plug(std::make_shared<KempstonJoystick>(computer->BUS()));
    //computer->plug(std::make_shared<Interface2Joystick>(computer->BUS()));

	loadROM(configuration, *computer);
    loadProgram(configuration, *computer);

    ::SDL_LogInfo(::SDL_LOG_CATEGORY_APPLICATION, "Completed application initialisation");

    *appstate = computer;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	::SDL_LogInfo(::SDL_LOG_CATEGORY_APPLICATION, "Terminating application");
	((Computer*)appstate)->lowerPOWER();
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	::SDL_LogDebug(::SDL_LOG_CATEGORY_APPLICATION, "Executing application frame");
	return ((Computer*)appstate)->runFrame();
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	::SDL_LogDebug(::SDL_LOG_CATEGORY_APPLICATION, "Handling application event");
	return ((Computer*)appstate)->handleEvent(*event);
}
