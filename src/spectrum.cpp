#include "stdafx.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Application.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	Application* application = new Application();
	application->initialise();
    *appstate = application;
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	((Application*)appstate)->terminate();
	delete (Application*)appstate;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	return ((Application*)appstate)->iterate();
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	return ((Application*)appstate)->handleEvent(*event);
}
