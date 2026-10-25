#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <utility>

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
  bool DrawCurrentFrame();

private:
  SDL_Renderer *renderer_ = nullptr;
  SDL_Texture *texture_ = nullptr;

  sk_sp<SkSurface> surface_;
  std::unique_ptr<TSkiaRenderer> uiRenderer_;

  std::pair<int, int> currentRenderSize_ = std::make_pair(0, 0);
};
} // namespace tuix
