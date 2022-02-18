#include <SDL.h>

#include <array>
#include <iostream>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 512;

constexpr size_t CELL_SIZE = 64;
constexpr size_t MAP_WIDTH = 10;
constexpr size_t MAP_HEIGHT = 8;

struct Player {
  SDL_Point pos{100, 100};
  float angle{0.0f};
};

using Map = std::array<int, MAP_WIDTH * MAP_HEIGHT>;

Player player;
constexpr Map map = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 1, 1, 0, 1, //
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, //
    1, 0, 0, 0, 1, 1, 1, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 1, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, //
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //
};

void printSdlError(const char *title) {
  std::cerr << title << ": " << SDL_GetError() << '\n';
}

void renderMap(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
  for (size_t i = 0; i < map.size(); ++i) {
    if (map[i] == 0) {
      continue;
    }
    int x = static_cast<int>(i % MAP_WIDTH * CELL_SIZE);
    int y = static_cast<int>(i / MAP_WIDTH * CELL_SIZE);
    SDL_Rect rect{x, y, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
  }
}

void renderPlayer(SDL_Renderer *renderer) {
  const int ray_len = 30;
  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
  int ray_x = player.pos.x + ray_len * static_cast<int>(cos(player.angle));
  int ray_y = player.pos.y + ray_len * static_cast<int>(sin(player.angle));
  SDL_RenderDrawLine(renderer, player.pos.x, player.pos.y, ray_x, ray_y);

  const int player_size = 5;
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
  SDL_Rect rect{player.pos.x - player_size / 2, player.pos.y - player_size / 2,
                player_size, player_size};
  SDL_RenderFillRect(renderer, &rect);
}

void render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
  SDL_RenderClear(renderer);

  renderMap(renderer);
  renderPlayer(renderer);

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
