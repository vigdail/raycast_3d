#include <SDL.h>

#include <iostream>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

void printSdlError(const char *title) {
  std::cerr << title << ": " << SDL_GetError() << '\n';
}

void render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
  SDL_RenderClear(renderer);

  SDL_RenderPresent(renderer);
}

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printSdlError("Unable to init SDL");
    exit(EXIT_FAILURE);
  }

  SDL_Window *window = SDL_CreateWindow("Raycast 3D", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                        SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printSdlError("Unable to create window");
    exit(EXIT_FAILURE);
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    printSdlError("Unable to create renderer");
    exit(EXIT_FAILURE);
  }

  bool is_running = true;
  SDL_Event e;

  while (is_running) {
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
      case SDL_QUIT:
        is_running = false;
        break;
      }
    }

    render(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}
