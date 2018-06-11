//
// Created by edwin on 02-06-18.
//

#include "sdl/gfx.h"
#include "gob_t.h"
#include "globals.h"
#include "leftovers_t.h"
#include "screen_position_t.h"

void leftovers_t::add(const screen_position_t &position, int image, gob_t *pob_data) {
    this->pobs.emplace_back(position, image, pob_data);
}

void leftovers_t::draw() {
    int c1;

    for (c1 = this->pobs.size() - 1; c1 >= 0; c1--)
        put_pob(this->pobs[c1].position.x, this->pobs[c1].position.y, this->pobs[c1].image,
                *this->pobs[c1].pob_data, 1);

    this->pobs.clear();

}