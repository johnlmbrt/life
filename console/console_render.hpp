//
// Created by John
// 31st of January, 2022
//
// Console Render Functions

#pragma once

#include <string_view>

#include "console.hpp"
#include "console_draw.hpp"

#undef border

namespace console::render {

auto erase(console_t& console) -> void {
  werase(stdscr);

  console.border = false;

  // update where the next render should occur --
  console.render_x = 0;
  console.render_y = 0;
  // --------------------------------------------
}

auto border(console_t& console) -> void {
  wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);

  console.border = true;

  // update where the next render should occur --
  console.render_x = console.border ? 1 : 0;
  console.render_y = console.border ? 1 : 0;
  // --------------------------------------------
}

auto header(console_t& console, std::string_view left, std::string_view center, std::string_view right) -> void {
  // header_top_border
  console::draw::horizontal_top_border(0);

  // header_clear
  mvwhline(stdscr, 1, 1, ' ', getmaxx(stdscr) - 2);

  // header_left_border
  mvwaddch(stdscr, 1, 0, ACS_VLINE);

  /// header_left
  if (!left.empty()) console::draw::left_text(1, left);

  // header_right
  if (!right.empty()) console::draw::right_text(1, right);

  // header_center
  if (!center.empty()) console::draw::center_text(1, center);

  // header_right_border
  mvwaddch(stdscr, 1, getmaxx(stdscr) - 1, ACS_VLINE);

  // header_bottom_border
  console.border ? console::draw::horizontal_border(2) : console::draw::horizontal_bottom_border(2);

  // update where the next render should occur --
  console.render_x = console.border ? 1 : 0;
  console.render_y = 3;
  // --------------------------------------------
}

auto footer(console_t& console, std::string_view left, std::string_view center, std::string_view right) -> void {
  // footer_bottom_border
  console::draw::horizontal_bottom_border(getmaxy(stdscr) - 1);

  // footer_clear
  mvwhline(stdscr, getmaxy(stdscr) - 2, 1, ' ', getmaxx(stdscr) - 2);

  // footer_left_border
  mvwaddch(stdscr, getmaxy(stdscr) - 2, 0, ACS_VLINE);

  // footer_left
  if (!left.empty()) console::draw::left_text(getmaxy(stdscr) - 2, left);

  // footer_right
  if (!right.empty()) console::draw::right_text(getmaxy(stdscr) - 2, right);

  // footer_center
  if (!center.empty()) console::draw::center_text(getmaxy(stdscr) - 2, center);

  // footer_right_border
  mvwaddch(stdscr, getmaxy(stdscr) - 2, getmaxx(stdscr) - 1, ACS_VLINE);

  // footer_top_border
  console.border ? console::draw::horizontal_border(getmaxy(stdscr) - 3) : console::draw::horizontal_top_border(getmaxy(stdscr) - 3);

  // update where the next render should occur --
  console.render_x = 0;
  console.render_y = getmaxy(stdscr);
  // --------------------------------------------
}

auto button(console_t& console, std::string_view label) -> bool {
  int begin_y = console.render_y;
  int end_y = begin_y + 2;

  int begin_x = console.render_x;
  int end_x = begin_x + static_cast<int>(label.size()) + 1;

  console::draw::button_border(begin_y, end_y, begin_x, end_x);
  // draw label
  mvwprintw(stdscr, begin_y + 1, begin_x + 1, "%s", label.begin());

  // update where the next render should occur --
  console.render_x = end_x + 1;
  console.render_y = begin_y;
  // --------------------------------------------

  auto [mouse_x, mouse_y] = console::mouse::position(console.mouse);
  bool inside_x = begin_x <= mouse_x && mouse_x <= end_x;
  bool inside_y = begin_y <= mouse_y && mouse_y <= end_y;
  return inside_x && inside_y && console::mouse::left_press(console.mouse);
}

auto text(console_t& console, std::string_view text) -> void {
  mvwprintw(stdscr, console.render_y, console.render_x, "%s", text.begin());

  // update where the next render should occur --
  console.render_x += text.size() + 1;
  // --------------------------------------------
}

auto line(console_t& console, std::string_view line) -> void {
  mvwprintw(stdscr, console.render_y, console.render_x, "%s", line.begin());

  // update where the next render should occur --
  console.render_x = console.border ? 1 : 0;
  ++console.render_y;
  // --------------------------------------------
}

auto divider(console_t& console) -> void {
  console::draw::horizontal_border(console.render_y);
  if (!console.border) mvwaddch(stdscr, console.render_y, 0, ACS_HLINE);
  if (!console.border) mvwaddch(stdscr, console.render_y, getmaxx(stdscr) - 1, ACS_HLINE);

  // update where the next render should occur --
  ++console.render_y;
  // --------------------------------------------
}

}  // namespace console::render