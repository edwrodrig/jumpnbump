//
// Created by edwin on 02-06-18.
//

#ifndef JUMPNBUMP_OBJECT_T_H
#define JUMPNBUMP_OBJECT_T_H

class player_t;

#include "position_t.h"
#include "anim_t.h"
#include "anim_handler_t.h"

class game_manager_t;

struct object_t{

	enum class Type {
		SPRING = 0,
		SPLASH = 1,
		SMOKE = 2,
		YEL_BUTFLY = 3,
		PINK_BUTFLY = 4,
		FUR = 5,
		FLESH = 6,
		FLESH_TRACE = 7
	};

	int used;

	Type type;

	game_manager_t& game_manager;

	position_t position;

	int x_add, y_add;
	int x_acc, y_acc;

	anim_handler_t anim_handler;

	object_t(game_manager_t& game_manager, object_t::Type type, const position_t& position, int x_add, int y_add, int anim, int frame);
	void operator=(const object_t& other);

	/**
	 * Read only
	 * @return
	 */
	position_t get_position() const {
		return this->position;
	}


	void update_flesh_trace();
	void update_smoke();
	void update_spring();
	void update_splash();
	void update_butterfly();
	void update_flesh(game_manager_t& game_manager);
	void update_fur();

	/**
	 * Is this element an active element?
	 * @return
	 */
	bool is_used() const { return this->used == 1; }

	/**
	 * Is this element available to create a new element?
	 * @return
	 */
	bool is_available() const { return this->used == 0; }

	/**
	 * Set this element available.
	 *
	 * Use this function when you want to free this element as a new element
	 */
	void set_available() { this->used = 0; }

	void set_anim(int anim, int frame);

	void advance_anim();
};

#endif //JUMPNBUMP_OBJECT_T_H
