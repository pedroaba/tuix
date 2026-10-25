#pragma once

#include "tuix/rendering/color.hpp"
#include "tuix/rendering/rect.hpp"

class SkCanvas;

namespace tuix {

class TRenderer {
public:
  virtual ~TRenderer() = default;
  virtual void Clear(TColor color) = 0;
  virtual void FillRoundedRect(tuix::TRect bounds, float radius,
                               tuix::TColor color) = 0;
  virtual void ChangeCanvas(SkCanvas &canvas) = 0;
};

} // namespace tuix
