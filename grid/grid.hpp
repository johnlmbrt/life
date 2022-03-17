//
// Created by John
// 26th of February, 2022
//

#include <algorithm>
#include <vector>

#include "display.hpp"

template <typename N>
auto half(N n) -> N {
  return n >> 1;
}

struct grid_offset_t {
  int x;
  int y;

  friend auto reset(grid_offset_t& offset) -> void {
    offset.x = 0;
    offset.y = 0;
  }
};

struct grid_t {
  int subdivisions{1};

  int cell_width{0};
  int cell_height{0};

  int cell_size{0};

  grid_offset_t offset;

  int cursor_x{0};
  int cursor_y{0};
};

auto grid_max_subdivisions(const display::display_t& display, const grid_t& grid) -> int {
  int window_width, window_height;
  SDL_GetWindowSize(display.window, &window_width, &window_height);

  int width_subdivisions{0};
  int cell_width = window_width;
  while (cell_width >>= 1) ++width_subdivisions;

  int height_subdivisions{0};
  int cell_height = window_height;
  while (cell_height >>= 1) ++height_subdivisions;

  return std::min(width_subdivisions, height_subdivisions);
}

auto grid_space_grid_coord_origin_x(const grid_t& grid, const int coord_x) -> int { return grid.cell_size * coord_x; }

auto grid_space_grid_coord_origin_y(const grid_t& grid, const int coord_y) -> int { return grid.cell_size * coord_y; }

auto display_space_grid_coord_origin_x(const display::display_t& display, const grid_t& grid, const int coord_x) -> int {
  int window_width;
  SDL_GetWindowSize(display.window, &window_width, nullptr);
  return (coord_x * grid.cell_size) + grid.offset.x + half(window_width);
}

auto display_space_grid_coord_origin_y(const display::display_t& display, const grid_t& grid, const int coord_y) -> int {
  int window_height;
  SDL_GetWindowSize(display.window, nullptr, &window_height);
  return (coord_y * grid.cell_size) + grid.offset.y + half(window_height);
}

auto display_space_grid_coord_x(const display::display_t& display, grid_t& grid, int x) -> int {
  int window_width;
  SDL_GetWindowSize(display.window, &window_width, nullptr);

  int cell_width = grid.cell_size;
  int adjusted_x = x - grid.offset.x - half(window_width) + half(cell_width);
  int coord_x = adjusted_x / cell_width;
  return adjusted_x < 0 ? --coord_x : coord_x;
}

auto display_space_grid_coord_y(const display::display_t& display, grid_t& grid, int y) -> int {
  int window_height;
  SDL_GetWindowSize(display.window, nullptr, &window_height);

  int cell_height = grid.cell_size;
  int adjusted_y = y - grid.offset.y - half(window_height) + half(cell_height);
  int coord_y = adjusted_y / cell_height;
  return adjusted_y < 0 ? --coord_y : coord_y;
}

auto increment_grid_subdivisions(const display::display_t& display, grid_t& grid) -> void {
  if (grid.subdivisions < grid_max_subdivisions(display, grid)) ++grid.subdivisions;
}

auto decrement_grid_subdivisions(const display::display_t& display, grid_t& grid) -> void {
  if (grid.subdivisions > 1) --grid.subdivisions;
}

auto reset_grid_offset(grid_offset_t& offset) -> void {
  offset.x = 0;
  offset.y = 0;
}

auto modify_grid_offset(grid_offset_t& offset, int delta_x, int delta_y) -> void {
  offset.x += delta_x;
  offset.y += delta_y;
}

auto modify_grid_cell_wdith_height(grid_t& grid, int delta_width, int delta_height) -> void {
  grid.cell_width += delta_width;
  grid.cell_height += delta_height;
}

auto modify_grid_cell_size(grid_t& grid, int delta_size) -> void {
  int new_size = grid.cell_size + delta_size;
  if (new_size > 0) grid.cell_size = new_size;
}

auto update_grid(SDL_Event& event, const display::display_t& display, grid_t& grid) -> void {
  if (event.type == SDL_MOUSEMOTION) {
    grid.cursor_x = event.motion.x;
    grid.cursor_y = event.motion.y;
  } else if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      grid.cursor_x = event.button.x;
      grid.cursor_y = event.button.y;

      // add/remove coord in grid.selected
      // int coord_x = display_space_grid_coord_x(display, grid, event.button.x);
      // int coord_y = display_space_grid_coord_y(display, grid, event.button.y);
      // toggle_selected_coord(grid, coord_x, coord_y);
    }
  } else if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT) {
      modify_grid_offset(grid.offset, grid.cell_size, 0);
    } else if (event.key.keysym.sym == SDLK_RIGHT) {
      modify_grid_offset(grid.offset, -grid.cell_size, 0);
    } else if (event.key.keysym.sym == SDLK_UP) {
      modify_grid_offset(grid.offset, 0, grid.cell_size);
    } else if (event.key.keysym.sym == SDLK_DOWN) {
      modify_grid_offset(grid.offset, 0, -grid.cell_size);
    } else if (event.key.keysym.sym == SDLK_SPACE) {
      reset_grid_offset(grid.offset);
      // reset(grid.offset);
    } else if (event.key.keysym.sym == SDLK_KP_PLUS) {
      modify_grid_cell_size(grid, 10);
    } else if (event.key.keysym.sym == SDLK_KP_MINUS) {
      modify_grid_cell_size(grid, -10);
    }
  }
}

auto render_grid(display::display_t& display, const grid_t& grid) -> void {
  int window_width, window_height;
  SDL_GetWindowSize(display.window, &window_width, &window_height);

  int begin_x = 0;
  int half_x = half(window_width);
  int end_x = window_width;

  int begin_y = 0;
  int half_y = half(window_height);
  int end_y = window_height;

  SDL_SetRenderDrawColor(display.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  // int cell_width = window_width >> grid.subdivisions;
  // int cell_height = window_height >> grid.subdivisions;

  // int cell_width = grid.cell_width;
  // int cell_height = grid.cell_height;

  int cell_width = grid.cell_size;
  int cell_height = grid.cell_size;

  // draw vertical grid cell lines
  int x = half_x - half(cell_width) + grid.offset.x;
  while (x >= begin_x) {
    SDL_RenderDrawLine(display.renderer, x, begin_y, x, end_y);
    x -= cell_width;
  }
  x = half_x + half(cell_width) + grid.offset.x;
  while (x < end_x) {
    SDL_RenderDrawLine(display.renderer, x, begin_y, x, end_y);
    x += cell_width;
  }

  // draw horizontal grid cell lines
  int y = half_y - half(cell_height) + grid.offset.y;
  while (y >= begin_y) {
    SDL_RenderDrawLine(display.renderer, begin_x, y, end_x, y);
    y -= cell_height;
  }
  y = half_y + half(cell_height) + grid.offset.y;
  while (y < end_y) {
    SDL_RenderDrawLine(display.renderer, begin_x, y, end_x, y);
    y += cell_height;
  }

  // draw grid cursor
  // int cursor_radius = 10;
  // SDL_RenderDrawLine(display.static_cast<int>(grid.cell)renderer, grid.cursor_x - cursor_radius, grid.cursor_y - cursor_radius, grid.cursor_x + cursor_radius, grid.cursor_y + cursor_radius);
  // SDL_RenderDrawLine(display.renderer, grid.cursor_x - cursor_radius, grid.cursor_y + cursor_radius, grid.cursor_x + cursor_radius, grid.cursor_y - cursor_radius);

  // draw selected grid cells
  // for (const auto& [x, y] : grid.selected) {
  //   int display_x = display_space_grid_coord_origin_x(display, grid, x);
  //   int display_y = display_space_grid_coord_origin_y(display, grid, y);
  //   SDL_RenderDrawLine(display.renderer, display_x - cursor_radius, display_y - cursor_radius, display_x + cursor_radius, display_y + cursor_radius);
  //   SDL_RenderDrawLine(display.renderer, display_x - cursor_radius, display_y + cursor_radius, display_x + cursor_radius, display_y - cursor_radius);
  // }
}