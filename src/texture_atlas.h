#pragma once

#include <memory>
#include <vector>

class Texture;

class TextureAtlas {
 public:
  TextureAtlas(std::unique_ptr<Texture>&& texture, const uint32_t count_x, const uint32_t count_y);
  std::vector<uint32_t> column(const size_t frame_x, const size_t frame_y, const size_t row, const size_t height) const;
 private:
  uint32_t frame_width_;

 public:
  uint32_t getFrameWidth() const;

 private:
  uint32_t frame_height_;
  std::unique_ptr<Texture> texture_;
};
