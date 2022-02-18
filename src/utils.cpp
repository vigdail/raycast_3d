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
