#include "Configuration.h"
#include "Computer.h"

#include <SDL3/SDL.h>

class Application final {
private:
	Configuration _configuration;
	Computer _computer;

	void loadROM();
	void loadProgram();

public:
	Application();

	void initialise();
	void terminate();
	SDL_AppResult iterate();
	SDL_AppResult handleEvent(SDL_Event& event);
};
