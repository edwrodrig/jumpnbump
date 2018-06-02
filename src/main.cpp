/*
 * main.c
 * Copyright (C) 1998 Brainchild Design - http://brainchilddesign.com/
 *
 * Copyright (C) 2001 Chuck Mason <cemason@users.sourceforge.net>
 *
 * Copyright (C) 2002 Florian Schulze <crow@icculus.org>
 *
 * Copyright (C) 2015 Côme Chilliet <come@chilliet.eu>
 *
 * This file is part of Jump 'n Bump.
 *
 * Jump 'n Bump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Jump 'n Bump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "globals.h"
#include "menu.h"
#include <fcntl.h>
#include <string>

#ifndef _WIN32

#include <unistd.h>
#include <utility>
#include <tuple>

#endif

#ifdef BZLIB_SUPPORT

#include "bzlib.h"

#endif

#ifdef ZLIB_SUPPORT

#include "zlib.h"
#include "main_info.h"
#include "gob.h"
#include "player.h"
#include "data.h"

#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
int endscore_reached;

const auto RABBIT_NAME_1 = "AMANDA";
const auto RABBIT_NAME_2 = "EDWIN";
unsigned char *datafile_buffer = nullptr;

gob_t rabbit_gobs = {0};
gob_t font_gobs = {0};
gob_t object_gobs = {0};
gob_t number_gobs = {0};

main_info_t main_info;
player_t player[JNB_MAX_PLAYERS];
player_anim_t player_anims[7];
object_t objects[NUM_OBJECTS];
joy_t joy;

char datfile_name[2048];

unsigned char *background_pic;
unsigned char *mask_pic;
int flip = 0;
char pal[768];
char cur_pal[768];

int ai[JNB_MAX_PLAYERS];

unsigned int ban_map[17][22] = {
        {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
        {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 1, 1},
        {2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

#define GET_BAN_MAP_XY(x, y) ban_map[(y) >> 4][(x) >> 4]


struct {
    int num_frames;
    int restart_frame;
    struct {
        int image;
        int ticks;
    } frame[10];
} object_anims[8] = {
        {
                6,  0,
                {
                        {0,  3},
                        {1,  3},
                        {2,  3},
                        {3,  3},
                        {4,  3},
                        {5,  3},
                        {0,  0},
                        {0,  0},
                        {0,  0},
                        {0,  0}
                }
        },
        {
                9,  0,
                {
                        {6,  2},
                        {7,  2},
                        {8,  2},
                        {9,  2},
                        {10, 2},
                        {11, 2},
                        {12, 2},
                        {13, 2},
                        {14, 2},
                        {0,  0}
                }
        },
        {
                5,  0,
                {
                        {15, 3},
                        {16, 3},
                        {16, 3},
                        {17, 3},
                        {18, 3},
                        {19, 3},
                        {0,  0},
                        {0,  0},
                        {0,  0},
                        {0,  0}
                }
        },
        {
                10, 0,
                {
                        {20, 2},
                        {21, 2},
                        {22, 2},
                        {23, 2},
                        {24, 2},
                        {25, 2},
                        {24, 2},
                        {23, 2},
                        {22, 2},
                        {21, 2}
                }
        },
        {
                10, 0,
                {
                        {26, 2},
                        {27, 2},
                        {28, 2},
                        {29, 2},
                        {30, 2},
                        {31, 2},
                        {30, 2},
                        {29, 2},
                        {28, 2},
                        {27, 2}
                }
        },
        {
                10, 0,
                {
                        {32, 2},
                        {33, 2},
                        {34, 2},
                        {35, 2},
                        {36, 2},
                        {37, 2},
                        {36, 2},
                        {35, 2},
                        {34, 2},
                        {33, 2}
                }
        },
        {
                10, 0,
                {
                        {38, 2},
                        {39, 2},
                        {40, 2},
                        {41, 2},
                        {42, 2},
                        {43, 2},
                        {42, 2},
                        {41, 2},
                        {40, 2},
                        {39, 2}
                }
        },
        {
                4,  0,
                {
                        {76, 4},
                        {77, 4},
                        {78, 4},
                        {79, 4},
                        {0,  0},
                        {0,  0},
                        {0,  0},
                        {0,  0},
                        {0,  0},
                        {0,  0}
                }
        }
};

struct {
    struct {
        short num_pobs;
        struct {
            int x, y;
            int image;
            gob_t *pob_data;
        } pobs[NUM_LEFTOVERS];
    } page[2];
} leftovers;

int pogostick, bunnies_in_space, jetpack, blood_is_thicker_than_water;

int client_player_num = -1;



static void flip_pixels(unsigned char *pixels) {
    int x, y;
    unsigned char temp;

    assert(pixels);
    for (y = 0; y < JNB_HEIGHT; y++) {
        for (x = 0; x < (352 / 2); x++) {
            temp = pixels[y * JNB_WIDTH + x];
            pixels[y * JNB_WIDTH + x] = pixels[y * JNB_WIDTH + (352 - x) - 1];
            pixels[y * JNB_WIDTH + (352 - x) - 1] = temp;
        }
    }
}


void serverSendKillPacket(int killer, int victim) {
    int c1 = killer;
    int c2 = victim;
    int x = player[victim].x;
    int y = player[victim].y;
    int c4 = 0;
    int s1 = 0;

    player[c1].y_add = -player[c1].y_add;
    if (player[c1].y_add > -262144L)
        player[c1].y_add = -262144L;
    player[c1].jump_abort = 1;
    player[c2].dead_flag = 1;
    if (player[c2].anim != 6) {
        player[c2].anim = 6;
        player[c2].frame = 0;
        player[c2].frame_tick = 0;
        player[c2].image = player_anims[player[c2].anim].frame[player[c2].frame].image + player[c2].direction * 9;
        if (main_info.no_gore == 0) {
            for (c4 = 0; c4 < 6; c4++)
                add_object(OBJ_FUR, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 44 + c2 * 8);
            for (c4 = 0; c4 < 6; c4++)
                add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 76);
            for (c4 = 0; c4 < 6; c4++)
                add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 77);
            for (c4 = 0; c4 < 8; c4++)
                add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 78);
            for (c4 = 0; c4 < 10; c4++)
                add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 79);
        }
        dj_play_sfx(main_info, SFX_DEATH, (unsigned short) (SFX_DEATH_FREQ + rnd(2000) - 1000), 64, 0, -1);
        player[c1].bumps++;
        if (player[c1].bumps >= JNB_END_SCORE) {
            endscore_reached = 1;
        }
        player[c1].bumped[c2]++;
        s1 = player[c1].bumps % 100;
        add_leftovers(0, 360, 34 + c1 * 64, s1 / 10, &number_gobs);
        add_leftovers(1, 360, 34 + c1 * 64, s1 / 10, &number_gobs);
        add_leftovers(0, 376, 34 + c1 * 64, s1 - (s1 / 10) * 10, &number_gobs);
        add_leftovers(1, 376, 34 + c1 * 64, s1 - (s1 / 10) * 10, &number_gobs);
    }
}

static void player_kill(int c1, int c2) {
    if (player[c1].y_add >= 0) {

        serverSendKillPacket(c1, c2);
    } else {
        if (player[c2].y_add < 0)
            player[c2].y_add = 0;
    }
}

static void check_cheats(void) {
    if (strncmp(last_keys, "kcitsogop", strlen("kcitsogop")) == 0) {
        pogostick ^= 1;
        last_keys[0] = 0;
    }
    if (strncmp(last_keys, "ecapsniseinnub", strlen("ecapsniseinnub")) == 0) {
        bunnies_in_space ^= 1;
        last_keys[0] = 0;
    }
    if (strncmp(last_keys, "kcaptej", strlen("kcaptej")) == 0) {
        jetpack ^= 1;
        last_keys[0] = 0;
    }
    if (strncmp(last_keys, "retawnahtrekcihtsidoolb", strlen("retawnahtrekcihtsidoolb")) == 0) {
        char blood[32] = {
                63, 32, 32, 53, 17, 17, 42, 7,
                7, 28, 0, 0, 24, 0, 0, 19,
                0, 0, 12, 0, 0, 7, 0, 0
        };
        char water[32] = {
                63, 63, 63, 40, 53, 62, 19, 42,
                60, 0, 33, 60, 3, 32, 46, 3,
                26, 33, 3, 19, 21, 1, 8, 8
        };
        int i;

        blood_is_thicker_than_water ^= 1;
        if (blood_is_thicker_than_water == 1) {
            for (i = 0; i < 32; i++)
                pal[432 + i] = blood[i];
        } else {
            for (i = 0; i < 32; i++)
                pal[432 + i] = water[i];
        }
        register_background(background_pic);
        last_keys[0] = 0;
    }
}

void check_collision(int c1, int c2) {
    if (player[c1].enabled == 1 && player[c2].enabled == 1) {
        if (labs(player[c1].x - player[c2].x) < (12L << 16) &&
            labs(player[c1].y - player[c2].y) < (12L << 16)) {
            if ((labs(player[c1].y - player[c2].y) >> 16) > 5) {
                if (player[c1].y < player[c2].y) {
                    player_kill(c1, c2);
                } else {
                    player_kill(c2, c1);
                }
            } else {
                if (player[c1].x < player[c2].x) {
                    if (player[c1].x_add > 0)
                        player[c1].x = player[c2].x - (12L << 16);
                    else if (player[c2].x_add < 0)
                        player[c2].x = player[c1].x + (12L << 16);
                    else {
                        player[c1].x -= player[c1].x_add;
                        player[c2].x -= player[c2].x_add;
                    }
                    int l1 = player[c2].x_add;
                    player[c2].x_add = player[c1].x_add;
                    player[c1].x_add = l1;
                    if (player[c1].x_add > 0)
                        player[c1].x_add = -player[c1].x_add;
                    if (player[c2].x_add < 0)
                        player[c2].x_add = -player[c2].x_add;
                } else {
                    if (player[c1].x_add > 0)
                        player[c2].x = player[c1].x - (12L << 16);
                    else if (player[c2].x_add < 0)
                        player[c1].x = player[c2].x + (12L << 16);
                    else {
                        player[c1].x -= player[c1].x_add;
                        player[c2].x -= player[c2].x_add;
                    }
                    int l1 = player[c2].x_add;
                    player[c2].x_add = player[c1].x_add;
                    player[c1].x_add = l1;
                    if (player[c1].x_add < 0)
                        player[c1].x_add = -player[c1].x_add;
                    if (player[c2].x_add > 0)
                        player[c2].x_add = -player[c2].x_add;
                }
            }
        }
    }
}


static void collision_check(void) {
    /* collision check */
    for (int c1 = 0; c1 < 4; c1++) {
        for (int c2 = c1 + 1; c2 < 4 ; c2++) {
            check_collision(c1, c2);

        }
    }
}

static void game_loop(void) {
    int mod_vol, sfx_vol;
    int update_count = 1;
    int end_loop_flag = 0;
    int fade_flag = 0;
    int update_palette = 0;
    int mod_fade_direction;
    int i;

    mod_vol = sfx_vol = 0;
    mod_fade_direction = 1;
    dj_ready_mod(main_info, MOD_GAME, datafile_buffer);
    dj_set_mod_volume(main_info, (char) mod_vol);
    dj_set_sfx_volume(main_info, (char) mod_vol);
    dj_start_mod(main_info);

    intr_sysupdate();

    endscore_reached = 0;
    while (1) {
        while (update_count) {

            if (endscore_reached || (key_pressed(1) == 1)) {

                end_loop_flag = 1;
                memset(pal, 0, 768);
                mod_fade_direction = 0;
            }

            check_cheats();


            steer_players();


            collision_check();


            main_info.page_info[main_info.draw_page].num_pobs = 0;
            for (i = 0; i < JNB_MAX_PLAYERS; i++) {
                if (player[i].enabled == 1)
                    main_info.page_info[main_info.draw_page].num_pobs++;
            }

            update_objects();


            if (update_count == 1) {
                int c2;

                for (i = 0, c2 = 0; i < JNB_MAX_PLAYERS; i++) {
                    if (player[i].enabled == 1) {
                        main_info.page_info[main_info.draw_page].pobs[c2].x = player[i].x >> 16;
                        main_info.page_info[main_info.draw_page].pobs[c2].y = player[i].y >> 16;
                        main_info.page_info[main_info.draw_page].pobs[c2].image = player[i].image + i * 18;
                        main_info.page_info[main_info.draw_page].pobs[c2].pob_data = &rabbit_gobs;
                        c2++;
                    }
                }

                draw_begin();

                draw_pobs(main_info.draw_page);


                draw_end();
            }

            if (mod_fade_direction == 1) {
                if (mod_vol < 30) {
                    mod_vol++;
                    dj_set_mod_volume(main_info, (char) mod_vol);
                }
                if (sfx_vol < 64) {
                    sfx_vol++;
                    dj_set_sfx_volume(main_info, (char) sfx_vol);
                }
            } else {
                if (mod_vol > 0) {
                    mod_vol--;
                    dj_set_mod_volume(main_info, (char) mod_vol);
                }
                if (sfx_vol > 0) {
                    sfx_vol--;
                    dj_set_sfx_volume(main_info, (char) sfx_vol);
                }
            }

            fade_flag = 0;
            for (i = 0; i < 768; i++) {
                if (cur_pal[i] < pal[i]) {
                    cur_pal[i]++;
                    fade_flag = 1;
                } else if (cur_pal[i] > pal[i]) {
                    cur_pal[i]--;
                    fade_flag = 1;
                }
            }
            if (fade_flag == 1)
                update_palette = 1;
            if (fade_flag == 0 && end_loop_flag == 1)
                break;

            if (update_count == 1) {
                if (update_palette == 1) {
                    setpalette(0, 256, cur_pal);
                    update_palette = 0;
                }

                main_info.draw_page ^= 1;
                main_info.view_page ^= 1;

                flippage(main_info.view_page);

                wait_vrt();

                draw_begin();

                redraw_pob_backgrounds(main_info.draw_page);

                draw_leftovers(main_info.draw_page);

                draw_end();
            }

            update_count--;
        }

        update_count = intr_sysupdate();

        if ((fade_flag == 0) && (end_loop_flag == 1))
            break;
    }
}


static int menu_loop(unsigned char* datafile_buffer) {
    unsigned char *handle;
    int mod_vol;
    int c1, c2;

    for (c1 = 0; c1 < JNB_MAX_PLAYERS; c1++)        // reset player values
    {
        ai[c1] = 0;
    }

    while (1) {

        if (menu(main_info, datafile_buffer) != 0)
            deinit_program();


        if (key_pressed(1) == 1) {
            return 0;
        }
        if (init_level(0, pal) != 0) {
            deinit_level();
            deinit_program();
        }

        memset(cur_pal, 0, 768);
        setpalette(0, 256, cur_pal);


        flippage(1);
        register_background(background_pic);
        flippage(0);

        bunnies_in_space = jetpack = pogostick = blood_is_thicker_than_water = 0;
        //blood_is_thicker_than_water = 1; HERE IS TO MOD THE CHEATS
        main_info.page_info[0].num_pobs = 0;
        main_info.page_info[1].num_pobs = 0;
        main_info.view_page = 0;
        main_info.draw_page = 1;

        game_loop();

        main_info.view_page = 0;
        main_info.draw_page = 1;

        dj_stop_sfx_channel(main_info, 4);

        deinit_level();

        memset(mask_pic, 0, JNB_WIDTH * JNB_HEIGHT);
        register_mask(mask_pic);

        register_background(NULL);

        draw_begin();

        put_text(main_info.view_page, 100, 50, RABBIT_NAME_1, 2);
        put_text(main_info.view_page, 160, 50, RABBIT_NAME_2, 2);
        put_text(main_info.view_page, 220, 50, "FIZZ", 2);
        put_text(main_info.view_page, 280, 50, "MIJJI", 2);
        put_text(main_info.view_page, 40, 80, RABBIT_NAME_1, 2);
        put_text(main_info.view_page, 40, 110, RABBIT_NAME_2, 2);
        put_text(main_info.view_page, 40, 140, "FIZZ", 2);
        put_text(main_info.view_page, 40, 170, "MIJJI", 2);

        for (c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
            if (!player[c1].enabled) {
                continue;
            }
            char str1[100];

            for (c2 = 0; c2 < JNB_MAX_PLAYERS; c2++) {
                if (!player[c2].enabled) {
                    continue;
                }
                if (c2 != c1) {
                    sprintf(str1, "%d", player[c1].bumped[c2]);
                    put_text(main_info.view_page, 100 + c2 * 60, 80 + c1 * 30, str1, 2);
                } else
                    put_text(main_info.view_page, 100 + c2 * 60, 80 + c1 * 30, "-", 2);
            }
            sprintf(str1, "%d", player[c1].bumps);
            put_text(main_info.view_page, 350, 80 + c1 * 30, str1, 2);
        }

        put_text(main_info.view_page, 200, 230, "Press ESC to continue", 2);

        draw_end();

        flippage(main_info.view_page);

        if ((handle = dat_open("menu.pcx", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'menu.pcx', aborting...\n";
            return 1;
        }
        if (read_pcx(handle, background_pic, JNB_WIDTH * JNB_HEIGHT, pal) != 0) {
            main_info.error_str = "Error loading 'menu.pcx', aborting...\n";
            return 1;
        }

        /* fix dark font */
        for (c1 = 0; c1 < 16; c1++) {
            pal[(240 + c1) * 3 + 0] = c1 << 2;
            pal[(240 + c1) * 3 + 1] = c1 << 2;
            pal[(240 + c1) * 3 + 2] = c1 << 2;
        }

        memset(cur_pal, 0, 768);

        setpalette(0, 256, cur_pal);

        mod_vol = 0;
        dj_ready_mod(main_info, MOD_SCORES, datafile_buffer);
        dj_set_mod_volume(main_info, (char) mod_vol);
        dj_start_mod(main_info);

        while (key_pressed(1) == 0) {
            if (mod_vol < 35)
                mod_vol++;
            dj_set_mod_volume(main_info, (char) mod_vol);
            for (c1 = 0; c1 < 768; c1++) {
                if (cur_pal[c1] < pal[c1])
                    cur_pal[c1]++;
            }

            intr_sysupdate();
            wait_vrt();
            setpalette(0, 256, cur_pal);
            flippage(main_info.view_page);
        }
        while (key_pressed(1) == 1) {

            intr_sysupdate();
        }

        memset(pal, 0, 768);

        while (mod_vol > 0) {
            mod_vol--;
            dj_set_mod_volume(main_info, (char) mod_vol);
            for (c1 = 0; c1 < 768; c1++) {
                if (cur_pal[c1] > pal[c1])
                    cur_pal[c1]--;
            }

            wait_vrt();
            setpalette(0, 256, cur_pal);
            flippage(main_info.view_page);
        }

        fillpalette(0, 0, 0);


        dj_stop_mod(main_info);
    }
}


int main(int argc, char *argv[]) {
    int result;

    if (init_program(argc, argv, pal) != 0)
        deinit_program();

    result = menu_loop(datafile_buffer);

    deinit_program();

    return result;
}


static void player_action_left(int c1) {
    int s1 = 0, s2 = 0;
    int below_left, below, below_right;

    s1 = (player[c1].x >> 16);
    s2 = (player[c1].y >> 16);
    below_left = GET_BAN_MAP_XY(s1, s2 + 16);
    below = GET_BAN_MAP_XY(s1 + 8, s2 + 16);
    below_right = GET_BAN_MAP_XY(s1 + 15, s2 + 16);

    if (below == BAN_ICE) {
        if (player[c1].x_add > 0)
            player[c1].x_add -= 1024;
        else
            player[c1].x_add -= 768;
    } else if ((below_left != BAN_SOLID && below_right == BAN_ICE) ||
               (below_left == BAN_ICE && below_right != BAN_SOLID)) {
        if (player[c1].x_add > 0)
            player[c1].x_add -= 1024;
        else
            player[c1].x_add -= 768;
    } else {
        if (player[c1].x_add > 0) {
            player[c1].x_add -= 16384;
            if (player[c1].x_add > -98304L && player[c1].in_water == 0 && below == BAN_SOLID)
                add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0,
                           -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
        } else
            player[c1].x_add -= 12288;
    }
    if (player[c1].x_add < -98304L)
        player[c1].x_add = -98304L;
    player[c1].direction = 1;
    if (player[c1].anim == 0) {
        player[c1].anim = 1;
        player[c1].frame = 0;
        player[c1].frame_tick = 0;
        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
    }
}


static void player_action_right(int c1) {
    int s1 = 0, s2 = 0;
    int below_left, below, below_right;

    s1 = (player[c1].x >> 16);
    s2 = (player[c1].y >> 16);
    below_left = GET_BAN_MAP_XY(s1, s2 + 16);
    below = GET_BAN_MAP_XY(s1 + 8, s2 + 16);
    below_right = GET_BAN_MAP_XY(s1 + 15, s2 + 16);

    if (below == BAN_ICE) {
        if (player[c1].x_add < 0)
            player[c1].x_add += 1024;
        else
            player[c1].x_add += 768;
    } else if ((below_left != BAN_SOLID && below_right == BAN_ICE) ||
               (below_left == BAN_ICE && below_right != BAN_SOLID)) {
        if (player[c1].x_add > 0)
            player[c1].x_add += 1024;
        else
            player[c1].x_add += 768;
    } else {
        if (player[c1].x_add < 0) {
            player[c1].x_add += 16384;
            if (player[c1].x_add < 98304L && player[c1].in_water == 0 && below == BAN_SOLID)
                add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0,
                           -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
        } else
            player[c1].x_add += 12288;
    }
    if (player[c1].x_add > 98304L)
        player[c1].x_add = 98304L;
    player[c1].direction = 0;
    if (player[c1].anim == 0) {
        player[c1].anim = 1;
        player[c1].frame = 0;
        player[c1].frame_tick = 0;
        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
    }
}

#define GET_BAN_MAP_IN_WATER(s1, s2) (GET_BAN_MAP_XY((s1), ((s2) + 7)) == BAN_VOID || GET_BAN_MAP_XY(((s1) + 15), ((s2) + 7)) == BAN_VOID) && (GET_BAN_MAP_XY((s1), ((s2) + 8)) == BAN_WATER || GET_BAN_MAP_XY(((s1) + 15), ((s2) + 8)) == BAN_WATER)


void steer_players(void) {
    int c1, c2;
    int s1 = 0, s2 = 0;

    update_player_actions();

    for (c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {

        if (player[c1].enabled == 1) {

            if (player[c1].dead_flag == 0) {

                if (player[c1].action_left && player[c1].action_right) {
                    if (player[c1].direction == 0) {
                        if (player[c1].action_right) {
                            player_action_right(c1);
                        }
                    } else {
                        if (player[c1].action_left) {
                            player_action_left(c1);
                        }
                    }
                } else if (player[c1].action_left) {
                    player_action_left(c1);
                } else if (player[c1].action_right) {
                    player_action_right(c1);
                } else if ((!player[c1].action_left) && (!player[c1].action_right)) {
                    int below_left, below, below_right;

                    s1 = (player[c1].x >> 16);
                    s2 = (player[c1].y >> 16);
                    below_left = GET_BAN_MAP_XY(s1, s2 + 16);
                    below = GET_BAN_MAP_XY(s1 + 8, s2 + 16);
                    below_right = GET_BAN_MAP_XY(s1 + 15, s2 + 16);
                    if (below == BAN_SOLID || below == BAN_SPRING ||
                        (((below_left == BAN_SOLID || below_left == BAN_SPRING) && below_right != BAN_ICE) ||
                         (below_left != BAN_ICE && (below_right == BAN_SOLID || below_right == BAN_SPRING)))) {
                        if (player[c1].x_add < 0) {
                            player[c1].x_add += 16384;
                            if (player[c1].x_add > 0)
                                player[c1].x_add = 0;
                        } else {
                            player[c1].x_add -= 16384;
                            if (player[c1].x_add < 0)
                                player[c1].x_add = 0;
                        }
                        if (player[c1].x_add != 0 && GET_BAN_MAP_XY((s1 + 8), (s2 + 16)) == BAN_SOLID)
                            add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5),
                                       0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                    if (player[c1].anim == 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image =
                                player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                }
                if (jetpack == 0) {
                    /* no jetpack */
                    if (pogostick == 1 || (player[c1].jump_ready == 1 && player[c1].action_up)) {
                        s1 = (player[c1].x >> 16);
                        s2 = (player[c1].y >> 16);
                        if (s2 < -16)
                            s2 = -16;
                        /* jump */
                        if (GET_BAN_MAP_XY(s1, (s2 + 16)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 16)) == BAN_ICE ||
                            GET_BAN_MAP_XY((s1 + 15), (s2 + 16)) == BAN_SOLID ||
                            GET_BAN_MAP_XY((s1 + 15), (s2 + 16)) == BAN_ICE) {
                            player[c1].y_add = -280000L;
                            player[c1].anim = 2;
                            player[c1].frame = 0;
                            player[c1].frame_tick = 0;
                            player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image +
                                               player[c1].direction * 9;
                            player[c1].jump_ready = 0;
                            player[c1].jump_abort = 1;
                            if (pogostick == 0)
                                dj_play_sfx(main_info, SFX_JUMP, (unsigned short) (SFX_JUMP_FREQ + rnd(2000) - 1000),
                                            64, 0, -1);
                            else
                                dj_play_sfx(main_info, SFX_SPRING,
                                            (unsigned short) (SFX_SPRING_FREQ + rnd(2000) - 1000), 64, 0, -1);
                        }
                        /* jump out of water */
                        if (GET_BAN_MAP_IN_WATER(s1, s2)) {
                            player[c1].y_add = -196608L;
                            player[c1].in_water = 0;
                            player[c1].anim = 2;
                            player[c1].frame = 0;
                            player[c1].frame_tick = 0;
                            player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image +
                                               player[c1].direction * 9;
                            player[c1].jump_ready = 0;
                            player[c1].jump_abort = 1;
                            if (pogostick == 0)
                                dj_play_sfx(main_info, SFX_JUMP, (unsigned short) (SFX_JUMP_FREQ + rnd(2000) - 1000),
                                            64, 0, -1);
                            else
                                dj_play_sfx(main_info, SFX_SPRING,
                                            (unsigned short) (SFX_SPRING_FREQ + rnd(2000) - 1000), 64, 0, -1);
                        }
                    }
                    /* fall down by gravity */
                    if (pogostick == 0 && (!player[c1].action_up)) {
                        player[c1].jump_ready = 1;
                        if (player[c1].in_water == 0 && player[c1].y_add < 0 && player[c1].jump_abort == 1) {
                            if (bunnies_in_space == 0)
                                /* normal gravity */
                                player[c1].y_add += 32768;
                            else
                                /* light gravity */
                                player[c1].y_add += 16384;
                            if (player[c1].y_add > 0)
                                player[c1].y_add = 0;
                        }
                    }
                } else {
                    /* with jetpack */
                    if (player[c1].action_up) {
                        player[c1].y_add -= 16384;
                        if (player[c1].y_add < -400000L)
                            player[c1].y_add = -400000L;
                        if (GET_BAN_MAP_IN_WATER(s1, s2))
                            player[c1].in_water = 0;
                        if (rnd(100) < 50)
                            add_object(OBJ_SMOKE, (player[c1].x >> 16) + 6 + rnd(5), (player[c1].y >> 16) + 10 + rnd(5),
                                       0, 16384 + rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                }

                player[c1].x += player[c1].x_add;
                if ((player[c1].x >> 16) < 0) {
                    player[c1].x = 0;
                    player[c1].x_add = 0;
                }
                if ((player[c1].x >> 16) + 15 > 351) {
                    player[c1].x = 336L << 16;
                    player[c1].x_add = 0;
                }
                {
                    if (player[c1].y > 0) {
                        s2 = (player[c1].y >> 16);
                    } else {
                        /* check top line only */
                        s2 = 0;
                    }

                    s1 = (player[c1].x >> 16);
                    if (GET_BAN_MAP_XY(s1, s2) == BAN_SOLID || GET_BAN_MAP_XY(s1, s2) == BAN_ICE ||
                        GET_BAN_MAP_XY(s1, s2) == BAN_SPRING || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SOLID ||
                        GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_ICE || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING) {
                        player[c1].x = (((s1 + 16) & 0xfff0)) << 16;
                        player[c1].x_add = 0;
                    }

                    s1 = (player[c1].x >> 16);
                    if (GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_ICE ||
                        GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SPRING ||
                        GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SOLID ||
                        GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_ICE ||
                        GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                        player[c1].x = (((s1 + 16) & 0xfff0) - 16) << 16;
                        player[c1].x_add = 0;
                    }
                }

                player[c1].y += player[c1].y_add;

                s1 = (player[c1].x >> 16);
                s2 = (player[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (GET_BAN_MAP_XY((s1 + 8), (s2 + 15)) == BAN_SPRING ||
                    ((GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING &&
                      GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) != BAN_SOLID) ||
                     (GET_BAN_MAP_XY(s1, (s2 + 15)) != BAN_SOLID &&
                      GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING))) {
                    player[c1].y = ((player[c1].y >> 16) & 0xfff0) << 16;
                    player[c1].y_add = -400000L;
                    player[c1].anim = 2;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image =
                            player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    player[c1].jump_ready = 0;
                    player[c1].jump_abort = 0;
                    for (c2 = 0; c2 < NUM_OBJECTS; c2++) {
                        if (objects[c2].used == 1 && objects[c2].type == OBJ_SPRING) {
                            if (GET_BAN_MAP_XY((s1 + 8), (s2 + 15)) == BAN_SPRING) {
                                if ((objects[c2].x >> 20) == ((s1 + 8) >> 4) &&
                                    (objects[c2].y >> 20) == ((s2 + 15) >> 4)) {
                                    objects[c2].frame = 0;
                                    objects[c2].ticks = object_anims[objects[c2].anim].frame[objects[c2].frame].ticks;
                                    objects[c2].image = object_anims[objects[c2].anim].frame[objects[c2].frame].image;
                                    break;
                                }
                            } else {
                                if (GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING) {
                                    if ((objects[c2].x >> 20) == (s1 >> 4) &&
                                        (objects[c2].y >> 20) == ((s2 + 15) >> 4)) {
                                        objects[c2].frame = 0;
                                        objects[c2].ticks = object_anims[objects[c2].anim].frame[objects[c2].frame].ticks;
                                        objects[c2].image = object_anims[objects[c2].anim].frame[objects[c2].frame].image;
                                        break;
                                    }
                                } else if (GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING) {
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
                s1 = (player[c1].x >> 16);
                s2 = (player[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (GET_BAN_MAP_XY(s1, s2) == BAN_SOLID || GET_BAN_MAP_XY(s1, s2) == BAN_ICE ||
                    GET_BAN_MAP_XY(s1, s2) == BAN_SPRING || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SOLID ||
                    GET_BAN_MAP_XY((s1 + 15), s2) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SPRING) {
                    player[c1].y = (((s2 + 16) & 0xfff0)) << 16;
                    player[c1].y_add = 0;
                    player[c1].anim = 0;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image =
                            player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                }
                s1 = (player[c1].x >> 16);
                s2 = (player[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (GET_BAN_MAP_XY((s1 + 8), (s2 + 8)) == BAN_WATER) {
                    if (player[c1].in_water == 0) {
                        /* falling into water */
                        player[c1].in_water = 1;
                        player[c1].anim = 4;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image =
                                player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                        if (player[c1].y_add >= 32768) {
                            add_object(OBJ_SPLASH, (player[c1].x >> 16) + 8, ((player[c1].y >> 16) & 0xfff0) + 15, 0, 0,
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
                    player[c1].y_add -= 1536;
                    if (player[c1].y_add < 0 && player[c1].anim != 5) {
                        player[c1].anim = 5;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image =
                                player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                    if (player[c1].y_add < -65536L)
                        player[c1].y_add = -65536L;
                    if (player[c1].y_add > 65535L)
                        player[c1].y_add = 65535L;
                    if (GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_ICE ||
                        GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SOLID ||
                        GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_ICE) {
                        player[c1].y = (((s2 + 16) & 0xfff0) - 16) << 16;
                        player[c1].y_add = 0;
                    }
                } else if (GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_ICE ||
                           GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING ||
                           GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SOLID ||
                           GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_ICE ||
                           GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                    player[c1].in_water = 0;
                    player[c1].y = (((s2 + 16) & 0xfff0) - 16) << 16;
                    player[c1].y_add = 0;
                    if (player[c1].anim != 0 && player[c1].anim != 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image =
                                player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                } else {
                    if (player[c1].in_water == 0) {
                        if (bunnies_in_space == 0)
                            player[c1].y_add += 12288;
                        else
                            player[c1].y_add += 6144;
                        if (player[c1].y_add > 327680L)
                            player[c1].y_add = 327680L;
                    } else {
                        player[c1].y = (player[c1].y & 0xffff0000) + 0x10000;
                        player[c1].y_add = 0;
                    }
                    player[c1].in_water = 0;
                }
                if (player[c1].y_add > 36864 && player[c1].anim != 3 && player[c1].in_water == 0) {
                    player[c1].anim = 3;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image =
                            player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                }

            }

            player[c1].frame_tick++;
            if (player[c1].frame_tick >= player_anims[player[c1].anim].frame[player[c1].frame].ticks) {
                player[c1].frame++;
                if (player[c1].frame >= player_anims[player[c1].anim].num_frames) {
                    if (player[c1].anim != 6)
                        player[c1].frame = player_anims[player[c1].anim].restart_frame;
                    else
                        position_player(c1);
                }
                player[c1].frame_tick = 0;
            }
            player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;

        }

    }

}


std::pair<int, int> get_random_available_position() {
    int x, y = 0;
    while (true) {
        x = rnd(22);
        y = rnd(16);

        if (
                ban_map[y][x] == BAN_VOID            //si esta vacio
                && (
                        ban_map[y + 1][x] == BAN_SOLID ||     //y el de abajo es un solido, posiblemente un suelo
                        ban_map[y + 1][y] == BAN_ICE  //o un hielo
                   )
            ) {
            break;
        }
    }
    return std::make_pair(x,y);
}

void position_player(int player_num) {
    int c1;
    int x, y;

    while (1) {

        std::tie(x, y) = get_random_available_position();

        //verifica que el conejo no este cerca de otros conejos
        for (c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
            if (c1 != player_num && player[c1].enabled == 1) {
                if (abs((x << 4) - (player[c1].x >> 16)) < 32 && abs((y << 4) - (player[c1].y >> 16)) < 32)
                    break;
            }
        }


        if (c1 == JNB_MAX_PLAYERS) {
            player[player_num].x = (long) x << 20;
            player[player_num].y = (long) y << 20;
            player[player_num].x_add = player[player_num].y_add = 0;
            player[player_num].direction = 0;
            player[player_num].jump_ready = 1;
            player[player_num].in_water = 0;
            player[player_num].anim = 0;
            player[player_num].frame = 0;
            player[player_num].frame_tick = 0;
            player[player_num].image = player_anims[player[player_num].anim].frame[player[player_num].frame].image;

            player[player_num].dead_flag = 0;

            break;
        }
    }

}


void add_object(int type, int x, int y, int x_add, int y_add, int anim, int frame) {
    int c1;

    for (c1 = 0; c1 < NUM_OBJECTS; c1++) {
        if (objects[c1].used == 0) {
            objects[c1].used = 1;
            objects[c1].type = type;
            objects[c1].x = (long) x << 16;
            objects[c1].y = (long) y << 16;
            objects[c1].x_add = x_add;
            objects[c1].y_add = y_add;
            objects[c1].x_acc = 0;
            objects[c1].y_acc = 0;
            objects[c1].anim = anim;
            objects[c1].frame = frame;
            objects[c1].ticks = object_anims[anim].frame[frame].ticks;
            objects[c1].image = object_anims[anim].frame[frame].image;
            break;
        }
    }

}


void update_objects(void) {
    int c1;
    int s1 = 0;

    for (c1 = 0; c1 < NUM_OBJECTS; c1++) {
        if (objects[c1].used == 1) {
            switch (objects[c1].type) {
                case OBJ_SPRING:
                    objects[c1].ticks--;
                    if (objects[c1].ticks <= 0) {
                        objects[c1].frame++;
                        if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames) {
                            objects[c1].frame--;
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                        } else {
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    }
                    if (objects[c1].used == 1)
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image,
                                &object_gobs);
                    break;
                case OBJ_SPLASH:
                    objects[c1].ticks--;
                    if (objects[c1].ticks <= 0) {
                        objects[c1].frame++;
                        if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
                            objects[c1].used = 0;
                        else {
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    }
                    if (objects[c1].used == 1)
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image,
                                &object_gobs);
                    break;
                case OBJ_SMOKE:
                    objects[c1].x += objects[c1].x_add;
                    objects[c1].y += objects[c1].y_add;
                    objects[c1].ticks--;
                    if (objects[c1].ticks <= 0) {
                        objects[c1].frame++;
                        if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
                            objects[c1].used = 0;
                        else {
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    }
                    if (objects[c1].used == 1)
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image,
                                &object_gobs);
                    break;
                case OBJ_YEL_BUTFLY:
                case OBJ_PINK_BUTFLY:
                    objects[c1].x_acc += rnd(128) - 64;
                    if (objects[c1].x_acc < -1024)
                        objects[c1].x_acc = -1024;
                    if (objects[c1].x_acc > 1024)
                        objects[c1].x_acc = 1024;
                    objects[c1].x_add += objects[c1].x_acc;
                    if (objects[c1].x_add < -32768)
                        objects[c1].x_add = -32768;
                    if (objects[c1].x_add > 32768)
                        objects[c1].x_add = 32768;
                    objects[c1].x += objects[c1].x_add;
                    if ((objects[c1].x >> 16) < 16) {
                        objects[c1].x = 16 << 16;
                        objects[c1].x_add = -objects[c1].x_add >> 2;
                        objects[c1].x_acc = 0;
                    } else if ((objects[c1].x >> 16) > 350) {
                        objects[c1].x = 350 << 16;
                        objects[c1].x_add = -objects[c1].x_add >> 2;
                        objects[c1].x_acc = 0;
                    }
                    if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0) {
                        if (objects[c1].x_add < 0) {
                            objects[c1].x = (((objects[c1].x >> 16) + 16) & 0xfff0) << 16;
                        } else {
                            objects[c1].x = ((((objects[c1].x >> 16) - 16) & 0xfff0) + 15) << 16;
                        }
                        objects[c1].x_add = -objects[c1].x_add >> 2;
                        objects[c1].x_acc = 0;
                    }
                    objects[c1].y_acc += rnd(64) - 32;
                    if (objects[c1].y_acc < -1024)
                        objects[c1].y_acc = -1024;
                    if (objects[c1].y_acc > 1024)
                        objects[c1].y_acc = 1024;
                    objects[c1].y_add += objects[c1].y_acc;
                    if (objects[c1].y_add < -32768)
                        objects[c1].y_add = -32768;
                    if (objects[c1].y_add > 32768)
                        objects[c1].y_add = 32768;
                    objects[c1].y += objects[c1].y_add;
                    if ((objects[c1].y >> 16) < 0) {
                        objects[c1].y = 0;
                        objects[c1].y_add = -objects[c1].y_add >> 2;
                        objects[c1].y_acc = 0;
                    } else if ((objects[c1].y >> 16) > 255) {
                        objects[c1].y = 255 << 16;
                        objects[c1].y_add = -objects[c1].y_add >> 2;
                        objects[c1].y_acc = 0;
                    }
                    if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0) {
                        if (objects[c1].y_add < 0) {
                            objects[c1].y = (((objects[c1].y >> 16) + 16) & 0xfff0) << 16;
                        } else {
                            objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
                        }
                        objects[c1].y_add = -objects[c1].y_add >> 2;
                        objects[c1].y_acc = 0;
                    }
                    if (objects[c1].type == OBJ_YEL_BUTFLY) {
                        if (objects[c1].x_add < 0 && objects[c1].anim != OBJ_ANIM_YEL_BUTFLY_LEFT) {
                            objects[c1].anim = OBJ_ANIM_YEL_BUTFLY_LEFT;
                            objects[c1].frame = 0;
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        } else if (objects[c1].x_add > 0 && objects[c1].anim != OBJ_ANIM_YEL_BUTFLY_RIGHT) {
                            objects[c1].anim = OBJ_ANIM_YEL_BUTFLY_RIGHT;
                            objects[c1].frame = 0;
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    } else {
                        if (objects[c1].x_add < 0 && objects[c1].anim != OBJ_ANIM_PINK_BUTFLY_LEFT) {
                            objects[c1].anim = OBJ_ANIM_PINK_BUTFLY_LEFT;
                            objects[c1].frame = 0;
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        } else if (objects[c1].x_add > 0 && objects[c1].anim != OBJ_ANIM_PINK_BUTFLY_RIGHT) {
                            objects[c1].anim = OBJ_ANIM_PINK_BUTFLY_RIGHT;
                            objects[c1].frame = 0;
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    }
                    objects[c1].ticks--;
                    if (objects[c1].ticks <= 0) {
                        objects[c1].frame++;
                        if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
                            objects[c1].frame = object_anims[objects[c1].anim].restart_frame;
                        else {
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    }
                    if (objects[c1].used == 1)
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image,
                                &object_gobs);
                    break;
                case OBJ_FUR:
                    if (rnd(100) < 30)
                        add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0,
                                   OBJ_ANIM_FLESH_TRACE, 0);
                    if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 0) {
                        objects[c1].y_add += 3072;
                        if (objects[c1].y_add > 196608L)
                            objects[c1].y_add = 196608L;
                    } else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 2) {
                        if (objects[c1].x_add < 0) {
                            if (objects[c1].x_add < -65536L)
                                objects[c1].x_add = -65536L;
                            objects[c1].x_add += 1024;
                            if (objects[c1].x_add > 0)
                                objects[c1].x_add = 0;
                        } else {
                            if (objects[c1].x_add > 65536L)
                                objects[c1].x_add = 65536L;
                            objects[c1].x_add -= 1024;
                            if (objects[c1].x_add < 0)
                                objects[c1].x_add = 0;
                        }
                        objects[c1].y_add += 1024;
                        if (objects[c1].y_add < -65536L)
                            objects[c1].y_add = -65536L;
                        if (objects[c1].y_add > 65536L)
                            objects[c1].y_add = 65536L;
                    }
                    objects[c1].x += objects[c1].x_add;
                    if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1 ||
                                                      ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3)) {
                        if (objects[c1].x_add < 0) {
                            objects[c1].x = (((objects[c1].x >> 16) + 16) & 0xfff0) << 16;
                            objects[c1].x_add = -objects[c1].x_add >> 2;
                        } else {
                            objects[c1].x = ((((objects[c1].x >> 16) - 16) & 0xfff0) + 15) << 16;
                            objects[c1].x_add = -objects[c1].x_add >> 2;
                        }
                    }
                    objects[c1].y += objects[c1].y_add;
                    if ((objects[c1].x >> 16) < -5 || (objects[c1].x >> 16) > 405 || (objects[c1].y >> 16) > 260)
                        objects[c1].used = 0;
                    if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0)) {
                        if (objects[c1].y_add < 0) {
                            if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 2) {
                                objects[c1].y = (((objects[c1].y >> 16) + 16) & 0xfff0) << 16;
                                objects[c1].x_add >>= 2;
                                objects[c1].y_add = -objects[c1].y_add >> 2;
                            }
                        } else {
                            if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1) {
                                if (objects[c1].y_add > 131072L) {
                                    objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
                                    objects[c1].x_add >>= 2;
                                    objects[c1].y_add = -objects[c1].y_add >> 2;
                                } else
                                    objects[c1].used = 0;
                            } else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3) {
                                objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
                                if (objects[c1].y_add > 131072L)
                                    objects[c1].y_add = -objects[c1].y_add >> 2;
                                else
                                    objects[c1].y_add = 0;
                            }
                        }
                    }
                    if (objects[c1].x_add < 0 && objects[c1].x_add > -16384)
                        objects[c1].x_add = -16384;
                    if (objects[c1].x_add > 0 && objects[c1].x_add < 16384)
                        objects[c1].x_add = 16384;
                    if (objects[c1].used == 1) {
                        s1 = (int) (atan2(objects[c1].y_add, objects[c1].x_add) * 4 / M_PI);
                        if (s1 < 0)
                            s1 += 8;
                        if (s1 < 0)
                            s1 = 0;
                        if (s1 > 7)
                            s1 = 7;
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].frame + s1,
                                &object_gobs);
                    }
                    break;
                case OBJ_FLESH:
                    if (rnd(100) < 30) {
                        if (objects[c1].frame == 76)
                            add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0,
                                       OBJ_ANIM_FLESH_TRACE, 1);
                        else if (objects[c1].frame == 77)
                            add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0,
                                       OBJ_ANIM_FLESH_TRACE, 2);
                        else if (objects[c1].frame == 78)
                            add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0,
                                       OBJ_ANIM_FLESH_TRACE, 3);
                    }
                    if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 0) {
                        objects[c1].y_add += 3072;
                        if (objects[c1].y_add > 196608L)
                            objects[c1].y_add = 196608L;
                    } else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 2) {
                        if (objects[c1].x_add < 0) {
                            if (objects[c1].x_add < -65536L)
                                objects[c1].x_add = -65536L;
                            objects[c1].x_add += 1024;
                            if (objects[c1].x_add > 0)
                                objects[c1].x_add = 0;
                        } else {
                            if (objects[c1].x_add > 65536L)
                                objects[c1].x_add = 65536L;
                            objects[c1].x_add -= 1024;
                            if (objects[c1].x_add < 0)
                                objects[c1].x_add = 0;
                        }
                        objects[c1].y_add += 1024;
                        if (objects[c1].y_add < -65536L)
                            objects[c1].y_add = -65536L;
                        if (objects[c1].y_add > 65536L)
                            objects[c1].y_add = 65536L;
                    }
                    objects[c1].x += objects[c1].x_add;
                    if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1 ||
                                                      ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3)) {
                        if (objects[c1].x_add < 0) {
                            objects[c1].x = (((objects[c1].x >> 16) + 16) & 0xfff0) << 16;
                            objects[c1].x_add = -objects[c1].x_add >> 2;
                        } else {
                            objects[c1].x = ((((objects[c1].x >> 16) - 16) & 0xfff0) + 15) << 16;
                            objects[c1].x_add = -objects[c1].x_add >> 2;
                        }
                    }
                    objects[c1].y += objects[c1].y_add;
                    if ((objects[c1].x >> 16) < -5 || (objects[c1].x >> 16) > 405 || (objects[c1].y >> 16) > 260)
                        objects[c1].used = 0;
                    if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0)) {
                        if (objects[c1].y_add < 0) {
                            if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 2) {
                                objects[c1].y = (((objects[c1].y >> 16) + 16) & 0xfff0) << 16;
                                objects[c1].x_add >>= 2;
                                objects[c1].y_add = -objects[c1].y_add >> 2;
                            }
                        } else {
                            if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1) {
                                if (objects[c1].y_add > 131072L) {
                                    objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
                                    objects[c1].x_add >>= 2;
                                    objects[c1].y_add = -objects[c1].y_add >> 2;
                                } else {
                                    if (rnd(100) < 10) {
                                        s1 = rnd(4) - 2;
                                        add_leftovers(0, objects[c1].x >> 16, (objects[c1].y >> 16) + s1,
                                                      objects[c1].frame, &object_gobs);
                                        add_leftovers(1, objects[c1].x >> 16, (objects[c1].y >> 16) + s1,
                                                      objects[c1].frame, &object_gobs);
                                    }
                                    objects[c1].used = 0;
                                }
                            } else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3) {
                                objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
                                if (objects[c1].y_add > 131072L)
                                    objects[c1].y_add = -objects[c1].y_add >> 2;
                                else
                                    objects[c1].y_add = 0;
                            }
                        }
                    }
                    if (objects[c1].x_add < 0 && objects[c1].x_add > -16384)
                        objects[c1].x_add = -16384;
                    if (objects[c1].x_add > 0 && objects[c1].x_add < 16384)
                        objects[c1].x_add = 16384;
                    if (objects[c1].used == 1)
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].frame,
                                &object_gobs);
                    break;
                case OBJ_FLESH_TRACE:
                    objects[c1].ticks--;
                    if (objects[c1].ticks <= 0) {
                        objects[c1].frame++;
                        if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
                            objects[c1].used = 0;
                        else {
                            objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
                            objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
                        }
                    }
                    if (objects[c1].used == 1)
                        add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image,
                                &object_gobs);
                    break;
            }
        }
    }

}


int add_pob(int page, int x, int y, int image, gob_t *pob_data) {

    if (main_info.page_info[page].num_pobs >= main_info_t::NUM_POBS)
        return 1;

    main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].x = x;
    main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].y = y;
    main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].image = image;
    main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].pob_data = pob_data;
    main_info.page_info[page].num_pobs++;

    return 0;

}


void draw_pobs(int page) {
    int c1;
    int back_buf_ofs;

    back_buf_ofs = 0;

    for (c1 = main_info.page_info[page].num_pobs - 1; c1 >= 0; c1--) {
        main_info.page_info[page].pobs[c1].back_buf_ofs = back_buf_ofs;
        get_block(page, main_info.page_info[page].pobs[c1].x -
                        pob_hs_x(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  main_info.page_info[page].pobs[c1].y -
                  pob_hs_y(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  (unsigned char *) main_info.pob_backbuf[page] + back_buf_ofs);
        if (scale_up)
            back_buf_ofs +=
                    pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data) *
                    pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data) *
                    4;
        else
            back_buf_ofs +=
                    pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data) *
                    pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data);
        put_pob(page, main_info.page_info[page].pobs[c1].x, main_info.page_info[page].pobs[c1].y,
                main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data, 1);
    }

}


void redraw_pob_backgrounds(int page) {
    int c1;

    for (c1 = 0; c1 < main_info.page_info[page].num_pobs; c1++)
        put_block(page, main_info.page_info[page].pobs[c1].x -
                        pob_hs_x(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  main_info.page_info[page].pobs[c1].y -
                  pob_hs_y(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data),
                  (unsigned char *) main_info.pob_backbuf[page] + main_info.page_info[page].pobs[c1].back_buf_ofs);

}


int add_leftovers(int page, int x, int y, int image, gob_t *pob_data) {

    if (leftovers.page[page].num_pobs >= NUM_LEFTOVERS)
        return 1;

    leftovers.page[page].pobs[leftovers.page[page].num_pobs].x = x;
    leftovers.page[page].pobs[leftovers.page[page].num_pobs].y = y;
    leftovers.page[page].pobs[leftovers.page[page].num_pobs].image = image;
    leftovers.page[page].pobs[leftovers.page[page].num_pobs].pob_data = pob_data;
    leftovers.page[page].num_pobs++;

    return 0;

}


void draw_leftovers(int page) {
    int c1;

    for (c1 = leftovers.page[page].num_pobs - 1; c1 >= 0; c1--)
        put_pob(page, leftovers.page[page].pobs[c1].x, leftovers.page[page].pobs[c1].y,
                leftovers.page[page].pobs[c1].image, leftovers.page[page].pobs[c1].pob_data, 1);

    leftovers.page[page].num_pobs = 0;

}


int init_level(int level, char *pal) {
    unsigned char *handle;
    int c1, c2;
    int s1, s2;

    if ((handle = dat_open("level.pcx", datafile_buffer)) == 0) {
        main_info.error_str = "Error loading 'level.pcx', aborting...\n";
        return 1;
    }
    if (read_pcx(handle, background_pic, JNB_WIDTH * JNB_HEIGHT, pal) != 0) {
        main_info.error_str = "Error loading 'level.pcx', aborting...\n";
        return 1;
    }
    if (flip)
        flip_pixels(background_pic);
    if ((handle = dat_open("mask.pcx", datafile_buffer)) == 0) {
        main_info.error_str = "Error loading 'mask.pcx', aborting...\n";
        return 1;
    }
    if (read_pcx(handle, mask_pic, JNB_WIDTH * JNB_HEIGHT, 0) != 0) {
        main_info.error_str = "Error loading 'mask.pcx', aborting...\n";
        return 1;
    }
    if (flip)
        flip_pixels(mask_pic);
    register_mask(mask_pic);

    for (c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
        if (player[c1].enabled == 1) {
            player[c1].bumps = 0;
            for (c2 = 0; c2 < JNB_MAX_PLAYERS; c2++)
                player[c1].bumped[c2] = 0;
            position_player(c1);
            add_leftovers(0, 360, 34 + c1 * 64, 0, &number_gobs);
            add_leftovers(1, 360, 34 + c1 * 64, 0, &number_gobs);
            add_leftovers(0, 376, 34 + c1 * 64, 0, &number_gobs);
            add_leftovers(1, 376, 34 + c1 * 64, 0, &number_gobs);
        }
    }

    for (c1 = 0; c1 < NUM_OBJECTS; c1++)
        objects[c1].used = 0;

    for (c1 = 0; c1 < 16; c1++) {
        for (c2 = 0; c2 < 22; c2++) {
            if (ban_map[c1][c2] == BAN_SPRING)
                add_object(OBJ_SPRING, c2 << 4, c1 << 4, 0, 0, OBJ_ANIM_SPRING, 5);
        }
    }

    while (1) {
        s1 = rnd(22);
        s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_YEL_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2, (rnd(65535) - 32768) * 2,
                       0, 0);
            break;
        }
    }
    while (1) {
        s1 = rnd(22);
        s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_YEL_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2, (rnd(65535) - 32768) * 2,
                       0, 0);
            break;
        }
    }
    while (1) {
        s1 = rnd(22);
        s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_PINK_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2,
                       (rnd(65535) - 32768) * 2, 0, 0);
            break;
        }
    }
    while (1) {
        s1 = rnd(22);
        s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_PINK_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2,
                       (rnd(65535) - 32768) * 2, 0, 0);
            break;
        }
    }

    return 0;

}


void deinit_level(void) {

    dj_stop_mod(main_info);
}


#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif

static void preread_datafile(const char *fname) {
    int fd = 0;
    int len;


#ifdef BZLIB_SUPPORT
    auto bzfilename = std::string(fname) + ".bz2";
    BZFILE *bzf = BZ2_bzopen(bzfilename.c_str(), "rb");

    if (bzf != NULL) {
        int bufsize = 0;
        int bufpos = 0;
        int br;
        unsigned char *ptr;
        do {
            if (bufpos >= bufsize) {
                bufsize += 1024 * 1024;
                datafile_buffer = (unsigned char *) realloc(datafile_buffer, bufsize);
                if (datafile_buffer == NULL) {
                    perror("realloc()");
                    exit(42);
                }
            }

            br = BZ2_bzread(bzf, datafile_buffer + bufpos, bufsize - bufpos);
            if (br == -1) {
                fprintf(stderr, "gzread failed.\n");
                exit(42);
            }

            bufpos += br;
        } while (br > 0);

        /* try to shrink buffer... */
        ptr = (unsigned char *) realloc(datafile_buffer, bufpos);
        if (ptr != NULL)
            datafile_buffer = ptr;

        BZ2_bzclose(bzf);
        return;
    }

    /* drop through and try for an gzip compressed or uncompressed datafile... */
#endif

#ifdef ZLIB_SUPPORT


    auto gzfilename = std::string(fname) + ".gz";
    gzFile gzf = gzopen(gzfilename.c_str(), "rb");

    if (gzf != NULL) {
        int bufsize = 0;
        int bufpos = 0;
        unsigned char *ptr;
        do {
            int br;
            if (bufpos >= bufsize) {
                bufsize += 1024 * 1024;
                datafile_buffer = (unsigned char *) realloc(datafile_buffer, bufsize);
                if (datafile_buffer == NULL) {
                    perror("realloc()");
                    exit(42);
                }
            }

            br = gzread(gzf, datafile_buffer + bufpos, bufsize - bufpos);
            if (br == -1) {
                fprintf(stderr, "gzread failed.\n");
                exit(42);
            }

            bufpos += br;
        } while (!gzeof(gzf));

        /* try to shrink buffer... */
        ptr = (unsigned char *) realloc(datafile_buffer, bufpos);
        if (ptr != NULL)
            datafile_buffer = ptr;

        gzclose(gzf);
        return;
    }

    /* drop through and try for an uncompressed datafile... */
#endif

    fd = open(fname, O_RDONLY | O_BINARY);
    if (fd == -1) {
        fprintf(stderr, "can't open %s:", fname);
        perror("");
        exit(42);
    }

    len = filelength(fd);
    datafile_buffer = (unsigned char *) malloc(len);
    if (datafile_buffer == NULL) {
        perror("malloc()");
        close(fd);
        exit(42);
    }

    if (read(fd, datafile_buffer, len) != len) {
        perror("read()");
        close(fd);
        exit(42);
    }

    close(fd);
}


int init_program(int argc, char *argv[], char *pal) {
    unsigned char *handle = (unsigned char *) NULL;
    int c1 = 0, c2 = 0;
    int load_flag = 0;
    int player_anim_data[] = {
            1, 0, 0, 0x7fff, 0, 0, 0, 0, 0, 0,
            4, 0, 0, 4, 1, 4, 2, 4, 3, 4,
            1, 0, 4, 0x7fff, 0, 0, 0, 0, 0, 0,
            4, 2, 5, 8, 6, 10, 7, 3, 6, 3,
            1, 0, 6, 0x7fff, 0, 0, 0, 0, 0, 0,
            2, 1, 5, 8, 4, 0x7fff, 0, 0, 0, 0,
            1, 0, 8, 5, 0, 0, 0, 0, 0, 0
    };

    srand(time(NULL));

    if (hook_keyb_handler() != 0)
        return 1;

    //memset(&main_info, 0, sizeof(main_info));

    strcpy(datfile_name, DATA_PATH);

    if (argc > 1) {
        for (c1 = 1; c1 < argc; c1++) {
            if (stricmp(argv[c1], "-nosound") == 0)
                main_info.no_sound = 1;
            else if (stricmp(argv[c1], "-musicnosound") == 0)
                main_info.music_no_sound = 1;
            else if (stricmp(argv[c1], "-nomusic") == 0);
            else if (stricmp(argv[c1], "-nogore") == 0)
                main_info.no_gore = 1;
            else if (stricmp(argv[c1], "-nojoy") == 0)
                main_info.joy_enabled = 0;
            else if (stricmp(argv[c1], "-fullscreen") == 0)
                fs_toggle();
            else if (stricmp(argv[c1], "-scaleup") == 0)
                set_scaling(1);
            else if (stricmp(argv[c1], "-mirror") == 0)
                flip = 1;
            else if (stricmp(argv[c1], "-dat") == 0) {
                if (c1 < (argc - 1)) {
                    FILE *f;

                    if ((f = fopen(argv[c1 + 1], "rb")) != NULL) {
                        fclose(f);
                        strcpy(datfile_name, argv[c1 + 1]);
                    }
                }
            } else if (stricmp(argv[c1], "-player") == 0) {
                if (c1 < (argc - 1)) {
                    if (client_player_num < 0)
                        client_player_num = atoi(argv[c1 + 1]);
                }
            } else if (strstr(argv[1], "-v")) {
                printf("jumpnbump %s compiled with", JNB_VERSION);
#ifndef USE_NET
                printf("out");
#endif
                printf(" network support.\n");
                return 1;
            } else if (strstr(argv[1], "-h")) {
                printf("Usage: jumpnbump [OPTION]...\n");
                printf("\n");
                printf("  -h                       this help\n");
                printf("  -v                       print version\n");
                printf("  -dat level.dat           play a different level\n");
                printf("  -player num              set main player to num (0-3). Needed for networking\n");
                printf("  -fullscreen              run in fullscreen mode\n");
                printf("  -nosound                 play without sound\n");
                printf("  -nogore                  play without blood\n");
                printf("  -mirror                  play with mirrored level\n");
                printf("  -scaleup                 play with doubled resolution (800x512)\n");
                printf("  -musicnosound            play with music but without sound\n");
                printf("\n");
                return 1;
            }
        }
    }

    preread_datafile(datfile_name);

    main_info.pob_backbuf[0] = malloc(screen_pitch * screen_height);
    main_info.pob_backbuf[1] = malloc(screen_pitch * screen_height);

    for (c1 = 0; c1 < 7; c1++) {
        player_anims[c1].num_frames = player_anim_data[c1 * 10];
        player_anims[c1].restart_frame = player_anim_data[c1 * 10 + 1];
        for (c2 = 0; c2 < 4; c2++) {
            player_anims[c1].frame[c2].image = player_anim_data[c1 * 10 + c2 * 2 + 2];
            player_anims[c1].frame[c2].ticks = player_anim_data[c1 * 10 + c2 * 2 + 3];
        }
    }

    if ((handle = dat_open("menu.pcx", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'menu.pcx', aborting...\n";
        return 1;
    }
    if (read_pcx(handle, background_pic, JNB_WIDTH * JNB_HEIGHT, pal) != 0) {
        main_info.error_str = "Error loading 'menu.pcx', aborting...\n";
        return 1;
    }

    if ((handle = dat_open("rabbit.gob", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'rabbit.gob', aborting...\n";
        return 1;
    }
    if (register_gob(handle, &rabbit_gobs, dat_filelen("rabbit.gob", datafile_buffer))) {
        /* error */
        return 1;
    }

    if ((handle = dat_open("objects.gob", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'objects.gob', aborting...\n";
        return 1;
    }
    if (register_gob(handle, &object_gobs, dat_filelen("objects.gob", datafile_buffer))) {
        /* error */
        return 1;
    }

    if ((handle = dat_open("font.gob", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'font.gob', aborting...\n";
        return 1;
    }
    if (register_gob(handle, &font_gobs, dat_filelen("font.gob", datafile_buffer))) {
        /* error */
        return 1;
    }

    if ((handle = dat_open("numbers.gob", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'numbers.gob', aborting...\n";
        return 1;
    }
    if (register_gob(handle, &number_gobs, dat_filelen("numbers.gob", datafile_buffer))) {
        /* error */
        return 1;
    }

    if (read_level() != 0) {
        main_info.error_str = "Error loading 'levelmap.txt', aborting...\n";
        return 1;
    }

    dj_init(main_info);

    if (main_info.no_sound == 0) {

        dj_set_mixing_freq(20000);

        dj_set_num_sfx_channels(5);
        dj_set_sfx_volume(main_info, 64);

        if ((handle = dat_open("jump.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'jump.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(main_info, handle, 0, dat_filelen("jump.smp", datafile_buffer), SFX_JUMP) != 0) {
            main_info.error_str = "Error loading 'jump.smp', aborting...\n";
            return 1;
        }

        if ((handle = dat_open("death.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'death.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(main_info, handle, 0, dat_filelen("death.smp", datafile_buffer), SFX_DEATH) != 0) {
            main_info.error_str = "Error loading 'death.smp', aborting...\n";
            return 1;
        }

        if ((handle = dat_open("spring.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'spring.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(main_info, handle, 0, dat_filelen("spring.smp", datafile_buffer), SFX_SPRING) != 0) {
            main_info.error_str = "Error loading 'spring.smp', aborting...\n";
            return 1;
        }

        if ((handle = dat_open("splash.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'splash.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(main_info, handle, 0, dat_filelen("splash.smp", datafile_buffer), SFX_SPLASH) != 0) {
            main_info.error_str = "Error loading 'splash.smp', aborting...\n";
            return 1;
        }
    }

    if ((background_pic = reinterpret_cast<unsigned char *>(malloc(JNB_WIDTH * JNB_HEIGHT))) == nullptr)
        return 1;
    if ((mask_pic = reinterpret_cast<unsigned char *>(malloc(JNB_WIDTH * JNB_HEIGHT))) == nullptr)
        return 1;
    memset(mask_pic, 0, JNB_WIDTH * JNB_HEIGHT);
    register_mask(mask_pic);

    /* fix dark font */
    for (c1 = 0; c1 < 16; c1++) {
        pal[(240 + c1) * 3 + 0] = c1 << 2;
        pal[(240 + c1) * 3 + 1] = c1 << 2;
        pal[(240 + c1) * 3 + 2] = c1 << 2;
    }

    setpalette(0, 256, pal);

    init_inputs(main_info);


    if (main_info.joy_enabled == 1) {
        load_flag = 0;
        put_text(0, 200, 40, "JOYSTICK CALIBRATION", 2);
        put_text(0, 200, 100, "Move the joystick to the", 2);
        put_text(0, 200, 115, "UPPER LEFT", 2);
        put_text(0, 200, 130, "and press button A", 2);
        put_text(0, 200, 200, "Or press ESC to use", 2);
        put_text(0, 200, 215, "previous settings", 2);
        if (calib_joy() != 0)
            load_flag = 1;
        else {
            register_background(NULL);

            main_info.view_page = 1;
            flippage(1);

            wait_vrt();

            put_text(1, 200, 40, "JOYSTICK CALIBRATION", 2);
            put_text(1, 200, 100, "Move the joystick to the", 2);
            put_text(1, 200, 115, "LOWER RIGHT", 2);
            put_text(1, 200, 130, "and press button A", 2);
            put_text(1, 200, 200, "Or press ESC to use", 2);
            put_text(1, 200, 215, "previous settings", 2);
            if (calib_joy() != 0)
                load_flag = 1;
            else {
                register_background(NULL);
                flippage(0);

                wait_vrt();

                put_text(0, 200, 40, "JOYSTICK CALIBRATION", 2);
                put_text(0, 200, 100, "Move the joystick to the", 2);
                put_text(0, 200, 115, "CENTER", 2);
                put_text(0, 200, 130, "and press button A", 2);
                put_text(0, 200, 200, "Or press ESC to use", 2);
                put_text(0, 200, 215, "previous settings", 2);
                if (calib_joy() != 0)
                    load_flag = 1;
                else {
                    if (joy.calib_data.x1 == joy.calib_data.x2)
                        joy.calib_data.x1 -= 10;
                    if (joy.calib_data.x3 == joy.calib_data.x2)
                        joy.calib_data.x3 += 10;
                    if (joy.calib_data.y1 == joy.calib_data.y2)
                        joy.calib_data.y1 -= 10;
                    if (joy.calib_data.y3 == joy.calib_data.y2)
                        joy.calib_data.y3 += 10;
                    write_calib_data();
                }
            }
        }
        if (load_flag == 1) {
            if ((handle = dat_open("calib.dat", datafile_buffer)) == 0) {
                main_info.error_str = "Error loading 'calib.dat', aborting...\n";
                return 1;
            }
            joy.calib_data.x1 = (handle[0]) + (handle[1] << 8) + (handle[2] << 16) + (handle[3] << 24);
            handle += 4;
            joy.calib_data.x2 = (handle[0]) + (handle[1] << 8) + (handle[2] << 16) + (handle[3] << 24);
            handle += 4;
            joy.calib_data.x3 = (handle[0]) + (handle[1] << 8) + (handle[2] << 16) + (handle[3] << 24);
            handle += 4;
            joy.calib_data.y1 = (handle[0]) + (handle[1] << 8) + (handle[2] << 16) + (handle[3] << 24);
            handle += 4;
            joy.calib_data.y2 = (handle[0]) + (handle[1] << 8) + (handle[2] << 16) + (handle[3] << 24);
            handle += 4;
            joy.calib_data.y3 = (handle[0]) + (handle[1] << 8) + (handle[2] << 16) + (handle[3] << 24);
            handle += 4;
        }
    }

    return 0;

}

void deinit_program() {

    dj_free_sfx(main_info, SFX_DEATH);
    dj_free_sfx(main_info, SFX_SPRING);
    dj_free_sfx(main_info, SFX_SPLASH);
    dj_deinit(main_info);

    if (background_pic != 0)
        free(background_pic);
    if (mask_pic != 0)
        free(mask_pic);


    if (!main_info.error_str.empty()) {
        printf("%s", main_info.error_str.c_str());
        exit(1);
    } else
        exit(0);

}


unsigned short rnd(unsigned short max) {
#if (RAND_MAX < 0x7fff)
#error "rand returns too small values"
#elif (RAND_MAX == 0x7fff)
    return (unsigned short)((rand()*2) % (int)max);
#else
    return (unsigned short) (rand() % (int) max);
#endif
}


int read_level() {
    unsigned char *handle;
    int c1, c2;
    int chr;

    if ((handle = dat_open("levelmap.txt", datafile_buffer)) == 0) {
        main_info.error_str = "Error loading 'levelmap.txt', aborting...\n";
        return 1;
    }

    for (c1 = 0; c1 < 16; c1++) {
        for (c2 = 0; c2 < 22; c2++) {
            while (1) {
                chr = (int) *(handle++);
                if (chr >= '0' && chr <= '4')
                    break;
            }
            if (flip)
                ban_map[c1][21 - c2] = chr - '0';
            else
                ban_map[c1][c2] = chr - '0';
        }
    }

    for (c2 = 0; c2 < 22; c2++)
        ban_map[16][c2] = BAN_SOLID;

    return 0;

}


