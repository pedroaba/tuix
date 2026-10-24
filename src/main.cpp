#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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

    SDL_Quit();
    return 1;
  }

  SDL_AddEventWatch(OnWindowEvent, window);
  while (isRunningApplication) {
    ProcessSDLEvents(window);

    SDL_Delay(16); // irá mudar depois
  }

  SDL_RemoveEventWatch(OnWindowEvent, window);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
