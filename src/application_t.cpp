//
// Created by edwin on 17-06-18.
//

#include "application_t.h"

#include "game_manager_t.h"

bool application_t::init() {
    this->window.create(sf::VideoMode(400, 256), "My window");

    external_game_manager.reset(new game_manager_t(this->window));

    srand(time(NULL));

    return true;
}