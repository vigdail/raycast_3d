#pragma once

#include <SDL.h>

[[maybe_unused]] SDL_FPoint normalize(const SDL_FPoint& point);
SDL_FPoint clamp(const SDL_FPoint& point, const SDL_FPoint& min, const SDL_FPoint& max);
float clamp(float v, float min, float max);
float distance(const SDL_FPoint& a, const SDL_FPoint& b);
