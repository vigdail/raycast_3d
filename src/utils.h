#pragma once

#include <SDL.h>

SDL_FPoint normalize(const SDL_FPoint& point);
SDL_FPoint clamp(const SDL_FPoint& point, const SDL_FPoint& min, const SDL_FPoint& max);
float clamp(float v, float min, float max);
float len2(const SDL_FPoint& point);
float distance(const SDL_FPoint& a, const SDL_FPoint& b);
SDL_Color unpackColor(const uint32_t color);