#include "skia_renderer.hpp"

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include <include/core/SkRect.h>

namespace tuix {
SkRect FromTRect(tuix::Rect *rect) {
  SkRect skRect = SkRect();
  skRect.setXYWH(rect->x, rect->y, rect->width, rect->height);

  return skRect;
}

TSkiaRenderer::TSkiaRenderer(SkCanvas &canvas) : canvas_(&canvas) {}

void TSkiaRenderer::Clear(Color color) {
  canvas_->clear(
      SkColorSetARGB(color.alpha, color.red, color.green, color.blue));
}

void TSkiaRenderer::FillRoundedRect(tuix::Rect bounds, float radius,
                                    tuix::Color color) {
  SkPaint paint;

  paint.setARGB(color.alpha, color.red, color.green, color.blue);
  paint.setAntiAlias(true);

  SkRect rect = FromTRect(&bounds);
  canvas_->drawRoundRect(rect, radius, radius, paint);
}

void TSkiaRenderer::ChangeCanvas(SkCanvas &canvas) { canvas_ = &canvas; }

} // namespace tuix