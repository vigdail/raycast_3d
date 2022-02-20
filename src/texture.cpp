#include "texture.h"

#include <SDL.h>

#include <cassert>
#include <cstring>
#include <iostream>

Texture::Texture(const int w, const int h, const void* data) : w(w), h(h) {
  pixels.resize(w * h);
  memcpy(pixels.data(), data, w * h * sizeof(uint32_t));
}

Texture Texture::load(const std::filesystem::path& path) {
  SDL_Surface* surface = SDL_LoadBMP(path.c_str());
  const Texture texture(surface->w, surface->h, surface->pixels);
  SDL_FreeSurface(surface);

  return texture;
}

uint32_t Texture::get(const size_t x, const size_t y) const {
  assert(x < w && y < h);
  const size_t index = y * w + x;
  return pixels[index];
}
