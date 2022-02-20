#include "utils.h"

#include <algorithm>

SDL_FPoint normalize(const SDL_FPoint& point) {
  float len = sqrt(point.x * point.x + point.y * point.y);
  SDL_FPoint result;
  result.x = point.x / len;
  result.y = point.y / len;

  return result;
}

SDL_FPoint clamp(const SDL_FPoint& point, const SDL_FPoint& min, const SDL_FPoint& max) {
  float x = std::clamp(point.x, min.x, max.x);
  float y = std::clamp(point.y, min.y, max.y);
  return {x, y};
}

float clamp(float v, float min, float max) {
  return std::clamp(v, min, max);
}

float distance(const SDL_FPoint& a, const SDL_FPoint& b) {
  const float x = a.x - b.x;
  const float y = a.y - b.y;
  return sqrt(x * x + y * y);
}

float len2(const SDL_FPoint& point) {
  return point.x * point.x + point.y * point.y;
}

SDL_Color unpackColor(const uint32_t color) {
  const Uint8 a = color >> 24 & 0xff;
  const Uint8 r = color >> 16 & 0xff;
  const Uint8 g = color >> 8 & 0xff;
  const Uint8 b = color & 0xff;
  return {r, g, b, a};
}

uint32_t packColor(const SDL_Color& color) {
  return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}
