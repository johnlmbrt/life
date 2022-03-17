//
// Created by John
// 5th of February, 2022
//
// Console Mouse Functions

#pragma once

#include <array>
#include <vector>

#include "ncurses.h"

namespace console::mouse {

struct mouse_t {
  MEVENT event;
  std::vector<bool> pressed;

  mouse_t() : pressed(3, false) {}
};

static const std::array<std::pair<mmask_t, mmask_t>, 3> mouse_buttons{std::make_pair(BUTTON1_PRESSED, BUTTON1_RELEASED), std::make_pair(BUTTON2_PRESSED, BUTTON2_RELEASED),
                                                                      std::make_pair(BUTTON3_PRESSED, BUTTON3_RELEASED)};

auto update(mouse_t& mouse) -> void {
  getmouse(&mouse.event);

  for (int button = 0; const auto [pressed, released] : mouse_buttons) {
    if (mouse.event.bstate & pressed) mouse.pressed[button] = true;
    if (mouse.event.bstate & released) mouse.pressed[button] = false;
    ++button;
  }
}
auto reset(mouse_t& mouse) -> void { mouse.event.bstate = 0; }
auto position(mouse_t& mouse) -> std::pair<int, int> { return std::pair(mouse.event.x, mouse.event.y); }

auto left_press(const mouse_t& mouse) -> bool { return mouse.event.bstate & BUTTON1_PRESSED; }
auto middle_press(const mouse_t& mouse) -> bool { return mouse.event.bstate & BUTTON2_PRESSED; }
auto right_press(const mouse_t& mouse) -> bool { return mouse.event.bstate & BUTTON3_PRESSED; }

auto left_pressed(const mouse_t& mouse) -> bool { return mouse.pressed[0]; }
auto middle_pressed(const mouse_t& mouse) -> bool { return mouse.pressed[1]; }
auto right_pressed(const mouse_t& mouse) -> bool { return mouse.pressed[2]; }

}  // namespace console::mouse