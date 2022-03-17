//
// Created by John
// 14th of March, 2022
//
// Display Mouse Functions

#pragma once

#include "SDL.h"

namespace display::mouse {

struct mouse_t {
  int x{0}, y{0};
  uint32_t bstate;
  std::vector<bool> pressed;

  mouse_t() : pressed(3, false) {}
};

static const std::array<uint32_t, 3> button_masks{SDL_BUTTON_LMASK, SDL_BUTTON_MMASK, SDL_BUTTON_RMASK};

auto update(mouse_t& mouse) -> void {
  mouse.bstate = SDL_GetMouseState(&mouse.x, &mouse.y);

  for (int button = 0; const auto button_mask : button_masks) {
    mouse.pressed[button] = mouse.bstate & button_mask;
    ++button;
  }
}
auto reset(mouse_t& mouse) -> void { mouse.bstate = 0; }
auto position(const mouse_t& mouse) -> std::pair<int, int> { return std::pair(mouse.x, mouse.y); }

auto left_press(const mouse_t& mouse) -> bool { return mouse.bstate & button_masks[0]; }
auto middle_press(const mouse_t& mouse) -> bool { return mouse.bstate & button_masks[1]; }
auto right_press(const mouse_t& mouse) -> bool { return mouse.bstate & button_masks[2]; }

auto left_pressed(const mouse_t& mouse) -> bool { return mouse.pressed[0]; }
auto middle_pressed(const mouse_t& mouse) -> bool { return mouse.pressed[1]; }
auto right_pressed(const mouse_t& mouse) -> bool { return mouse.pressed[2]; }

}  // namespace display