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
