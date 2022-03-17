//
// Created by John
// 31st of January, 2022
//
// Console Initialisation Functions

#pragma once

#include "ncurses.h"

namespace console::initialise {

auto enable_stdscr() -> void {
  initscr();
  use_default_colors();
  start_color();
  cbreak();
  noecho();
  curs_set(0);
  set_escdelay(25);
  wtimeout(stdscr, 0);
  keypad(stdscr, true);
  nodelay(stdscr, true);
}

auto disable_stdscr() -> void { endwin(); }

auto enable_mouse() -> void {
  mouseinterval(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
  printf("\033[?1003h\n");

  MEVENT mouse_event;
  mouse_event.x = 0;
  mouse_event.y = 0;
  mouse_event.bstate = REPORT_MOUSE_POSITION;
  ungetmouse(&mouse_event);
}

auto disable_mouse() -> void { printf("\033[?1003l\n"); }

}  // namespace console::initialise