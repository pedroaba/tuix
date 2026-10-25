
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

#include "rendering/render_context.hpp"
#include "tuix/components/box.hpp"
#include "tuix/components/container.hpp"
#include "tuix/layout/alignment.hpp"
#include "tuix/layout/direction.hpp"

constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;
constexpr const char *APP_NAME = "TUIX ENGINE";

bool SDLCALL OnWindowEvent(void *userdata, SDL_Event *event) {
  if (event->type == SDL_EVENT_WINDOW_EXPOSED && event->window.data1 == 1) {
    auto *context = static_cast<tuix::TRenderContext *>(userdata);

    if (!context->DrawCurrentFrame()) {
      SDL_Log("Failed to render during resize");
    }
  }

  return true;
}

int main(int argc, char *argv[]) {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL Error: %s", SDL_GetError());
    return 1;
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow(APP_NAME, SCREEN_WIDTH, SCREEN_HEIGHT,
                                        SDL_WINDOW_RESIZABLE);

  if (!window) {
    SDL_Log("Window Error: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create SDL renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  if (!renderer) {
    SDL_Log("Renderer Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  int exitCode = 0;

  {
    tuix::TContainer root = tuix::TContainer(
        {.gap = 4.0f, .alignment = tuix::TLayoutAlignment::Start});

    tuix::TContainer horizontal =
        tuix::TContainer({.padding = 0.0f,
                          .gap = 4.0f,
                          .direction = tuix::TLayoutDirection::Horizontal});
    horizontal.AddChild(std::make_unique<tuix::TBox>(tuix::TBoxProps{
        .size = {120, 60}, .background = {0, 0, 255, 255}, .radius = 8}));

    horizontal.AddChild(std::make_unique<tuix::TBox>(tuix::TBoxProps{
        .size = {480, 60}, .background = {0, 255, 0, 255}, .radius = 8}));

    horizontal.AddChild(std::make_unique<tuix::TBox>(tuix::TBoxProps{
        .size = {280, 60}, .background = {255, 0, 0, 255}, .radius = 8}));

    root.AddChild(std::make_unique<tuix::TBox>(tuix::TBoxProps{
        .size = {170, 60}, .background = {0, 0, 255, 255}, .radius = 8}));

    root.AddChild(std::make_unique<tuix::TContainer>(std::move(horizontal)));
    root.AddChild(std::make_unique<tuix::TBox>(tuix::TBoxProps{
        .size = {480, 60}, .background = {0, 255, 0, 255}, .radius = 8}));

    root.AddChild(std::make_unique<tuix::TBox>(tuix::TBoxProps{
        .size = {280, 60}, .background = {255, 0, 0, 255}, .radius = 8}));

    // RenderContext owns the texture and Skia surface
    tuix::TRenderContext graphics(renderer);
    graphics.SetRoot(root);

    bool running = true;
    bool needsResize = true;

    SDL_AddEventWatch(OnWindowEvent, &graphics);
    while (running) {
      SDL_Event event;

      while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          running = false;
          break;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_RENDER_DEVICE_RESET:
          needsResize = true;
          break;

        default:
          break;
        }
      }

      if (!running) {
        break;
      }

      if (needsResize) {
        int width = 0;
        int height = 0;

        if (!SDL_GetRenderOutputSize(renderer, &width, &height)) {
          SDL_Log("Output size error: %s", SDL_GetError());
          exitCode = 1;
          break;
        }

        if (width <= 0 || height <= 0) {
          SDL_Delay(16);
          continue;
        }

        if (!graphics.Resize(width, height)) {
          exitCode = 1;
          break;
        }

        needsResize = false;
      }

      if (!graphics.Render()) {
        SDL_Log("Rendering failed: %s", SDL_GetError());
        exitCode = 1;
        break;
      }

      SDL_Delay(16);
    }

    // limpando o evento
    SDL_RemoveEventWatch(OnWindowEvent, &graphics);
  }

  // Graphics context is already destroyed here
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return exitCode;
}
