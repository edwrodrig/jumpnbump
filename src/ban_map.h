//
// Created by edwin on 02-06-18.
//

#ifndef JUMPNBUMP_BAN_MAP_H
#define JUMPNBUMP_BAN_MAP_H

#include <tuple>
#include <vector>
#include "map_position_t.h"

/**
 * The ban map is a two dimensional map of blocks.
 * A map in the game as 22x17 blocks. A block is about the size of a bunny.
 * It seems that the ban map has information about objects on the map to calculate valid positions of bunnies.
 * For example to not respawn a death bunny in a position where is already something, like a floor block, water or someything else.
 * If you want to get a ban map pos from pizel use {@see ban_map_t::get_by_pixel()}
 */
class ban_map_t {

public:

    enum class Type {
        VOID = 0,
        SOLID = 1,
        WATER = 2,
        ICE = 3,
        SPRING = 4
    };

    int width;
    int height;

    bool read_from_file(const std::string& filename);

    Type get(const map_position_t& position) const;


    bool is_in_water(const screen_position_t& position) const;
    map_position_t get_random_position() const;

    /**
     * Jusdt return a position with no VOID map
     * @return
     */
    map_position_t get_random_available_position() const;

    /**
     * Return a position suitable for a bunny new bunny in the floor
     * @return
     */
    map_position_t get_random_available_floor_position() const;

    std::vector<std::vector<ban_map_t::Type>> map{0};

    int get_width() const { return this->width; }
    int get_height() const { return this->height; }
    void flip();
};

extern ban_map_t ban_map;


#endif //JUMPNBUMP_BAN_MAP_H
