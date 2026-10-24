#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstddef>

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkSurface.h"

#include "tuix/core/print.hpp"

constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;

const char *APP_NAME = "TUIX ENGINE";

bool isRunningApplication = true;

bool SDLCALL OnWindowEvent(void *userdata, SDL_Event *event) {
  if (event->type == SDL_EVENT_WINDOW_EXPOSED && event->window.data1 == 1) {
    auto *window = static_cast<SDL_Window *>(userdata);

    int width = 0;
    int height = 0;

    SDL_GetWindowSize(window, &width, &height);

    tuix::tprint("Resizing: ", width, "x", height);
  }

  return true;
}

void CleanupSDL(SDL_Window *window = nullptr, SDL_Renderer *renderer = nullptr,
                SDL_Texture *texture = nullptr) {
  if (window != nullptr) {
    SDL_RemoveEventWatch(OnWindowEvent, window);
  }

  if (texture != nullptr) {
    tuix::tprint("DESTRUINDO TEXTURA: ", SDL_GetTextureProperties(texture));
    SDL_DestroyTexture(texture);
  }

  if (renderer != nullptr) {
    tuix::tprint("DESTRUINDO RENDERER: ", SDL_GetRendererName(renderer));
    SDL_DestroyRenderer(renderer);
  }

  if (window != nullptr) {
    tuix::tprint("DESTRUINDO JANELA: ", SDL_GetWindowID(window));
    SDL_DestroyWindow(window);
  }

  SDL_Quit();
}

void ProcessSDLEvents(SDL_Window *window) {
  SDL_Event event;
  int windowWidth = 0, windowHeight = 0;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      tuix::tprint("USUÁRIO FECHOU A APLICAÇÃO, BYE BYE");
      isRunningApplication = false;
      break;
    case SDL_EVENT_WINDOW_RESIZED:
      tuix::tprint("WINDOW WIDTH: ", event.window.data1);
      tuix::tprint("WINDOW HEIGHT: ", event.window.data2);
      break;
    // case SDL_EVENT_WINDOW_MOVED:
    //   tuix::tprint("WINDOW POSITION X: ", event.window.data1);
    //   tuix::tprint("WINDOW POSITION Y: ", event.window.data2);
    //   break;
    case SDL_EVENT_WINDOW_EXPOSED:
      SDL_GetWindowSize(window, &windowWidth, &windowHeight);

      tuix::tprint("WINDOW WIDTH: ", windowWidth);
      tuix::tprint("WINDOW HEIGHT: ", windowHeight);
      break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
      tuix::tprint("USUÁRIO ESTÁ NA TELA");
      break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
      tuix::tprint("USUÁRIO NÃO ESTÁ NA TELA");
      break;
    }
  }
}

int main(int argc, char *args[]) {
  bool isInitialized = SDL_Init(SDL_INIT_VIDEO);
  if (!isInitialized) {
    SDL_Log("SDL Error: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(APP_NAME, SCREEN_WIDTH, SCREEN_HEIGHT,
                                        SDL_WINDOW_RESIZABLE);

  if (window == nullptr) {
    SDL_Log("Window Error: %s", SDL_GetError());
    CleanupSDL();

    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  if (renderer == nullptr) {
    SDL_Log("Renderer Error: %s", SDL_GetError());
    CleanupSDL(window);

    return 1;
  }

  int width, height;
  bool isSuccessOperation = SDL_GetWindowSizeInPixels(window, &width, &height);
  if (!isSuccessOperation) {
    SDL_Log("Window size error: %s", SDL_GetError());
    CleanupSDL(window, renderer);

    return 1;
  }

  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32,
                        SDL_TEXTUREACCESS_STREAMING, width, height);

  if (texture == nullptr) {
    SDL_Log("Texture error: %s", SDL_GetError());

    CleanupSDL(window, renderer);
    return 1;
  }

  void *pixels = nullptr;
  int pitch = 0;

  bool isTextureLock = SDL_LockTexture(texture, nullptr, &pixels, &pitch);
  if (!isTextureLock) {
    SDL_Log("Texture error: %s", SDL_GetError());
    CleanupSDL(window, renderer, texture);

    return 1;
  }

  SkImageInfo imageInfo = SkImageInfo::Make(
      width, height, kBGRA_8888_SkColorType, kPremul_SkAlphaType);

  auto surface = SkSurfaces::Raster(imageInfo);

  if (surface == nullptr) {
    CleanupSDL(window, renderer, texture);
    SDL_Log("Failed to create Skia surface");

    return 1;
  }

  SkCanvas *canvas = surface->getCanvas();
  if (canvas == nullptr) {
    CleanupSDL(window, renderer, texture);
    SDL_Log("Failed to create Skia canvas");

    return 1;
  }

  // cleanup canvas com uma cor branca
  canvas->clear(SK_ColorWHITE);
  SkPaint paint;

  paint.setColor(SK_ColorBLUE);
  paint.setAntiAlias(true);

  canvas->drawRoundRect(SkRect::MakeXYWH(40, 40, 180, 60), 8, 8, paint);

  SkPixmap pixmap;
  if (!surface->peekPixels(&pixmap)) {
    SDL_Log("Failed to access Skia pixels");

    CleanupSDL(window, renderer, texture);
    return 1;
  }

  SDL_UpdateTexture(texture, nullptr, pixmap.addr(),
                    static_cast<int>(pixmap.rowBytes()));

  SDL_AddEventWatch(OnWindowEvent, window);
  while (isRunningApplication) {
    ProcessSDLEvents(window);

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    SDL_Delay(16); // irá mudar depois
  }

  CleanupSDL(window, renderer, texture);

  return 0;
}
