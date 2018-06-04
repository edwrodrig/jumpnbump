//
// Created by edwin on 01-06-18.
//

#include "globals.h"
#include "player.h"
#include "ban_map.h"
#include "object_anim.h"
#include "util.h"

extern int jetpack;
extern int bunnies_in_space;
extern int pogostick;
extern int blood_is_thicker_than_water;
extern main_info_t main_info;
extern object_anim_t object_anims[8];

void serverSendKillPacket(int c1, int c2);

void player_action_left(player_t& player) {
    int s1 = 0, s2 = 0;
    int below_left, below, below_right;

    s1 = (player.x >> 16);
    s2 = (player.y >> 16);
    below_left = ban_map.get_by_pixel(s1, s2 + 16);
    below = ban_map.get_by_pixel(s1 + 8, s2 + 16);
    below_right = ban_map.get_by_pixel(s1 + 15, s2 + 16);

    if (below == BAN_ICE) {
        if (player.x_add > 0)
            player.x_add -= 1024;
        else
            player.x_add -= 768;
    } else if ((below_left != BAN_SOLID && below_right == BAN_ICE) ||
               (below_left == BAN_ICE && below_right != BAN_SOLID)) {
        if (player.x_add > 0)
            player.x_add -= 1024;
        else
            player.x_add -= 768;
    } else {
        if (player.x_add > 0) {
            player.x_add -= 16384;
            if (player.x_add > -98304L && player.in_water == 0 && below == BAN_SOLID)
                add_object(OBJ_SMOKE, (player.x >> 16) + 2 + rnd(9), (player.y >> 16) + 13 + rnd(5), 0,
                           -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
        } else
            player.x_add -= 12288;
    }
    if (player.x_add < -98304L)
        player.x_add = -98304L;
    player.direction = 1;
    if (player.anim == 0) {
        player.anim = 1;
        player.frame = 0;
        player.frame_tick = 0;
        player.image = player_anims[player.anim].frame[player.frame].image + player.direction * 9;
    }
}

void player_action_right(player_t& player) {
    int s1 = 0, s2 = 0;
    int below_left, below, below_right;

    s1 = (player.x >> 16);
    s2 = (player.y >> 16);
    below_left = ban_map.get_by_pixel(s1, s2 + 16);
    below = ban_map.get_by_pixel(s1 + 8, s2 + 16);
    below_right = ban_map.get_by_pixel(s1 + 15, s2 + 16);

    if (below == BAN_ICE) {
        if (player.x_add < 0)
            player.x_add += 1024;
        else
            player.x_add += 768;
    } else if ((below_left != BAN_SOLID && below_right == BAN_ICE) ||
               (below_left == BAN_ICE && below_right != BAN_SOLID)) {
        if (player.x_add > 0)
            player.x_add += 1024;
        else
            player.x_add += 768;
    } else {
        if (player.x_add < 0) {
            player.x_add += 16384;
            if (player.x_add < 98304L && player.in_water == 0 && below == BAN_SOLID)
                add_object(OBJ_SMOKE, (player.x >> 16) + 2 + rnd(9), (player.y >> 16) + 13 + rnd(5), 0,
                           -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
        } else
            player.x_add += 12288;
    }
    if (player.x_add > 98304L)
        player.x_add = 98304L;
    player.direction = 0;
    if (player.anim == 0) {
        player.anim = 1;
        player.frame = 0;
        player.frame_tick = 0;
        player.image = player_anims[player.anim].frame[player.frame].image + player.direction * 9;
    }
}

void steer_players(void) {
    int c1, c2;
    int s1 = 0, s2 = 0;

    update_player_actions();

    for (c1 = 0; c1 < players.size(); c1++) {

        if (players[c1].enabled == 1) {

            if (players[c1].is_alive()) {

                if (players[c1].action_left && players[c1].action_right) {
                    if (players[c1].direction == 0) {
                        if (players[c1].action_right) {
                            player_action_right(players[c1]);
                        }
                    } else {
                        if (players[c1].action_left) {
                            player_action_left(players[c1]);
                        }
                    }
                } else if (players[c1].action_left) {
                    player_action_left(players[c1]);
                } else if (players[c1].action_right) {
                    player_action_right(players[c1]);
                } else if ((!players[c1].action_left) && (!players[c1].action_right)) {
                    int below_left, below, below_right;

                    s1 = (players[c1].x >> 16);
                    s2 = (players[c1].y >> 16);

                    //TODO: log de steer
                    if ( c1 == 0 )
                        printf("Steer players %d => [%d, %d] => [%d, %d]\n", c1, players[c1].x, players[c1].y, s1, s2);
                    below_left = ban_map.get_by_pixel(s1, s2 + 16);
                    below = ban_map.get_by_pixel(s1 + 8, s2 + 16);
                    below_right = ban_map.get_by_pixel(s1 + 15, s2 + 16);
                    if (below == BAN_SOLID || below == BAN_SPRING ||
                        (((below_left == BAN_SOLID || below_left == BAN_SPRING) && below_right != BAN_ICE) ||
                         (below_left != BAN_ICE && (below_right == BAN_SOLID || below_right == BAN_SPRING)))) {
                        if (players[c1].x_add < 0) {
                            players[c1].x_add += 16384;
                            if (players[c1].x_add > 0)
                                players[c1].x_add = 0;
                        } else {
                            players[c1].x_add -= 16384;
                            if (players[c1].x_add < 0)
                                players[c1].x_add = 0;
                        }
                        if (players[c1].x_add != 0 && ban_map.get_by_pixel((s1 + 8), (s2 + 16)) == BAN_SOLID)
                            add_object(OBJ_SMOKE, (players[c1].x >> 16) + 2 + rnd(9), (players[c1].y >> 16) + 13 + rnd(5),
                                       0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                    if (players[c1].anim == 1) {
                        players[c1].anim = 0;
                        players[c1].frame = 0;
                        players[c1].frame_tick = 0;
                        players[c1].image =
                                player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                    }
                }
                if (jetpack == 0) {
                    /* no jetpack */
                    if (pogostick == 1 || (players[c1].jump_ready == 1 && players[c1].action_up)) {
                        s1 = (players[c1].x >> 16);
                        s2 = (players[c1].y >> 16);
                        if (s2 < -16)
                            s2 = -16;
                        /* jump */
                        if (ban_map.get_by_pixel(s1, (s2 + 16)) == BAN_SOLID ||
                                ban_map.get_by_pixel(s1, (s2 + 16)) == BAN_ICE ||
                                ban_map.get_by_pixel((s1 + 15), (s2 + 16)) == BAN_SOLID ||
                                ban_map.get_by_pixel((s1 + 15), (s2 + 16)) == BAN_ICE) {
                            players[c1].y_add = -280000L;
                            players[c1].anim = 2;
                            players[c1].frame = 0;
                            players[c1].frame_tick = 0;
                            players[c1].image = player_anims[players[c1].anim].frame[players[c1].frame].image +
                                               players[c1].direction * 9;
                            players[c1].jump_ready = 0;
                            players[c1].jump_abort = 1;
                            if (pogostick == 0)
                                dj_play_sfx(main_info, SFX_JUMP, (unsigned short) (SFX_JUMP_FREQ + rnd(2000) - 1000),
                                            64, 0, -1);
                            else
                                dj_play_sfx(main_info, SFX_SPRING,
                                            (unsigned short) (SFX_SPRING_FREQ + rnd(2000) - 1000), 64, 0, -1);
                        }
                        /* jump out of water */
                        if (ban_map.is_pixel_in_water(s1, s2)) {
                            players[c1].y_add = -196608L;
                            players[c1].in_water = 0;
                            players[c1].anim = 2;
                            players[c1].frame = 0;
                            players[c1].frame_tick = 0;
                            players[c1].image = player_anims[players[c1].anim].frame[players[c1].frame].image +
                                               players[c1].direction * 9;
                            players[c1].jump_ready = 0;
                            players[c1].jump_abort = 1;
                            if (pogostick == 0)
                                dj_play_sfx(main_info, SFX_JUMP, (unsigned short) (SFX_JUMP_FREQ + rnd(2000) - 1000),
                                            64, 0, -1);
                            else
                                dj_play_sfx(main_info, SFX_SPRING,
                                            (unsigned short) (SFX_SPRING_FREQ + rnd(2000) - 1000), 64, 0, -1);
                        }
                    }
                    /* fall down by gravity */
                    if (pogostick == 0 && (!players[c1].action_up)) {
                        players[c1].jump_ready = 1;
                        if (players[c1].in_water == 0 && players[c1].y_add < 0 && players[c1].jump_abort == 1) {
                            if (bunnies_in_space == 0)
                                /* normal gravity */
                                players[c1].y_add += 32768;
                            else
                                /* light gravity */
                                players[c1].y_add += 16384;
                            if (players[c1].y_add > 0)
                                players[c1].y_add = 0;
                        }
                    }
                } else {
                    /* with jetpack */
                    if (players[c1].action_up) {
                        players[c1].y_add -= 16384;
                        if (players[c1].y_add < -400000L)
                            players[c1].y_add = -400000L;
                        if (ban_map.is_pixel_in_water(s1, s2))
                            players[c1].in_water = 0;
                        if (rnd(100) < 50)
                            add_object(OBJ_SMOKE, (players[c1].x >> 16) + 6 + rnd(5), (players[c1].y >> 16) + 10 + rnd(5),
                                       0, 16384 + rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                }

                players[c1].x += players[c1].x_add;
                if ((players[c1].x >> 16) < 0) {
                    players[c1].x = 0;
                    players[c1].x_add = 0;
                }
                if ((players[c1].x >> 16) + 15 > 351) {
                    players[c1].x = 336L << 16;
                    players[c1].x_add = 0;
                }
                {
                    if (players[c1].y > 0) {
                        s2 = (players[c1].y >> 16);
                    } else {
                        /* check top line only */
                        s2 = 0;
                    }

                    s1 = (players[c1].x >> 16);
                    if (ban_map.get_by_pixel(s1, s2) == BAN_SOLID || ban_map.get_by_pixel(s1, s2) == BAN_ICE ||
                            ban_map.get_by_pixel(s1, s2) == BAN_SPRING || ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SOLID ||
                            ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_ICE ||
                            ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SPRING) {
                        players[c1].x = (((s1 + 16) & 0xfff0)) << 16;
                        players[c1].x_add = 0;
                    }

                    s1 = (players[c1].x >> 16);
                    if (ban_map.get_by_pixel((s1 + 15), s2) == BAN_SOLID ||
                            ban_map.get_by_pixel((s1 + 15), s2) == BAN_ICE ||
                            ban_map.get_by_pixel((s1 + 15), s2) == BAN_SPRING ||
                            ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SOLID ||
                            ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_ICE ||
                            ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                        players[c1].x = (((s1 + 16) & 0xfff0) - 16) << 16;
                        players[c1].x_add = 0;
                    }
                }

                players[c1].y += players[c1].y_add;

                s1 = (players[c1].x >> 16);
                s2 = (players[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (ban_map.get_by_pixel((s1 + 8), (s2 + 15)) == BAN_SPRING ||
                    ((ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SPRING &&
                            ban_map.get_by_pixel((s1 + 15), (s2 + 15)) != BAN_SOLID) ||
                     (ban_map.get_by_pixel(s1, (s2 + 15)) != BAN_SOLID &&
                             ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SPRING))) {
                    players[c1].y = ((players[c1].y >> 16) & 0xfff0) << 16;
                    players[c1].y_add = -400000L;
                    players[c1].anim = 2;
                    players[c1].frame = 0;
                    players[c1].frame_tick = 0;
                    players[c1].image =
                            player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                    players[c1].jump_ready = 0;
                    players[c1].jump_abort = 0;
                    for (c2 = 0; c2 < objects.size(); c2++) {
                        if (objects[c2].used == 1 && objects[c2].type == OBJ_SPRING) {
                            if (ban_map.get_by_pixel((s1 + 8), (s2 + 15)) == BAN_SPRING) {
                                if ((objects[c2].x >> 20) == ((s1 + 8) >> 4) &&
                                    (objects[c2].y >> 20) == ((s2 + 15) >> 4)) {
                                    objects[c2].frame = 0;
                                    objects[c2].ticks = object_anims[objects[c2].anim].frame[objects[c2].frame].ticks;
                                    objects[c2].image = object_anims[objects[c2].anim].frame[objects[c2].frame].image;
                                    break;
                                }
                            } else {
                                if (ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SPRING) {
                                    if ((objects[c2].x >> 20) == (s1 >> 4) &&
                                        (objects[c2].y >> 20) == ((s2 + 15) >> 4)) {
                                        objects[c2].frame = 0;
                                        objects[c2].ticks = object_anims[objects[c2].anim].frame[objects[c2].frame].ticks;
                                        objects[c2].image = object_anims[objects[c2].anim].frame[objects[c2].frame].image;
                                        break;
                                    }
                                } else if (ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                                    if ((objects[c2].x >> 20) == ((s1 + 15) >> 4) &&
                                        (objects[c2].y >> 20) == ((s2 + 15) >> 4)) {
                                        objects[c2].frame = 0;
                                        objects[c2].ticks = object_anims[objects[c2].anim].frame[objects[c2].frame].ticks;
                                        objects[c2].image = object_anims[objects[c2].anim].frame[objects[c2].frame].image;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    dj_play_sfx(main_info, SFX_SPRING, (unsigned short) (SFX_SPRING_FREQ + rnd(2000) - 1000), 64, 0, -1);
                }
                s1 = (players[c1].x >> 16);
                s2 = (players[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (ban_map.get_by_pixel(s1, s2) == BAN_SOLID || ban_map.get_by_pixel(s1, s2) == BAN_ICE ||
                        ban_map.get_by_pixel(s1, s2) == BAN_SPRING || ban_map.get_by_pixel((s1 + 15), s2) == BAN_SOLID ||
                        ban_map.get_by_pixel((s1 + 15), s2) == BAN_ICE ||
                        ban_map.get_by_pixel((s1 + 15), s2) == BAN_SPRING) {
                    players[c1].y = (((s2 + 16) & 0xfff0)) << 16; //TODO: MASK
                    players[c1].y_add = 0;
                    players[c1].anim = 0;
                    players[c1].frame = 0;
                    players[c1].frame_tick = 0;
                    players[c1].image =
                            player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                }
                s1 = (players[c1].x >> 16);
                s2 = (players[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (ban_map.get_by_pixel((s1 + 8), (s2 + 8)) == BAN_WATER) {
                    if (players[c1].in_water == 0) {
                        /* falling into water */
                        players[c1].in_water = 1;
                        players[c1].anim = 4;
                        players[c1].frame = 0;
                        players[c1].frame_tick = 0;
                        players[c1].image =
                                player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                        if (players[c1].y_add >= 32768) {
                            add_object(OBJ_SPLASH, (players[c1].x >> 16) + 8, ((players[c1].y >> 16) & 0xfff0) + 15, 0, 0,
                                       OBJ_ANIM_SPLASH, 0);
                            if (blood_is_thicker_than_water == 0)
                                dj_play_sfx(main_info, SFX_SPLASH,
                                            (unsigned short) (SFX_SPLASH_FREQ + rnd(2000) - 1000), 64, 0, -1);
                            else
                                dj_play_sfx(main_info, SFX_SPLASH,
                                            (unsigned short) (SFX_SPLASH_FREQ + rnd(2000) - 5000), 64, 0, -1);
                        }
                    }
                    /* slowly move up to water surface */
                    players[c1].y_add -= 1536;
                    if (players[c1].y_add < 0 && players[c1].anim != 5) {
                        players[c1].anim = 5;
                        players[c1].frame = 0;
                        players[c1].frame_tick = 0;
                        players[c1].image =
                                player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                    }
                    if (players[c1].y_add < -65536L)
                        players[c1].y_add = -65536L;
                    if (players[c1].y_add > 65535L)
                        players[c1].y_add = 65535L;
                    if (ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SOLID ||
                            ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_ICE ||
                            ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SOLID ||
                            ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_ICE) {
                        players[c1].y = (((s2 + 16) & 0xfff0) - 16) << 16;
                        players[c1].y_add = 0;
                    }
                } else if (ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SOLID ||
                        ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_ICE ||
                        ban_map.get_by_pixel(s1, (s2 + 15)) == BAN_SPRING ||
                        ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SOLID ||
                        ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_ICE ||
                        ban_map.get_by_pixel((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                    players[c1].in_water = 0;
                    players[c1].y = (((s2 + 16) & 0xfff0) - 16) << 16;
                    players[c1].y_add = 0;
                    if (players[c1].anim != 0 && players[c1].anim != 1) {
                        players[c1].anim = 0;
                        players[c1].frame = 0;
                        players[c1].frame_tick = 0;
                        players[c1].image =
                                player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                    }
                } else {
                    if (players[c1].in_water == 0) {
                        if (bunnies_in_space == 0)
                            players[c1].y_add += 12288;
                        else
                            players[c1].y_add += 6144;
                        if (players[c1].y_add > 327680L)
                            players[c1].y_add = 327680L;
                    } else {
                        players[c1].y = (players[c1].y & 0xffff0000) + 0x10000;
                        players[c1].y_add = 0;
                    }
                    players[c1].in_water = 0;
                }
                if (players[c1].y_add > 36864 && players[c1].anim != 3 && players[c1].in_water == 0) {
                    players[c1].anim = 3;
                    players[c1].frame = 0;
                    players[c1].frame_tick = 0;
                    players[c1].image =
                            player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;
                }

            }

            players[c1].frame_tick++;
            if (players[c1].frame_tick >= player_anims[players[c1].anim].frame[players[c1].frame].ticks) {
                players[c1].frame++;
                if (players[c1].frame >= player_anims[players[c1].anim].num_frames) {
                    if (players[c1].anim != 6)
                        players[c1].frame = player_anims[players[c1].anim].restart_frame;
                    else
                        position_player(c1);
                }
                players[c1].frame_tick = 0;
            }
            players[c1].image = player_anims[players[c1].anim].frame[players[c1].frame].image + players[c1].direction * 9;

        }

    }

}

void position_player(int player_num) {
    int c1;
    int x, y;

    while (1) {

        std::tie(x, y) = ban_map.get_random_available_floor_position();

        //verifica que el conejo no este cerca de otros conejos
        for (c1 = 0; c1 < players.size(); c1++) {
            if (c1 != player_num && players[c1].enabled == 1) {
                if (abs((x << 4) - (players[c1].x >> 16)) < 32 && abs((y << 4) - (players[c1].y >> 16)) < 32)
                    break;
            }
        }


        if (c1 == players.size()) {
            players[player_num].x = (long) x << 20;
            players[player_num].y = (long) y << 20;

            //TODO : valores
                    printf("Valories misteriosos %d => [%d, %d] => [%d, %d]\n", player_num, x, y, players[player_num].x, players[player_num].y);

            players[player_num].x_add = players[player_num].y_add = 0;
            players[player_num].direction = 0;
            players[player_num].jump_ready = 1;
            players[player_num].in_water = 0;
            players[player_num].anim = 0;
            players[player_num].frame = 0;
            players[player_num].frame_tick = 0;
            players[player_num].image = player_anims[players[player_num].anim].frame[players[player_num].frame].image;

            players[player_num].dead_flag = 0;

            break;
        }
    }

}

void player_kill(int c1, int c2) {
    if (players[c1].y_add >= 0) {

        serverSendKillPacket(c1, c2);
    } else {
        if (players[c2].y_add < 0)
            players[c2].y_add = 0;
    }
}

void check_collision(int c1, int c2) {
    if (players[c1].enabled == 1 && players[c2].enabled == 1) {
        if (labs(players[c1].x - players[c2].x) < (12L << 16) &&
            labs(players[c1].y - players[c2].y) < (12L << 16)) {
            if ((labs(players[c1].y - players[c2].y) >> 16) > 5) {
                if (players[c1].y < players[c2].y) {
                    player_kill(c1, c2);
                } else {
                    player_kill(c2, c1);
                }
            } else {
                if (players[c1].x < players[c2].x) {
                    if (players[c1].x_add > 0)
                        players[c1].x = players[c2].x - (12L << 16);
                    else if (players[c2].x_add < 0)
                        players[c2].x = players[c1].x + (12L << 16);
                    else {
                        players[c1].x -= players[c1].x_add;
                        players[c2].x -= players[c2].x_add;
                    }
                    int l1 = players[c2].x_add;
                    players[c2].x_add = players[c1].x_add;
                    players[c1].x_add = l1;
                    if (players[c1].x_add > 0)
                        players[c1].x_add = -players[c1].x_add;
                    if (players[c2].x_add < 0)
                        players[c2].x_add = -players[c2].x_add;
                } else {
                    if (players[c1].x_add > 0)
                        players[c2].x = players[c1].x - (12L << 16);
                    else if (players[c2].x_add < 0)
                        players[c1].x = players[c2].x + (12L << 16);
                    else {
                        players[c1].x -= players[c1].x_add;
                        players[c2].x -= players[c2].x_add;
                    }
                    int l1 = players[c2].x_add;
                    players[c2].x_add = players[c1].x_add;
                    players[c1].x_add = l1;
                    if (players[c1].x_add < 0)
                        players[c1].x_add = -players[c1].x_add;
                    if (players[c2].x_add > 0)
                        players[c2].x_add = -players[c2].x_add;
                }
            }
        }
    }
}

void collision_check() {
    /* collision check */
    for (int c1 = 0; c1 < 4; c1++) {
        for (int c2 = c1 + 1; c2 < 4 ; c2++) {
            check_collision(c1, c2);

        }
    }
}