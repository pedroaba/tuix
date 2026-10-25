#pragma once

namespace tuix {

struct TRect {
  float x;
  float y;
  float width;
  float height;

  void ToRenderer(auto &rect);
};

} // namespace tuix