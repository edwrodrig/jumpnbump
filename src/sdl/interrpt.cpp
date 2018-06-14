/*
 * interrpt.c
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

#ifndef _WIN32

#include <unistd.h>

#endif

#include "src/globals.h"
#include "src/sdl/gfx.h"

char keyb[256];

int addkey(unsigned int key) {
    if (!(key & 0x8000)) {
        keyb[key & 0x7fff] = 1;
    } else
        keyb[key & 0x7fff] = 0;
    return 0;
}

int key_pressed(int key) {
    return keyb[(unsigned char) key];
}

int intr_sysupdate() {
    SDL_Event e;
    int i = 0;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                SDL_Quit();
                exit(1);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (e.button.state == SDL_PRESSED &&
                    ((key_pressed(KEY_PL3_LEFT) && e.button.button == SDL_BUTTON_RIGHT) ||
                     (key_pressed(KEY_PL3_RIGHT) && e.button.button == SDL_BUTTON_LEFT) ||
                     (e.button.button == SDL_BUTTON_LEFT && e.button.button == SDL_BUTTON_RIGHT) ||
                     e.button.button == SDL_BUTTON_MIDDLE)) {
                    addkey(KEY_PL3_JUMP & 0x7fff);
                } else if (e.button.state == SDL_RELEASED &&
                           ((key_pressed(KEY_PL3_LEFT) && e.button.button == SDL_BUTTON_RIGHT) ||
                            (key_pressed(KEY_PL3_RIGHT) && e.button.button == SDL_BUTTON_LEFT) ||
                            e.button.button == SDL_BUTTON_MIDDLE)) {
                    addkey((KEY_PL3_JUMP & 0x7fff) | 0x8000);
                }

                if (e.button.button == SDL_BUTTON_LEFT) {
                    SDL_Scancode scancode = KEY_PL3_LEFT;
                    scancode = static_cast<SDL_Scancode>(scancode & 0x7fff);
                    if (e.button.state == SDL_RELEASED) {
                        if (key_pressed(KEY_PL3_JUMP) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)))
                            addkey(KEY_PL3_RIGHT & 0x7fff);
                        else
                            scancode = static_cast<SDL_Scancode>(scancode | 0x8000);
                    }
                    addkey(scancode);
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    SDL_Scancode scancode = KEY_PL3_RIGHT;
                    scancode = static_cast<SDL_Scancode>(scancode & 0x7fff);
                    if (e.button.state == SDL_RELEASED) {
                        if (key_pressed(KEY_PL3_JUMP) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
                            addkey(KEY_PL3_LEFT & 0x7fff);
                        else
                            scancode = static_cast<SDL_Scancode>(scancode | 0x8000);
                    }
                    addkey(scancode);
                }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (e.key.repeat != 0) {
                    continue;
                }
                switch (e.key.keysym.scancode) {
                    case SDL_SCANCODE_F12:
                        if (e.type == SDL_KEYDOWN) {
                            SDL_Quit();
                            exit(1);
                        }
                        break;
                    case SDL_SCANCODE_F10:
                        if (e.type == SDL_KEYDOWN) {
                            fs_toggle();
                        }
                        break;
                    case SDL_SCANCODE_1:
                        /* Release keys, otherwise it will continue moving that way */
                        addkey((KEY_PL1_LEFT & 0x7fff) | 0x8000);
                        addkey((KEY_PL1_RIGHT & 0x7fff) | 0x8000);
                        addkey((KEY_PL1_JUMP & 0x7fff) | 0x8000);
                        break;
                    case SDL_SCANCODE_2:

                        /* Release keys, otherwise it will continue moving that way */
                        addkey((KEY_PL2_LEFT & 0x7fff) | 0x8000);
                        addkey((KEY_PL2_RIGHT & 0x7fff) | 0x8000);
                        addkey((KEY_PL2_JUMP & 0x7fff) | 0x8000);
                        break;
                    case SDL_SCANCODE_3:
                        /* Release keys, otherwise it will continue moving that way */
                        addkey((KEY_PL3_LEFT & 0x7fff) | 0x8000);
                        addkey((KEY_PL3_RIGHT & 0x7fff) | 0x8000);
                        addkey((KEY_PL3_JUMP & 0x7fff) | 0x8000);
                        break;
                    case SDL_SCANCODE_4:
                        /* Release keys, otherwise it will continue moving that way */
                        addkey((KEY_PL4_LEFT & 0x7fff) | 0x8000);
                        addkey((KEY_PL4_RIGHT & 0x7fff) | 0x8000);
                        addkey((KEY_PL4_JUMP & 0x7fff) | 0x8000);
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        if (e.type == SDL_KEYUP)
                            addkey(1 | 0x8000);
                        else
                            addkey(1 & 0x7fff);
                        break;
                    default:
                        e.key.keysym.scancode = static_cast<SDL_Scancode>(e.key.keysym.scancode & 0x7fff);
                        if (e.type == SDL_KEYUP)
                            e.key.keysym.scancode = static_cast<SDL_Scancode>(e.key.keysym.scancode | 0x8000);
                        addkey(e.key.keysym.scancode);
                        break;
                }
                break;
            default:
                break;
        }
        i++;
    }

}
