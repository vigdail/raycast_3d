#pragma once

#include <memory>
#include <vector>

class Texture;

class TextureAtlas {
 public:
  TextureAtlas(std::unique_ptr<Texture>&& texture, uint32_t count_x, uint32_t count_y);
  std::vector<uint32_t> column(size_t frame_x, size_t frame_y, size_t row, size_t height) const;
 private:
  uint32_t frame_width_;

 public:
  uint32_t getFrameWidth() const;

 private:
  uint32_t frame_height_;
  std::unique_ptr<Texture> texture_;
};
