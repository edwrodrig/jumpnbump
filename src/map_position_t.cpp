//
// Created by edwin on 05-06-18.
//

#include "map_position_t.h"
#include "screen_position_t.h"
#include "position_t.h"

map_position_t::map_position_t(const position_t& position) : map_position_t{screen_position_t{position}} {}
map_position_t::map_position_t(const screen_position_t& position) : map_position_t{position.x, position.y}{}

map_position_t map_position_t::operator+(const map_position_t &position) const {
    return map_position_t{
            this->x + position.x,
            this->y + position.y
    };
}