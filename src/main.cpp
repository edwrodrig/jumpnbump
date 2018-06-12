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
#include "gob_t.h"
#include "anim_t.h"
#include "leftovers_t.h"
#include "util.h"
#include "player_t.h"
#include "data.h"
#include "ban_map.h"
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include "sdl/gfx.h"
#include "object_t.h"
#include "anim_t.h"
#include "joy_t.h"
#include "main_info.h"
#include "objects_t.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
int endscore_reached;


unsigned char *datafile_buffer = nullptr;

joy_t joy;

leftovers_t leftovers;

int pogostick, bunnies_in_space, jetpack, blood_is_thicker_than_water;

int client_player_num = -1;

void serverSendKillPacket(int killer, int victim) {
    int c1 = killer;
    int c2 = victim;
    int c4 = 0;

    players[c1].y_add = -players[c1].y_add;
    if (players[c1].y_add > -262144L)
        players[c1].y_add = -262144L;
    players[c1].jump_abort = 1;
    players[c2].dead_flag = 1;
    if (players[c2].anim_handler.anim != 6) {
        players[c2].set_anim(6);
        if (main_info.gore ) {
            auto screen_position = screen_position_t{players[victim].get_position()} + screen_position_t{6 + rnd(5), 6 + rnd(5)};
            for (c4 = 0; c4 < 6; c4++)
                objects.add(object_t::Type::FUR, screen_position, (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 44 + c2 * 8);
            for (c4 = 0; c4 < 6; c4++)
                objects.add(object_t::Type::FLESH, screen_position, (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 76);
            for (c4 = 0; c4 < 6; c4++)
                objects.add(object_t::Type::FLESH, screen_position, (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 77);
            for (c4 = 0; c4 < 8; c4++)
                objects.add(object_t::Type::FLESH, screen_position, (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 78);
            for (c4 = 0; c4 < 10; c4++)
                objects.add(object_t::Type::FLESH, screen_position, (rnd(65535) - 32768) * 3,
                           (rnd(65535) - 32768) * 3, 0, 79);
        }
        dj_play_sfx(SFX_DEATH, (unsigned short) (SFX_DEATH_FREQ + rnd(2000) - 1000), 64, 0, -1);


        players[c1].count_kill(c2);
        if (players[c1].bumps >= JNB_END_SCORE) {
            endscore_reached = 1;
        }
    }
}

static void game_loop(void) {

    int update_count = 1;
    int end_loop_flag = 0;
    int i;

    dj_ready_mod(datafile_buffer);
    dj_set_mod_volume((char) 30);
    dj_set_sfx_volume((char) 64);
    dj_start_mod();

    intr_sysupdate();

    endscore_reached = 0;

    //set_blood_is_thicker_than_water();
    while (true) {
        while (update_count) {

            if (endscore_reached || (key_pressed(1) == 1)) {
                end_loop_flag = 1;
            }


            steer_players();



            collision_check();


            objects.update();


            if (update_count == 1) {

                for (int i = 0 ; i < players.size(); i++) {
                    main_info.pobs.add(players[i].get_position(), players[i].anim_handler.image + i * 18, &rabbit_gobs);
                }

                draw_begin();

                main_info.pobs.draw();
                leftovers.draw();
                flippage();

                draw_end();


            }

            update_count--;
        }

        update_count = intr_sysupdate();

        if (end_loop_flag == 1)
            break;
    }
}


static int menu_loop() {

        init_players();

        if (init_level() != 0) {
            deinit_level();
        }


        bunnies_in_space = jetpack = pogostick = blood_is_thicker_than_water = 0;
        //blood_is_thicker_than_water = 1; HERE IS TO MOD THE CHEATS

        game_loop();

        dj_stop_sfx_channel(4);

        deinit_level();

}


int main(int argc, char *argv[]) {
    int result;

    if (init_program(argc, argv) == 0) {

        result = menu_loop();

    }

    return result;
}

int init_level() {

    unsigned char *background_pic = reinterpret_cast<unsigned char *>(malloc(JNB_WIDTH * JNB_HEIGHT));
    unsigned char *mask_pic = reinterpret_cast<unsigned char *>(malloc(JNB_WIDTH * JNB_HEIGHT));

    memset(mask_pic, 0, JNB_WIDTH * JNB_HEIGHT);

    if (read_pcx("/home/edwin/Projects/jumpnbump/data/level.pcx", background_pic, JNB_WIDTH * JNB_HEIGHT, true) != 0) {
        main_info.error_str = "Error loading 'level.pcx', aborting...\n";
        return 1;
    }
    register_background(background_pic);

    if (read_pcx("/home/edwin/Projects/jumpnbump/data/mask.pcx", mask_pic, JNB_WIDTH * JNB_HEIGHT) != 0) {
        main_info.error_str = "Error loading 'mask.pcx', aborting...\n";
        return 1;
    }
    register_mask(mask_pic);
    free(background_pic);
    free(mask_pic);

    for (auto& player : players) {
            player.reset_kills();
            position_player(player);
    }

    for (int c1 = 0; c1 < ban_map.get_height(); c1++) {
        for (int c2 = 0; c2 < ban_map.get_width() ; c2++) {
            if (ban_map.get(map_position_t{c2, c1}) == ban_map_t::Type::SPRING)
                objects.add(object_t::Type::SPRING, map_position_t{c2, c1}, 0, 0, OBJ_ANIM_SPRING, 5);
        }
    }

    for ( int i = 0 ; i < 2 ; i++ ) {
        auto new_pos = ban_map.get_random_available_position();
        objects.add(object_t::Type::YEL_BUTFLY, screen_position_t{8, 8} + new_pos , (rnd(65535) - 32768) * 2,
                   (rnd(65535) - 32768) * 2,
                   0, 0);
    }

    for ( int i = 0 ; i < 2 ; i++ ) {
        auto new_pos = ban_map.get_random_available_position();
        objects.add(object_t::Type::PINK_BUTFLY,  screen_position_t{8, 8} + new_pos, (rnd(65535) - 32768) * 2,
                   (rnd(65535) - 32768) * 2, 0, 0);
    }


    return 0;

}


void deinit_level(void) {

    dj_stop_mod();
}


#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif

static void preread_datafile(const std::string& fname) {
    int fd = 0;
    int len;


    fd = open(fname.c_str(), O_RDONLY | O_BINARY);
    if (fd == -1) {
        fprintf(stderr, "can't open %s:", fname.c_str());
        perror("");
        exit(42);
    }

    len = filelength(fd);
    datafile_buffer = (unsigned char *) malloc(len);
    if (datafile_buffer == nullptr) {
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


int init_program(int argc, char *argv[]) {
    unsigned char *handle = nullptr;
    int load_flag = 0;
    main_info.music_no_sound =0;
    main_info.no_sound = 0;

    srand(time(NULL));

    std::string datfile_name = "/home/edwin/Projects/jumpnbump/data/jumpbump.dat";

    if (argc > 1) {
        for (auto c1 = 1; c1 < argc; c1++) {
            if (stricmp(argv[c1], "-nosound") == 0)
                main_info.no_sound = 1;
            else if (stricmp(argv[c1], "-musicnosound") == 0)
                main_info.music_no_sound = 1;
            else if (stricmp(argv[c1], "-nomusic") == 0);
            else if (stricmp(argv[c1], "-nogore") == 0)
                main_info.gore = false;
            else if (stricmp(argv[c1], "-nojoy") == 0)
                main_info.joy_enabled = 0;
             else if (stricmp(argv[c1], "-players") == 0) {
                if (c1 < (argc - 1)) {
                    if (client_player_num < 0)
                        client_player_num = atoi(argv[c1 + 1]);
                }
            } else if (strstr(argv[1], "-v")) {
                printf("jumpnbump %s compiled with", JNB_VERSION);
                printf(" network support.\n");
                return 1;
            } else if (strstr(argv[1], "-h")) {
                printf("Usage: jumpnbump [OPTION]...\n");
                printf("\n");
                printf("  -h                       this help\n");
                printf("  -v                       print version\n");
                printf("  -dat level.dat           play a different level\n");
                printf("  -players num              set main players to num (0-3). Needed for networking\n");
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

    player_anims = {
            {0, {{ 0, 0x7fff}}},
            {0, {{0, 4}, {1, 4}, {2, 4}, {3, 4}}},
            {0, {{4, 0x7fff}}},
            {2, {{5, 8}, {6, 10}, {7, 3}, {6, 3}}},
            {0, {{6, 0x7fff}}},
            {1, {{5, 8}, {4, 0x7fff}}},
            {0, {{8, 5}}}
    };


    if ((handle = dat_open("rabbit.gob", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'rabbit.gob', aborting...\n";
        return 1;
    }
    if (register_gob(handle, rabbit_gobs, dat_filelen("rabbit.gob", datafile_buffer))) {
        /* error */
        return 1;
    }

    if ((handle = dat_open("objects.gob", datafile_buffer)) == nullptr) {
        main_info.error_str = "Error loading 'objects.gob', aborting...\n";
        return 1;
    }
    if (register_gob(handle, object_gobs, dat_filelen("objects.gob", datafile_buffer))) {
        /* error */
        return 1;
    }

    if (!ban_map.read_from_file("/home/edwin/Projects/jumpnbump/data/levelmap.txt")) {
        main_info.error_str = "Error loading 'levelmap.txt', aborting...\n";
        return 1;
    }

    open_screen();
    dj_init();

    if (main_info.no_sound == 0) {

        dj_set_mixing_freq(20000);

        dj_set_num_sfx_channels(5);
        dj_set_sfx_volume(64);

        if ((handle = dat_open("jump.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'jump.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(handle, dat_filelen("jump.smp", datafile_buffer), SFX_JUMP) != 0) {
            main_info.error_str = "Error loading 'jump.smp', aborting...\n";
            return 1;
        }

        if ((handle = dat_open("death.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'death.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(handle, dat_filelen("death.smp", datafile_buffer), SFX_DEATH) != 0) {
            main_info.error_str = "Error loading 'death.smp', aborting...\n";
            return 1;
        }

        if ((handle = dat_open("spring.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'spring.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(handle, dat_filelen("spring.smp", datafile_buffer), SFX_SPRING) != 0) {
            main_info.error_str = "Error loading 'spring.smp', aborting...\n";
            return 1;
        }

        if ((handle = dat_open("splash.smp", datafile_buffer)) == 0) {
            main_info.error_str = "Error loading 'splash.smp', aborting...\n";
            return 1;
        }
        if (dj_load_sfx(handle, dat_filelen("splash.smp", datafile_buffer), SFX_SPLASH) != 0) {
            main_info.error_str = "Error loading 'splash.smp', aborting...\n";
            return 1;
        }
    }

    init_inputs(main_info);


    if (main_info.joy_enabled == 1) {
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
    }

    return 0;

}



