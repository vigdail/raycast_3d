#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>

struct Texture {
  int w;
  int h;
  std::vector<uint32_t> pixels;

  Texture(const int w, const int h, const void* data);
  static Texture load(const std::filesystem::path& path);
  uint32_t get(const size_t x, const size_t y) const;
};