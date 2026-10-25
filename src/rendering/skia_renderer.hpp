#pragma once

#include "tuix/rendering/rect.hpp"
#include "tuix/rendering/renderer.hpp"
#include <include/core/SkRect.h>

class SkCanvas;

namespace tuix {
class TSkiaRenderer final : public TRenderer {
public:
  explicit TSkiaRenderer(SkCanvas &canvas);
  void Clear(tuix::TColor color) override;
  void FillRoundedRect(tuix::TRect bounds, float radius,
                       tuix::TColor color) override;
  void ChangeCanvas(SkCanvas &canvas) override;

private:
  SkCanvas *canvas_;
};
} // namespace tuix
