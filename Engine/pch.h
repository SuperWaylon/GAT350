// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

// Standard Library Includes
#include <array>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <algorithm>

// Third-Party Includes
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <fmod.hpp>
#include <fmod_errors.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#endif //PCH_H
