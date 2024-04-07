#ifndef GAME_HPP
#define GAME_HPP

#include "core_include.hpp"

void on_start();
void on_update();
bool on_event(std::shared_ptr<spark::event> event);

void register_functions();

#endif
