#include "frame_buffer.h"

#include <cassert>

void FrameBuffer::setPixel(const size_t x, const size_t y, const uint32_t color) {
  assert(x < w_);
  assert(y < h_);
  data_[y * w_ + x] = color;
}

void FrameBuffer::drawRect(const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color) {
  assert(x + w <= w_);
  assert(y + h <= h_);
  for (auto j = y; j < y + h; ++j){
    for(auto i = x; i < x + w; ++i) {
      data_[j * w_ + i] = color;
    }
  }
}

void FrameBuffer::clear() {
  for(auto i =0 ; i<data_.size(); ++i) {
    data_[i] = 0xff000000;
  }
}
