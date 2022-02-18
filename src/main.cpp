#include "utils.h"

#include <SDL.h>

#include <array>
#include <iostream>
#include <algorithm>

using size_t = std::size_t;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

constexpr size_t CELL_SIZE = 32;
constexpr size_t MAP_WIDTH = SCREEN_WIDTH / CELL_SIZE;
constexpr size_t MAP_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

class Clock {
 public:
  void start() {
    start_ = SDL_GetPerformanceCounter();
    last_frame_ = start_;
    now_ = start_;
  }

  void update() {
    last_frame_ = now_;
    now_ = SDL_GetPerformanceCounter();
  }

  double deltaTime() const {
    return (now_ - last_frame_) / static_cast<double>(SDL_GetPerformanceFrequency());
  }

 private:
  Uint64 start_;
  Uint64 last_frame_;
  Uint64 now_;
};

struct Input {
  SDL_Point dir;

  void clear() {
    dir.x = 0;
    dir.y = 0;
  }
};

struct Player {
  SDL_FPoint pos{100.0, 100.0};
  SDL_FPoint dir{1.0f, 0.0f};
};

using Map = std::array<int, MAP_WIDTH * MAP_HEIGHT>;

struct GameState {
  Player player;
  Input input;
  Clock clock;
  Map map;

  GameState() {
    createMap();
  }

 private:
  void createMap() {
    for (size_t i = 0; i < MAP_WIDTH; ++i) {
      size_t j = MAP_WIDTH * (MAP_HEIGHT - 1) + i;
      map[i] = 1;
      map[j] = 1;
    }
    for (size_t i = 1; i < MAP_HEIGHT - 1; ++i) {
      size_t left = i * MAP_WIDTH;
      size_t right = i * MAP_WIDTH + MAP_WIDTH - 1;
      map[left] = 1;
      map[right] = 1;
    }
  }
};

GameState state;

void printSdlError(const char* title) {
  std::cerr << title << ": " << SDL_GetError() << '\n';
}

void renderMap(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
  for (size_t i = 0; i < state.map.size(); ++i) {
    if (state.map[i] == 0) {
      continue;
    }
    int x = static_cast<int>(i % MAP_WIDTH * CELL_SIZE);
    int y = static_cast<int>(i / MAP_WIDTH * CELL_SIZE);
    SDL_Rect rect{x, y, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
  }
}

void renderPlayer(SDL_Renderer* renderer) {
  const Player& player = state.player;
  const int ray_len = 30;
  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
  const int ray_x = player.pos.x + ray_len * player.dir.x;
  const int ray_y = player.pos.y + ray_len * player.dir.y;
  SDL_RenderDrawLine(renderer, player.pos.x, player.pos.y, ray_x, ray_y);

  const int player_size = 5;
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
  const int x = static_cast<int>(player.pos.x - player_size / 2);
  const int y = static_cast<int>(player.pos.y - player_size / 2);
  SDL_Rect rect{x, y, player_size, player_size};
  SDL_RenderFillRect(renderer, &rect);
}

void render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
  SDL_RenderClear(renderer);

  renderMap(renderer);
  renderPlayer(renderer);

  SDL_RenderPresent(renderer);
}

void onMouseDown(const SDL_MouseButtonEvent& event) {
  if (event.button == SDL_BUTTON_RIGHT) {
    int x = event.x / CELL_SIZE;
    int y = event.y / CELL_SIZE;
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
      int index = y * MAP_WIDTH + x;
      state.map[index] = 1 - state.map[index];
    }
  }
}

void updateInput() {
  state.input.clear();
  const Uint8* keys = SDL_GetKeyboardState(nullptr);
  if (keys[SDL_SCANCODE_W]) {
    state.input.dir.y = -1;
  }
  if (keys[SDL_SCANCODE_S]) {
    state.input.dir.y = 1;
  }
  if (keys[SDL_SCANCODE_A]) {
    state.input.dir.x = -1;
  }
  if (keys[SDL_SCANCODE_D]) {
    state.input.dir.x = 1;
  }
}

void updatePlayer(float dt) {
  int x;
  int y;
  SDL_GetMouseState(&x, &y);
  auto& player = state.player;
  player.dir = normalize({x - player.pos.x, y - player.pos.y});

  float player_speed = 50.0f;
  player.pos.x += state.input.dir.x * player_speed * dt;
  player.pos.y += state.input.dir.y * player_speed * dt;

  player.pos = clamp(player.pos, {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT});
}

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printSdlError("Unable to init SDL");
    exit(EXIT_FAILURE);
  }

  SDL_Window* window = SDL_CreateWindow("Raycast 3D", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                        SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printSdlError("Unable to create window");
    exit(EXIT_FAILURE);
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    printSdlError("Unable to create renderer");
    exit(EXIT_FAILURE);
  }

  bool is_running = true;
  SDL_Event e;
  state.clock.start();

  while (is_running) {
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT: {
          is_running = false;
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          onMouseDown(e.button);
          break;
        }
      }
    }

    state.clock.update();
    updateInput();
    updatePlayer(state.clock.deltaTime());
    render(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}
