/*
 * input.c
 * Copyright (C) 1998 Brainchild Design - http://brainchilddesign.com/
 * 
 * Copyright (C) 2001 Chuck Mason <cemason@users.sourceforge.net>
 *
 * Copyright (C) 2002 Florian Schulze <crow@icculus.org>
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

#include "src/globals.h"
#include "src/player_t.h"
#include "src/main_info.h"

static int num_joys=0;

void update_player_actions()
{
	int tmp;

	tmp = (key_pressed(KEY_PL1_LEFT) == 1);
	if (tmp != players[0].action_left)
		tellServerPlayerMoved(0, MOVEMENT_LEFT, tmp);
	tmp = (key_pressed(KEY_PL1_RIGHT) == 1);
	if (tmp != players[0].action_right)
		tellServerPlayerMoved(0, MOVEMENT_RIGHT, tmp);
	tmp = (key_pressed(KEY_PL1_JUMP) == 1);
	if (tmp != players[0].action_up)
		tellServerPlayerMoved(0, MOVEMENT_UP, tmp);

	tmp = (key_pressed(KEY_PL2_LEFT) == 1);
	if (tmp != players[1].action_left)
		tellServerPlayerMoved(1, MOVEMENT_LEFT, tmp);
	tmp = (key_pressed(KEY_PL2_RIGHT) == 1);
	if (tmp != players[1].action_right)
		tellServerPlayerMoved(1, MOVEMENT_RIGHT, tmp);
	tmp = (key_pressed(KEY_PL2_JUMP) == 1);
	if (tmp != players[1].action_up)
		tellServerPlayerMoved(1, MOVEMENT_UP, tmp);

	tmp = (key_pressed(KEY_PL3_LEFT) == 1);
	if (tmp != players[2].action_left)
		tellServerPlayerMoved(2, MOVEMENT_LEFT, tmp);
	tmp = (key_pressed(KEY_PL3_RIGHT) == 1);
	if (tmp != players[2].action_right)
		tellServerPlayerMoved(2, MOVEMENT_RIGHT, tmp);
	tmp = (key_pressed(KEY_PL3_JUMP) == 1);
	if (tmp != players[2].action_up)
		tellServerPlayerMoved(2, MOVEMENT_UP, tmp);

	tmp = (key_pressed(KEY_PL4_LEFT) == 1);
	if (tmp != players[3].action_left)
		tellServerPlayerMoved(3, MOVEMENT_LEFT, tmp);
	tmp = (key_pressed(KEY_PL4_RIGHT) == 1);
	if (tmp != players[3].action_right)
		tellServerPlayerMoved(3, MOVEMENT_RIGHT, tmp);
	tmp = (key_pressed(KEY_PL4_JUMP) == 1);
	if (tmp != players[3].action_up)
		tellServerPlayerMoved(3, MOVEMENT_UP, tmp);
}

void tellServerPlayerMoved(int playerid, int movement_type, int newval)
{


	int movetype = movement_type;

	if (movetype == MOVEMENT_LEFT) {
		players[playerid].action_left = newval;
	} else if (movetype == MOVEMENT_RIGHT) {
		players[playerid].action_right = newval;
	} else if (movetype == MOVEMENT_UP) {
		players[playerid].action_up = newval;
	} else {
		printf("bogus MOVE packet!\n");
	}
}