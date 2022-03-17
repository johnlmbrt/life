//
// Created by John
// 31st of January, 2022
//
// Console Draw Functions

#pragma once

#include <string_view>

#include "ncurses.h"

namespace console::draw {

// Text Drawing Functions -----------------------
auto left_text(int y, std::string_view text) -> void { mvwprintw(stdscr, y, 1, "%s", text.begin()); }

auto center_text(int y, std::string_view text) -> void {
  auto max_x = getmaxx(stdscr) - 1;
  auto half_x = max_x >> 1;
  auto offset = text.size() >> 1;
  mvwprintw(stdscr, y, half_x - offset, "%s", text.begin());
}

auto right_text(int y, std::string_view text) -> void {
  auto max_x = getmaxx(stdscr) - 1;
  mvwprintw(stdscr, y, max_x - text.size(), "%s", text.begin());
}
// ----------------------------------------------

// Border Drawing Functions ---------------------
auto horizontal_top_border(int y) -> void {
  int begin_x = 0;
  int end_x = getmaxx(stdscr) - 1;
  mvwaddch(stdscr, y, begin_x, ACS_ULCORNER);
  mvwhline(stdscr, y, begin_x + 1, ACS_HLINE, end_x - begin_x + 1);
  mvwaddch(stdscr, y, end_x, ACS_URCORNER);
}

auto horizontal_border(int y) -> void {
  int begin_x = 0;
  int end_x = getmaxx(stdscr) - 1;
  mvwaddch(stdscr, y, begin_x, ACS_LTEE);
  mvwhline(stdscr, y, begin_x + 1, ACS_HLINE, end_x - begin_x + 1);
  mvwaddch(stdscr, y, end_x, ACS_RTEE);
}

auto horizontal_bottom_border(int y) -> void {
  int begin_x = 0;
  int end_x = getmaxx(stdscr) - 1;
  mvwaddch(stdscr, y, begin_x, ACS_LLCORNER);
  mvwhline(stdscr, y, begin_x + 1, ACS_HLINE, end_x - begin_x + 1);
  mvwaddch(stdscr, y, end_x, ACS_LRCORNER);
}
// ----------------------------------------------

// Button Drawing Functions ---------------------
auto button_top_border(int y, int begin_x, int end_x) -> void {
  mvwaddch(stdscr, y, begin_x, ACS_ULCORNER);
  mvwhline(stdscr, y, begin_x + 1, ACS_HLINE, end_x - begin_x - 1);
  mvwaddch(stdscr, y, end_x, ACS_URCORNER);
}

auto button_bottom_border(int y, int begin_x, int end_x) -> void {
  mvwaddch(stdscr, y, begin_x, ACS_LLCORNER);
  mvwhline(stdscr, y, begin_x + 1, ACS_HLINE, end_x - begin_x - 1);
  mvwaddch(stdscr, y, end_x, ACS_LRCORNER);
}

auto button_left_right_border(int begin_y, int end_y, int begin_x, int end_x) -> void {
  for (int y = begin_y; y <= end_y; ++y) {
    mvwaddch(stdscr, y, begin_x, ACS_VLINE);
    mvwaddch(stdscr, y, end_x, ACS_VLINE);
  }
}

auto button_border(int begin_y, int end_y, int begin_x, int end_x) -> void {
  button_top_border(begin_y, begin_x, end_x);
  button_left_right_border(begin_y + 1, end_y - 1, begin_x, end_x);
  button_bottom_border(end_y, begin_x, end_x);
}
// ----------------------------------------------

}  // namespace console::draw