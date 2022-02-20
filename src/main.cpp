#include "texture.h"
#include "texture_atlas.h"
#include "utils.h"

#include <SDL.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>

using size_t = std::size_t;

constexpr int SCREEN_WIDTH = 512;
constexpr int SCREEN_HEIGHT = 512;

constexpr size_t TILES_COUNT = 3;
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
  float vertical;
  float horizontal;
  void clear() {
    vertical = 0.0f;
    horizontal = 0.0f;
  }
};

struct Player {
  SDL_FPoint pos{SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
  float angle{0.0f};
  float speed{50.0f};
  float angular_speed{3.0f};
  float fov{M_PI / 3.0f};
};

struct HitRecord {
  SDL_FPoint intersection;
  size_t cell_index;
  int cell_type;
};

using Map = std::array<int, MAP_WIDTH * MAP_HEIGHT>;
TextureAtlas atlas(std::make_unique<Texture>(Texture::load("../assets/tileset.bmp")), 3, 1);

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

std::optional<HitRecord> castRay(const SDL_FPoint& origin, const SDL_FPoint& dir);

void printSdlError(const char* title) {
  std::cerr << title << ": " << SDL_GetError() << '\n';
}

void renderMap(SDL_Renderer* renderer) {
  static const SDL_Color colors[] = {
      {0x00, 0x00, 0xff},
      {0x00, 0xff, 0x00},
      {0xff, 0x00, 0x00},
  };
  for (size_t i = 0; i < state.map.size(); ++i) {
    if (state.map[i] == 0) {
      continue;
    }
    const auto color = colors[state.map[i] - 1];
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int x = static_cast<int>(i % MAP_WIDTH * CELL_SIZE);
    int y = static_cast<int>(i / MAP_WIDTH * CELL_SIZE);
    SDL_Rect rect{x, y, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
  }
}

void renderScene(SDL_Renderer* renderer) {
  const Player& player = state.player;

  {
    SDL_Rect rect{SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_SetRenderDrawColor(renderer, 0xcc, 0xcc, 0xcc, 0xff);
    SDL_RenderFillRect(renderer, &rect);

    rect.y = SCREEN_HEIGHT / 2;
    SDL_SetRenderDrawColor(renderer, 0x99, 0x99, 0x99, 0xff);
    SDL_RenderFillRect(renderer, &rect);
  }

  for (int i = 0; i < SCREEN_WIDTH; ++i) {
    const float current_angle = player.angle - player.fov / 2 + i * player.fov / SCREEN_WIDTH;
    const SDL_FPoint origin{player.pos.x / CELL_SIZE, player.pos.y / CELL_SIZE};
    const SDL_FPoint dir{cos(current_angle), sin(current_angle)};

    const std::optional<HitRecord> hit = castRay(origin, dir);
    if (hit.has_value()) {
      const SDL_FPoint intersection = {hit.value().intersection.x * CELL_SIZE, hit.value().intersection.y * CELL_SIZE};
      {
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(renderer, player.pos.x, player.pos.y, intersection.x, intersection.y);
        const int cell_x = hit.value().cell_index % MAP_WIDTH * CELL_SIZE;
        const int cell_y = hit.value().cell_index / MAP_WIDTH * CELL_SIZE;
        const SDL_Rect rect{cell_x, cell_y, CELL_SIZE, CELL_SIZE};
        SDL_RenderDrawRect(renderer, &rect);
      }
      {
        const float dist = distance(player.pos, intersection);
        const float height = clamp(25 * SCREEN_HEIGHT / (dist * cos(current_angle - player.angle)), 0.0f, SCREEN_HEIGHT);
        const size_t texture_tile_width = atlas.getFrameWidth();
        float hit_x = hit.value().intersection.x - floor(hit.value().intersection.x + 0.5f);
        float hit_y = hit.value().intersection.y - floor(hit.value().intersection.y + 0.5f);
        int texture_x = hit_x * texture_tile_width;
        if (std::abs(hit_y) > std::abs(hit_x)) {
          texture_x = hit_y * texture_tile_width;
        }
        if (texture_x < 0) {
          texture_x += texture_tile_width;
        }
        const auto t = atlas.column((hit.value().cell_type - 1), 0, texture_x, height);
        for (size_t y = 0; y < height; ++y) {
          const SDL_Color color = unpackColor(t[y]);
          SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
          SDL_RenderDrawPoint(renderer, i + SCREEN_WIDTH, (SCREEN_HEIGHT - height) / 2 + y);
        }
      }
    }
  }
}

void renderPlayer(SDL_Renderer* renderer) {
  const Player& player = state.player;

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
  renderScene(renderer);
  renderPlayer(renderer);

  SDL_RenderPresent(renderer);
}

std::optional<HitRecord> castRay(const SDL_FPoint& origin, const SDL_FPoint& dir) {
  SDL_FPoint ray_unit_step_size = {sqrt(1 + (dir.y / dir.x) * (dir.y / dir.x)), sqrt(1 + (dir.x / dir.y) * (dir.x / dir.y))};
  SDL_Point map_pos = {static_cast<int>(origin.x), static_cast<int>(origin.y)};
  SDL_FPoint ray_len_1d;
  SDL_FPoint step;

  if (dir.x < 0) {
    ray_len_1d.x = (origin.x - map_pos.x) * ray_unit_step_size.x;
    step.x = -1;
  } else {
    ray_len_1d.x = (map_pos.x + 1.0f - origin.x) * ray_unit_step_size.x;
    step.x = 1;
  }

  if (dir.y < 0) {
    ray_len_1d.y = (origin.y - map_pos.y) * ray_unit_step_size.y;
    step.y = -1;
  } else {
    ray_len_1d.y = (map_pos.y + 1.0f - origin.y) * ray_unit_step_size.y;
    step.y = 1;
  }

  bool tile_found = false;
  const float max_distance = 100.0f;
  float distance = 0.0f;

  std::optional<HitRecord> record{};

  while (!tile_found && distance < max_distance) {
    if (ray_len_1d.x < ray_len_1d.y) {
      map_pos.x += step.x;
      distance = ray_len_1d.x;
      ray_len_1d.x += ray_unit_step_size.x;
    } else {
      map_pos.y += step.y;
      distance = ray_len_1d.y;
      ray_len_1d.y += ray_unit_step_size.y;
    }

    if (map_pos.x >= 0 && map_pos.x < MAP_WIDTH && map_pos.y >= 0 && map_pos.y < MAP_HEIGHT) {
      const size_t i = map_pos.y * MAP_WIDTH + map_pos.x;
      if (state.map[i] > 0) {
        const float x = origin.x + dir.x * distance;
        const float y = origin.y + dir.y * distance;
        const SDL_FPoint intersection{x, y};
        record = HitRecord{
            intersection,
            i,
            state.map[i],
        };
        tile_found = true;
      }
    }
  }

  return record;
}

void onMouseDown(const SDL_MouseButtonEvent& event) {
  if (event.button == SDL_BUTTON_RIGHT) {
    int x = event.x / CELL_SIZE;
    int y = event.y / CELL_SIZE;
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
      int index = y * MAP_WIDTH + x;
      state.map[index] = (state.map[index] + 1) % (TILES_COUNT + 1);
    }
  }
}

void updateInput() {
  state.input.clear();
  const Uint8* keys = SDL_GetKeyboardState(nullptr);
  if (keys[SDL_SCANCODE_W]) {
    state.input.vertical += 1;
  }
  if (keys[SDL_SCANCODE_S]) {
    state.input.vertical -= 1;
  }
  if (keys[SDL_SCANCODE_A]) {
    state.input.horizontal -= 1;
  }
  if (keys[SDL_SCANCODE_D]) {
    state.input.horizontal += 1;
  }
}

void updatePlayer(float dt) {
  auto& player = state.player;
  const auto& input = state.input;

  if (std::abs(state.input.horizontal) > 0.0f) {
    player.angle += player.angular_speed * input.horizontal * dt;
  }

  player.pos.x += cos(player.angle) * input.vertical * player.speed * dt;
  player.pos.y += sin(player.angle) * input.vertical * player.speed * dt;

  player.pos = clamp(player.pos, {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT});
}

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printSdlError("Unable to init SDL");
    exit(EXIT_FAILURE);
  }

  SDL_Window* window = SDL_CreateWindow("Raycast 3D", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * 2,
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
    const float dt = state.clock.deltaTime();
    updateInput();
    updatePlayer(dt);
    render(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}
