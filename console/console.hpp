//
// Created by John
// 26th of January, 2022
//
// Console Library

#pragma once

#include "console_initialise.hpp"
#include "console_mouse.hpp"

namespace console {

struct console_t {
  mouse::mouse_t mouse;

  console_t() {
    initialise::enable_stdscr();
    initialise::enable_mouse();
  }

  ~console_t() {
    initialise::disable_mouse();
    initialise::disable_stdscr();
  }

  bool border{false};
  
  int render_x;
  int render_y;
};

}  // namespace console
