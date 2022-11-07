#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

class FrameBuffer {
 public:
  FrameBuffer(size_t w, size_t h) : w_{w}, h_{h}, data_(w * h) {}
  void setPixel(size_t x, size_t y, uint32_t color);
  void drawRect(size_t x, size_t y, size_t w, size_t h, uint32_t color);
  [[maybe_unused]] void clear();
  size_t getWidth() const {
    return w_;
  }
  size_t getHeight() const {
    return h_;
  }
  const std::vector<uint32_t>& getData() const {
    return data_;
  }

 private:
  size_t w_;
  size_t h_;
  std::vector<uint32_t> data_;
};
