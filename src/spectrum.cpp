#include "stdafx.h"

#include "Configuration.h"
#include "Computer.h"
#include "KempstonJoystick.h"

int main(int, char*[])
{
	Configuration configuration;

#ifdef _DEBUG
	configuration.setDebugMode(true);
	configuration.setProfileMode(true);
	configuration.setDrawGraphics(true);
#endif

	Computer computer(configuration);

	computer.plug(std::make_shared<KempstonJoystick>(computer.BUS()));

	computer.raisePOWER();

	auto romDirectory = configuration.getRomDirectory();
	//computer.plug(romDirectory + "\\G12R_ROM.bin");	// Planetoids (Asteroids)
	//computer.plug(romDirectory + "\\G24R_ROM.bin");	// Horace and the Spiders
	//computer.plug(romDirectory + "\\G9R_ROM.bin");	// Space Raiders (Space Invaders)
	//computer.plug(romDirectory + "\\Jet Pac (1983)(Sinclair Research)(GB).rom");	// Jet Pac
	
	//computer.plug(romDirectory + "\\System_Test_ROM.bin");	// Sinclair test ROM by Dr. Ian Logan
	//computer.plug(romDirectory + "\\Release-v0.37\\testrom.bin");
	//computer.plug(romDirectory + "\\smart\\ROMs\\DiagROM.v41");

	auto programDirectory = configuration.getProgramDirectory();

	//computer.loadSna(programDirectory + "\\ant_attack.sna");	// 3D ant attack
	//computer.loadSna(programDirectory + "\\zexall.sna");

	computer.loadZ80(programDirectory + "\\Jetpac (1983)(Ultimate Play The Game)[a][16K].z80");
	//computer.loadZ80(programDirectory + "\\HELCHOP.Z80");
	//computer.loadZ80(programDirectory + "\\TFF4.Z80");
	//computer.loadZ80(programDirectory + "\\BABY.Z80");
	//computer.loadZ80(programDirectory + "\\ATARI2.Z80");
	//computer.loadZ80(programDirectory + "\\HEDGEHOG.Z80");
	//computer.loadZ80(programDirectory + "\\Knight Lore (1984)(Ultimate).z80");

	try {
		computer.runLoop();
	} catch (const std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
		return 2;
	}

	return 0;
}