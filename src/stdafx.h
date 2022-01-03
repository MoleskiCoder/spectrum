#ifdef _MSC_VER
#pragma once
#endif

#define USE_COROUTINES
#define NOMINMAX

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef USE_COROUTINES
#if __cplusplus >= 202002L
#	include <co_generator_t.h>
#else
#	include <boost/coroutine2/all.hpp>
#endif
#endif

#if __cplusplus >= 202002L
#	include <span>
#else
#	include <boost/core/span.hpp>
#endif

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

#include <Disassembler.h>
#include <Profiler.h>
#include <Z80.h>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#endif
