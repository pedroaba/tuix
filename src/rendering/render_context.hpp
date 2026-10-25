#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"

#include "rendering/skia_renderer.hpp"

namespace tuix {
class TRenderContext {
public:
  explicit TRenderContext(SDL_Renderer *renderer);
  ~TRenderContext();

  TRenderContext(const TRenderContext &) = delete;
  TRenderContext &operator=(const TRenderContext &) = delete;

  bool Resize(int width, int height);
  bool RenderFrame();
  bool PresentFrame();

private:
  SDL_Renderer *renderer_ = nullptr;
  SDL_Texture *texture_ = nullptr;

  sk_sp<SkSurface> surface_;
  std::unique_ptr<TSkiaRenderer> uiRenderer_;
};
} // namespace tuix
