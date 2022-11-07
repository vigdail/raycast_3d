#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>

struct Texture {
  int w;
  int h;
  std::vector<uint32_t> pixels;

  Texture(int w, int h, const void* data);
  static Texture load(const std::filesystem::path& path);
  uint32_t get(size_t x, size_t y) const;
};