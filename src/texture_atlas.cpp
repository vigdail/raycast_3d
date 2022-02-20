#include "texture_atlas.h"

#include "texture.h"

#include <cassert>

TextureAtlas::TextureAtlas(std::unique_ptr<Texture>&& texture, uint32_t count_x, uint32_t count_y)
    : texture_{std::move(texture)}, frame_width_{texture->w / count_x}, frame_height_{texture->h / count_y} {
  assert(frame_width_ * count_x == texture_->w);
  assert(frame_height_ * count_y == texture_->h);
}

std::vector<uint32_t> TextureAtlas::column(const size_t frame_x, const size_t frame_y, const size_t row, const size_t height) const {
  const size_t u = frame_x * frame_width_ + row;
  const size_t v = frame_y * frame_height_;
  std::vector<uint32_t> column(height);
  for (int y = 0; y < height; ++y) {
    column[y] = texture_->get(u, v + y * frame_height_ / height);
  }
  return column;
}

uint32_t TextureAtlas::getFrameWidth() const {
  return frame_width_;
}
