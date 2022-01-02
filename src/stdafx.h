#ifdef _MSC_VER
#pragma once
#endif

#define NOMINMAX

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/dynamic_bitset.hpp>

#include <SDL.h>

#include <Game.h>
#include <SDLWrapper.h>

#include <Bus.h>
#include <Chip.h>
#include <ClockedChip.h>
#include <Device.h>
#include <InputOutput.h>
#include <Register.h>
#include <Ram.h>
#include <Rom.h>
#include <Signal.h>

#include <Z80.h>
#include <Profiler.h>
#include <Disassembler.h>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#endif
