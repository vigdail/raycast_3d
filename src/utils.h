#pragma once

#include <SDL.h>

SDL_FPoint normalize(const SDL_FPoint& point);
SDL_FPoint clamp(const SDL_FPoint& point, const SDL_FPoint& min, const SDL_FPoint& max);