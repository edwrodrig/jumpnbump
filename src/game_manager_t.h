//
// Created by edwin on 13-06-18.
//

#ifndef JUMPNBUMP_GAME_MANAGER_T_H
#define JUMPNBUMP_GAME_MANAGER_T_H

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "sound_manager_t.h"
#include "pobs_t.h"
#include "player_t.h"
#include "stage_t.h"
#include "anim_t.h"
#include "gob_t.h"
#include "objects_t.h"
#include "character_t.h"
#include <memory>
#include <SFML/Graphics/Font.hpp>

class game_manager_t {
public:
    sf::Clock clock;
    int frame_counter = 0;
    sf::RenderWindow& window;

    bool gore = true;

    sf::Texture object_texture;

    std::vector<std::shared_ptr<character_t>> characters;

    sf::Font font;

    stage_t stage;

    sound_manager_t sound_manager;

    pobs_t pobs;
    pobs_t leftovers;

    std::vector<player_t> players;

    std::vector<anim_t> object_anims;
    gob_t object_gobs;
    objects_t objects;

    game_manager_t(sf::RenderWindow& window);

    void init_textures();

    void init_players();
    void init_character();
    void init_deprecated_data();

    void collision_check();

    void draw();
    void reset_frames();
    int get_elapsed_frames();

    void process_input();
    stage_t& get_stage();

    bool init();
    void kill(int killer, int victim);

    void loop();

    void steer_players();

    void draw_hud();

    void fx_splash(const screen_position_t& position);
};


#endif //JUMPNBUMP_GAME_MANAGER_T_H
