//
// Created by John
// 9th of March, 2022
//

#include <map>

#include <array>
#include <random>

#include "czmq.h"
#include "fmt/chrono.h"
#include "fmt/format.h"

#include "console.hpp"
#include "console_render.hpp"
#include "display.hpp"

#include "grid.hpp"

using color_t = std::array<uint8_t, 3>;

struct random_color_generator_t {
  std::random_device device;
  std::mt19937 generator;
  std::uniform_int_distribution<uint8_t> distributor;

  random_color_generator_t() : generator(device()) {}

  auto generate() -> color_t {
    color_t color;
    color[0] = distributor(generator);
    color[1] = distributor(generator);
    color[2] = distributor(generator);
    return color;
  }
};

auto average_color_component(const uint8_t c1, const uint8_t c2) -> uint8_t { return static_cast<uint8_t>((static_cast<int>(c1) + static_cast<int>(c2)) / 2); }

auto average_colors(const color_t& c1, const color_t& c2) -> color_t {
  color_t color;
  color[0] = average_color_component(c1[0], c2[0]);
  color[1] = average_color_component(c1[1], c2[1]);
  color[2] = average_color_component(c1[2], c2[2]);
  return color;
}

struct cell_t {
  std::pair<int, int> coord;
  std::array<uint8_t, 3> color;

  auto operator<=>(const cell_t&) const = default;
};

struct program_t {
  console::console_t& console;
  display::display_t& display;

  grid_t grid;

  random_color_generator_t color_generator;
  std::vector<cell_t> cells;

  program_t(console::console_t& console, display::display_t& display) : console(console), display(display) {
    int window_width, window_height;
    SDL_GetWindowSize(display.window, &window_width, &window_height);

    grid.subdivisions = 3;
    grid.cell_size = window_width >> grid.subdivisions;
  }

  bool running{true};
  bool updating{false};

  bool mouse_left_pressed{false};
  bool mouse_right_pressed{false};

  bool adding_cells{false};
  bool removing_cells{false};

  auto run() -> void {
    zloop_t* loop = zloop_new();

    int update_id = zloop_timer(loop, 1, 0, program_t::loop_update, this);

    // zloop_start returns -1 when a handler cancels the loop
    // zloop_start returns  0 when the loop is interrupted
    bool interrupted = zloop_start(loop) == 0;
    while (interrupted) interrupted = zloop_start(loop) == 0;

    zloop_destroy(&loop);
  }

  auto update_console() -> void {
    chtype input = wgetch(stdscr);
    if (input == '\n') running = false;
    if (input == 27) running = false;

    if (input == KEY_MOUSE)
      console::mouse::update(console.mouse);
    else
      console::mouse::reset(console.mouse);
  }

  auto update_cells() -> void {
    auto previous_cells = cells;
    cells.clear();

    auto check_neighbour = [&](int x, int y) -> bool {
      auto coord_pred = [&x, &y](const cell_t& cell) -> bool { return x == cell.coord.first && y == cell.coord.second; };
      auto found_cell = std::ranges::find_if(previous_cells, coord_pred);
      return found_cell != std::end(previous_cells);
    };

    std::array<std::pair<int, int>, 8> deltas{std::make_pair(0, -1), std::make_pair(1, -1), std::make_pair(1, 0),  std::make_pair(1, 1),
                                              std::make_pair(0, 1),  std::make_pair(-1, 1), std::make_pair(-1, 0), std::make_pair(-1, -1)};

    std::map<std::pair<int, int>, int> candidate_coords;
    std::map<std::pair<int, int>, color_t> candidate_colors;

    for (const auto& cell : previous_cells) {
      int neighbour_count = 0;

      int x = cell.coord.first;
      int y = cell.coord.second;
      for (const auto& [delta_x, delta_y] : deltas) {
        int coord_x = x + delta_x;
        int coord_y = y + delta_y;
        if (check_neighbour(coord_x, coord_y)) {
          ++neighbour_count;
        } else {
          auto candidate_coord = std::make_pair(coord_x, coord_y);
          if (candidate_coords.contains(candidate_coord)) {
            ++candidate_coords[candidate_coord];
            candidate_colors[candidate_coord] = average_colors(candidate_colors[candidate_coord], cell.color);
          } else {
            candidate_coords.insert({candidate_coord, 1});
            candidate_colors.insert({candidate_coord, cell.color});
          }
        }
      }

      // if cell has 2 neighbours, cell lives on
      // if cell has 3 neighoburs, cell lives on
      if (neighbour_count >= 2 && neighbour_count <= 3) cells.push_back(cell);
    }

    // and dead cell with 3 live neighbours becomes a live cell
    for (const auto& [coord, count] : candidate_coords) {
      // if (count == 3) cells.emplace_back(coord, color_generator.generate());
      if (count == 3) cells.emplace_back(coord, candidate_colors[coord]);
    }
  }

  auto add_cell(int x, int y) -> bool {
    auto coord_pred = [&x, &y](const cell_t& cell) -> bool { return cell.coord.first == x && cell.coord.second == y; };
    auto found_cell = std::ranges::find_if(cells, coord_pred);
    if (found_cell == std::end(cells)) {
      cell_t cell{{x, y}, color_generator.generate()};
      cells.push_back(cell);
      return true;
    }
    return false;
  }

  auto remove_cell(int x, int y) -> bool {
    auto coord_pred = [&x, &y](const cell_t& cell) -> bool { return cell.coord.first == x && cell.coord.second == y; };
    auto erased = std::erase_if(cells, coord_pred);
    return erased != 0;
  }

  enum struct toggle_action_e { add, remove };

  auto toggle_cell(int x, int y) -> toggle_action_e {
    auto coord_pred = [&x, &y](const cell_t& cell) -> bool { return cell.coord.first == x && cell.coord.second == y; };
    auto found_cell = std::ranges::find_if(cells, coord_pred);
    if (found_cell != std::end(cells)) {
      std::erase(cells, *found_cell);
      return toggle_action_e::remove;
    } else {
      cell_t cell{{x, y}, color_generator.generate()};
      cells.push_back(cell);
      return toggle_action_e::add;
    }
  }

  auto toggle_rpentomino(int x, int y) -> void {
    toggle_cell(x, y);
    toggle_cell(x, y - 1);
    toggle_cell(x, y + 1);
    toggle_cell(x - 1, y);
    toggle_cell(x + 1, y - 1);
  }

  auto update_display() -> void {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;

      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN) running = false;
        if (event.key.keysym.sym == SDLK_c) updating = true;
        if (event.key.keysym.sym == SDLK_p) updating = false;

        if (event.key.keysym.sym == SDLK_n) update_cells();

        if (event.key.keysym.sym == SDLK_r) {
          int coord_x = display_space_grid_coord_x(display, grid, grid.cursor_x);
          int coord_y = display_space_grid_coord_y(display, grid, grid.cursor_y);
          toggle_rpentomino(coord_x, coord_y);
        }
      }

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          int coord_x = display_space_grid_coord_x(display, grid, event.button.x);
          int coord_y = display_space_grid_coord_y(display, grid, event.button.y);

          auto action = toggle_cell(coord_x, coord_y);
          if (action == toggle_action_e::add) adding_cells = true;
          else if (action == toggle_action_e::remove) removing_cells = true;
        }
      }

      if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          adding_cells = false;
          removing_cells = false;
        }
      }

      if (event.type == SDL_MOUSEMOTION) {
        if (event.motion.state & SDL_BUTTON_LMASK) {
          int coord_x = display_space_grid_coord_x(display, grid, event.motion.x);
          int coord_y = display_space_grid_coord_y(display, grid, event.motion.y);

          if (adding_cells)
            add_cell(coord_x, coord_y);
          else if (removing_cells)
            remove_cell(coord_x, coord_y);
        }

        if (event.motion.state & SDL_BUTTON_RMASK) modify_grid_offset(grid.offset, event.motion.xrel, event.motion.yrel);
      }

      if (event.type == SDL_MOUSEWHEEL) modify_grid_cell_size(grid, event.wheel.y);

      update_grid(event, display, grid);
    }
  }

  auto update() -> void {
    update_console();
    update_display();

    if (updating) update_cells();
  }

  auto render_console() -> void {
    console::render::erase(console);
    console::render::border(console);
    console::render::header(console, "", "Life", "");

    console::render::line(console, "Display");
    int window_width, window_height;
    SDL_GetWindowSize(display.window, &window_width, &window_height);
    console::render::line(console, fmt::format("display.width: {}", window_width));
    console::render::line(console, fmt::format("display.height: {}", window_height));
    console::render::divider(console);

    console::render::line(console, "Grid");
    console::render::line(console, fmt::format("grid.subdivisions: {}", grid.subdivisions));
    console::render::line(console, fmt::format("grid.max_subdivisions: {}", grid_max_subdivisions(display, grid)));
    console::render::line(console, fmt::format("grid.cell_width: {}", grid.cell_width));
    console::render::line(console, fmt::format("grid.cell_height: {}", grid.cell_height));
    console::render::line(console, fmt::format("grid.cell_size: {}", grid.cell_size));
    console::render::line(console, fmt::format("grid.offset.x: {}", grid.offset.x));
    console::render::line(console, fmt::format("grid.offset.y: {}", grid.offset.y));
    console::render::line(console, fmt::format("grid.cursor_x: {}", grid.cursor_x));
    console::render::line(console, fmt::format("grid.cursor_y: {}", grid.cursor_y));
    console::render::divider(console);

    console::render::line(console, "Life");
    console::render::line(console, fmt::format("updating: {}", updating));
    console::render::line(console, fmt::format("mouse_left_pressed: {}", mouse_left_pressed));
    console::render::line(console, fmt::format("mouse_right_pressed: {}", mouse_right_pressed));
    console::render::line(console, fmt::format("cells.size: {}", cells.size()));
    console::render::divider(console);
  }

  auto render_cells() -> void {
    int radius = half(grid.cell_size);
    for (const auto& cell : cells) {
      int display_x = display_space_grid_coord_origin_x(display, grid, cell.coord.first);
      int display_y = display_space_grid_coord_origin_y(display, grid, cell.coord.second);

      SDL_Rect rect;
      rect.x = display_x - radius;
      rect.y = display_y - radius;
      rect.w = grid.cell_size;
      rect.h = grid.cell_size;

      SDL_SetRenderDrawColor(display.renderer, cell.color[0], cell.color[1], cell.color[2], SDL_ALPHA_OPAQUE);
      // SDL_RenderDrawLine(display.renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
      // SDL_RenderDrawLine(display.renderer, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y);
      // SDL_RenderDrawRect(display.renderer, &rect);
      SDL_RenderFillRect(display.renderer, &rect);
    }
  }

  auto render_display() -> void {
    SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(display.renderer);

    // render_grid(display, grid);

    // render_selected_coords();
    render_cells();

    SDL_RenderPresent(display.renderer);
  }

  auto render() -> void {
    render_console();
    render_display();
  }

  static auto loop_update(zloop_t* loop, int timer_id, void* arg) -> int {
    auto& program = *reinterpret_cast<program_t*>(arg);
    program.update();
    program.render();
    return program.running ? 0 : -1;
  }
};

auto main(int argc, char** argv) -> int {
  console::console_t console;
  display::display_t display("Life");
  SDL_SetWindowSize(display.window, 640, 480);

  program_t program(console, display);
  program.run();
  return 0;
}
