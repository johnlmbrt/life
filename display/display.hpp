//
// Created by John
// 11th of February, 2022
//
// Display Library

#pragma once

#include <string_view>

#include "SDL.h"

#include "display_mouse.hpp"

namespace display {

struct display_t {
  SDL_Window* window;
  SDL_Renderer* renderer;

  mouse::mouse_t mouse;

  display_t() = default;
  display_t(std::string_view title) {
    window = SDL_CreateWindow(title.begin(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 100, 100, SDL_WINDOW_RESIZABLE);
    if (window != nullptr) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  }

  ~display_t() {
    if (renderer != nullptr) SDL_DestroyRenderer(renderer);
    if (window != nullptr) SDL_DestroyWindow(window);
  }
};

}  // namespace display