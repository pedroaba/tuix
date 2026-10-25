#include "rendering/render_context.hpp"

#include <utility>

#include "include/core/SkImageInfo.h"
#include "include/core/SkPixmap.h"

#include "tuix/components/component.hpp"

namespace tuix {

TRenderContext::TRenderContext(SDL_Renderer *renderer) : renderer_(renderer) {}

TRenderContext::~TRenderContext() {
  uiRenderer_.reset();
  surface_.reset();

  if (texture_ != nullptr) {
    SDL_DestroyTexture(texture_);
  }
}

bool TRenderContext::Resize(int width, int height) {
  if (renderer_ == nullptr || width <= 0 || height <= 0) {
    return false;
  }

  SkImageInfo imageInfo = SkImageInfo::Make(
      width, height, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
  auto newSurface = SkSurfaces::Raster(imageInfo);

  if (!newSurface) {
    SDL_Log("Failed to create Skia surface");
    return false;
  }

  SDL_Texture *newTexture =
      SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGRA32,
                        SDL_TEXTUREACCESS_STATIC, width, height);

  if (!newTexture) {
    SDL_Log("Texture error: %s", SDL_GetError());
    return false;
  }

  SkCanvas *newCanvas = newSurface->getCanvas();

  if (uiRenderer_ == nullptr) {
    uiRenderer_ = std::make_unique<TSkiaRenderer>(*newCanvas);
  } else {
    uiRenderer_->ChangeCanvas(*newCanvas);
  }

  surface_ = std::move(newSurface);

  // atualizando a textura
  SDL_DestroyTexture(texture_);
  texture_ = newTexture;

  return true;
}

bool TRenderContext::RenderFrame() {
  if (!surface_ || !texture_ || !uiRenderer_) {
    return false;
  }

  if (root_ == nullptr) {
    SDL_Log("No root component configured");
    return false;
  }

  TRenderer &renderer = *uiRenderer_;
  renderer.Clear({255, 255, 255, 255});

  root_->Arrange({0.0f, 0.0f, static_cast<float>(surface_->width()),
                  static_cast<float>(surface_->height())});
  root_->Paint(renderer);

  SkPixmap pixmap;
  if (!surface_->peekPixels(&pixmap)) {
    SDL_Log("Failed to access Skia pixels");
    return false;
  }

  bool isAbleToUpdatedTexture = SDL_UpdateTexture(
      texture_, nullptr, pixmap.addr(), static_cast<int>(pixmap.rowBytes()));

  if (!isAbleToUpdatedTexture) {
    SDL_Log("Texture update error: %s", SDL_GetError());
    return false;
  }

  return true;
}

bool TRenderContext::PresentFrame() {
  if (renderer_ == nullptr || texture_ == nullptr) {
    return false;
  }

  bool isSuccessOperation = SDL_RenderClear(renderer_);
  if (!isSuccessOperation) {
    return false;
  }

  isSuccessOperation = SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);
  if (!isSuccessOperation) {
    return false;
  }

  return SDL_RenderPresent(renderer_);
}

bool TRenderContext::DrawCurrentFrame() {
  // 1. Consultar o tamanho atual da saída SDL
  int w = 0, h = 0;
  if (!SDL_GetRenderOutputSize(renderer_, &w, &h)) {
    SDL_Log("Failed to get output size: %s", SDL_GetError());
    return false;
  }

  if (w <= 0 || h <= 0) {
    return true;
  }

  // 2. Se mudou, executar Resize()
  if (!Resize(w, h)) {
    return false;
  }

  currentRenderSize_.first = w;
  currentRenderSize_.second = h;

  // 3. Executar RenderFrame() e PresentFrame()
  return Render();
}

bool TRenderContext::Render() {
  if (!RenderFrame()) {
    return false;
  }

  return PresentFrame();
}

void TRenderContext::SetRoot(TComponent &component) { root_ = &component; }
} // namespace tuix