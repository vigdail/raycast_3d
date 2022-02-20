#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

class FrameBuffer {
 public:
  FrameBuffer(size_t w, size_t h) : w_{w}, h_{h}, data_(w * h) {}
  void setPixel(const size_t x, const size_t y, const uint32_t color);
  void drawRect(const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color);
  void clear();
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
