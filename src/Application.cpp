#include "stdafx.h"
#include "Application.h"

#include "KempstonJoystick.h"
#include "Interface2Joystick.h"

Application::Application()
: _computer(_configuration) {}

void Application::loadROM() {

	auto romDirectory = _computer.configuration().getRomDirectory();

	//_computer.plug(romDirectory + "\\G12R_ROM.bin");	// Planetoids (Asteroids)
	//_computer.plug(romDirectory + "\\G24R_ROM.bin");	// Horace and the Spiders
	//_computer.plug(romDirectory + "\\G9R_ROM.bin");	// Space Raiders (Space Invaders)
	//_computer.plug(romDirectory + "\\Jet Pac (1983)(Sinclair Research)(GB).rom");	// JetPac
	//_computer.plug(romDirectory + "\\Pssst (1983)(Sinclair Research)(GB).rom"); // Pssst
	//_computer.plug(romDirectory + "\\Knight Lore (1984)(Ultimate Play The Game)(GB)[b].rom"); // Knight Lore, 32k ROM??

	//_computer.plug(romDirectory + "\\System_Test_ROM.bin");	// Sinclair test ROM by Dr. Ian Logan
	//_computer.plug(romDirectory + "\\Release-v0.37\\testrom.bin");
	//_computer.plug(romDirectory + "\\smart\\ROMs\\DiagROM.v41");
	//_computer.plug(romDirectory + "\\DiagROM.v56");
	//_computer.plug(romDirectory + "\\diagrom\\DiagROMv.171");
	_computer.plug(romDirectory + "\\diagrom\\DiagROMv.173");

	//computer.plug(romDirectory + "\\VMM-TEST.ROM");
}

void Application::loadProgram() {

	auto programDirectory = _computer.configuration().getProgramDirectory();

	//_computer.loadZ80(programDirectory + "\\Alien 8 (1985)(Ultimate).z80");	// Works
	//_computer.loadSna(programDirectory + "\\ant_attack.sna");	// 3D ant attack, works

	//_computer.loadZ80(programDirectory + "\\Ballblazer (1985)(Activision).z80");	// Not 48k spectrum

	//_computer.loadZ80(programDirectory + "\\Boulder Dash (1984)(First Star Software).z80");	// Works

	//_computer.loadZ80(programDirectory + "\\Cobra (1986)(Ocean Software).z80");	// Shows title screen, then crashes
	//_computer.loadZ80(programDirectory + "\\Cobra (1986)(Ocean Software)[a2].z80");	// Shows menu screen, then crashes

	//_computer.loadSna(programDirectory + "\\ELITE.SNA");	// freeze
	//_computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird).z80");	// spins for a second, then freezes
	//_computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird)[a].z80");	// z80 v2
	//_computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird)[a2].z80");	// flashes, then freeze/garbage
	//_computer.loadZ80(programDirectory + "\\Elite (1986)(Firebird)[a3].z80");	// freeze

	//_computer.loadZ80(programDirectory + "\\Head Over Heels (1987)(Ocean Software).z80");	// v2, freeze

	//_computer.loadZ80(programDirectory + "\\HELCHOP.Z80");
	//_computer.loadZ80(programDirectory + "\\Helichopper_1985_Firebird_Software.z80");
	//_computer.loadZ80(programDirectory + "\\Helichopper_1985_Firebird_Software_a.z80");
	//_computer.loadZ80(programDirectory + "\\HALLSTHI.z80");

	//_computer.loadZ80(programDirectory + "\\Jetpac_1983_Ultimate_Play_The_Game_16K.z80");	// Doesn't exist
	//_computer.loadZ80(programDirectory + "\\Jetpac (1983)(Ultimate Play The Game)[a][16K].z80");
	//_computer.loadTZX(programDirectory + "\\jetpac.tzx");
	//_computer.loadZ80(programDirectory + "\\JetPac.z80");
	//_computer.loadSna(programDirectory + "\\JetPac.sna");

	//_computer.loadZ80(programDirectory + "\\Knight Lore (1984)(Ultimate).z80");	// Works

	//_computer.loadTZX(programDirectory + "\\Manic Miner.tzx");

	//_computer.loadZ80(programDirectory + "\\R-Type (1988)(Activision).z80");		// v3, not for 48K spectrum
	//_computer.loadZ80(programDirectory + "\\Rommels_Revenge_1983_Crystal_Computing.z80");	// Goes through menus, crashes in game

	//_computer.loadSna(programDirectory + "\\zexall.sna");


	// Utilities
	//_computer.loadZ80(programDirectory + "\\Spectrum Musicmaker (1983)(Robert Newman).z80");	// Works
	//_computer.loadZ80(programDirectory + "\\Spectrum Sound FX (1983)(Marolli Soft).z80");	// v3
	//_computer.loadZ80(programDirectory + "\\Sounds 2 (19xx)(The Champ).z80");	// Not for 48k Spectrums
	//_computer.loadZ80(programDirectory + "\\Sound Demo 01 (1992)(Theo Devil).z80");	// Not for 48k Spectrums
	//_computer.loadZ80(programDirectory + "\\iotbs2.z80");	// Not for 48k Spectrums

	// Demos

	//_computer.loadZ80(programDirectory + "\\TFF4.Z80");
	//_computer.loadZ80(programDirectory + "\\BABY.Z80");	// Plays sampled "I don't care"
	//_computer.loadZ80(programDirectory + "\\ATARI2.Z80");	// hangs
	//_computer.loadZ80(programDirectory + "\\HEDGEHOG.Z80");	// Not for 48K spectrum
}

void Application::initialise() {
	_computer.raisePOWER();

	_computer.plug(std::make_shared<KempstonJoystick>(_computer.BUS()));
	//_computer.plug(std::make_shared<Interface2Joystick>(_computer.BUS()));

	loadROM();
	loadProgram();

	::SDL_LogInfo(::SDL_LOG_CATEGORY_APPLICATION, "Completed application initialisation");
}

void Application::terminate() {
	::SDL_LogInfo(::SDL_LOG_CATEGORY_APPLICATION, "Terminating application");
	_computer.lowerPOWER();
}

SDL_AppResult Application::iterate() {
	::SDL_LogDebug(::SDL_LOG_CATEGORY_APPLICATION, "Executing application frame");
	return _computer.runFrame();
}

SDL_AppResult Application::handleEvent(SDL_Event& event) {
	::SDL_LogDebug(::SDL_LOG_CATEGORY_APPLICATION, "Handling application event");
	return _computer.handleEvent(event);
}
