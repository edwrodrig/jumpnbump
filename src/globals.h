/*
 * globals.h
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

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <SDL.h>

struct main_info_t;
struct gob_t;
#include <string>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <strings.h>
#endif
#include <time.h>
#include <math.h>
#include <src/dj.h>


#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <sys/stat.h>
# include <io.h>
# include "SDL.h"
# if USE_SDL_MIXER
#  include "SDL_mixer.h"
# endif
#else
# ifdef USE_SDL
#  include <sys/stat.h>
#include "player_t.h"
#include "anim_t.h"

#include "object_t.h"
#include "joy_t.h"

#  if USE_SDL_MIXER
#   include "SDL_mixer.h"
#  endif
# endif
#endif

#define JNB_END_SCORE 100

extern int client_player_num;
void tellServerPlayerMoved(int playerid, int movement_type, int newval);
#define MOVEMENT_LEFT  1
#define MOVEMENT_RIGHT 2
#define MOVEMENT_UP    3

#define JNB_VERSION "1.60"

#define JNB_WIDTH 400
#define JNB_HEIGHT 256

extern int screen_width;
extern int screen_height;
extern int screen_pitch;
extern int scale_up;

#define KEY_PL1_LEFT	SDL_SCANCODE_LEFT
#define KEY_PL1_RIGHT	SDL_SCANCODE_RIGHT
#define KEY_PL1_JUMP	SDL_SCANCODE_UP
#define KEY_PL2_LEFT	SDL_SCANCODE_A
#define KEY_PL2_RIGHT	SDL_SCANCODE_D
#define KEY_PL2_JUMP	SDL_SCANCODE_W
#define KEY_PL3_LEFT	SDL_SCANCODE_J
#define KEY_PL3_RIGHT	SDL_SCANCODE_L
#define KEY_PL3_JUMP	SDL_SCANCODE_I
#define KEY_PL4_LEFT	SDL_SCANCODE_KP_4
#define KEY_PL4_RIGHT	SDL_SCANCODE_KP_6
#define KEY_PL4_JUMP	SDL_SCANCODE_KP_8

#define OBJ_SPRING 0
#define OBJ_SPLASH 1
#define OBJ_SMOKE 2
#define OBJ_YEL_BUTFLY 3
#define OBJ_PINK_BUTFLY 4
#define OBJ_FUR 5
#define OBJ_FLESH 6
#define OBJ_FLESH_TRACE 7

#define OBJ_ANIM_SPRING 0
#define OBJ_ANIM_SPLASH 1
#define OBJ_ANIM_SMOKE 2
#define OBJ_ANIM_YEL_BUTFLY_RIGHT 3
#define OBJ_ANIM_YEL_BUTFLY_LEFT 4
#define OBJ_ANIM_PINK_BUTFLY_RIGHT 5
#define OBJ_ANIM_PINK_BUTFLY_LEFT 6
#define OBJ_ANIM_FLESH_TRACE 7

#define MOD_MENU 0
#define MOD_GAME 1
#define MOD_SCORES 2

#define SFX_JUMP 0
#define SFX_DEATH 2
#define SFX_SPRING 3
#define SFX_SPLASH 4

#define NUM_SFX 6

#define SFX_JUMP_FREQ 15000
#define SFX_DEATH_FREQ 20000
#define SFX_SPRING_FREQ 15000
#define SFX_SPLASH_FREQ 12000

extern std::vector<object_t> objects;
extern joy_t joy;

extern char datfile_name[2048];

extern unsigned char *background_pic;
extern unsigned char *mask_pic;

void update_objects(void);
void update_player_actions();

int init_level(char *pal);
void deinit_level(void);
int init_program(int argc, char *argv[], char *pal);
void deinit_program(void);
/* input.c */

void init_inputs(main_info_t&);
int calib_joy();

/* gfx.c */

void set_scaling(int scale);
void open_screen(void);
void wait_vrt();
void draw_begin(void);
void draw_end(void);
void flippage(int page);
void draw_begin(void);
void draw_end(void);
void clear_lines(int page, int y, int count, int color);
void setpalette(int index, int count, char *palette);
void fillpalette(int red, int green, int blue);

void get_block(int page, int x, int y, int width, int height, unsigned char *buffer);
void put_block(int page, int x, int y, int width, int height, unsigned char *buffer);

int read_pcx(const std::string& filename, unsigned char *buffer, int buf_len, char *pal);
void register_background(unsigned char *pixels);

void put_text(int page, int x, int y, const char *text, int align);
void register_mask(void *pixels);

/* interrpt.c */

int hook_keyb_handler(void);

int key_pressed(int key);
int addkey(unsigned int key);


#endif
