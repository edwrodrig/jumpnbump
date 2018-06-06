//
// Created by edwin on 02-06-18.
//

#ifndef JUMPNBUMP_BAN_MAP_H
#define JUMPNBUMP_BAN_MAP_H

#include <tuple>
#include "map_position_t.h"

const unsigned int BAN_VOID	= 0;
const unsigned int BAN_SOLID = 1;
const unsigned int BAN_WATER = 2;
const unsigned int BAN_ICE = 3;
const unsigned int BAN_SPRING =	4;

/**
 * The ban map is a two dimensional map of blocks.
 * A map in the game as 22x17 blocks. A block is about the size of a bunny.
 * It seems that the ban map has information about objects on the map to calculate valid positions of bunnies.
 * For example to not respawn a death bunny in a position where is already something, like a floor block, water or someything else.
 * If you want to get a ban map pos from pizel use {@see ban_map_t::get_by_pixel()}
 */
class ban_map_t {

public:
    /**
     * These should be dynamic not harcoded
     */
    static const int WIDTH = 22;
    static const int HEIGHT = 17;

public:

    unsigned int& get(const map_position_t& position);

    const unsigned int& get(const map_position_t& position) const;

    /**
    * Get the value of a ban map coord using pixel
    * @param x
    * @param y
    * @return
    */
    [[deprecated]]
    unsigned int& get(int x, int y);
    [[deprecated]]
    unsigned int& get_by_pixel(int x, int y);
    [[deprecated]]
    const unsigned int& get_by_pixel(int x, int y) const;
    bool is_pixel_in_water(int x, int y) const;
    map_position_t get_random_position() const;

    /**
     * Jusdt return a position with no BAN_VOID map
     * @return
     */
    map_position_t get_random_available_position() const;

    /**
     * Return a position suitable for a bunny new bunny in the floor
     * @return
     */
    map_position_t get_random_available_floor_position() const;

    unsigned int map[ban_map_t::HEIGHT][ban_map_t::WIDTH];

    int get_width() const { return WIDTH; }
    int get_height() const { return HEIGHT; }
};

/**
 * This must replace completely ban_map.
 * Just keep for migration propuses
 */
extern ban_map_t ban_map;


#endif //JUMPNBUMP_BAN_MAP_H
