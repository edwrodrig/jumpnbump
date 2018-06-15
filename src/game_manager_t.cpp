//
// Created by edwin on 13-06-18.
//

#include "game_manager_t.h"
#include "gob_t.h"
#include "leftovers_t.h"
#include "player_t.h"
#include "main_info.h"
#include "anim_t.h"
#include "data.h"
#include "globals.h"
#include "ban_map.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics/Sprite.hpp>

std::unique_ptr<game_manager_t> external_game_manager = nullptr;

void game_manager_t::init_window() {
    this->window.create(sf::VideoMode(400, 256), "My window");
}

void game_manager_t::init_textures() {

    this->object_texture.loadFromFile("/home/edwin/Projects/jumpnbump/data/objects.png");
    this->background_texture.loadFromFile("/home/edwin/Projects/jumpnbump/data/level.png");
    this->foreground_texture.loadFromFile("/home/edwin/Projects/jumpnbump/data/mask.png");
    this->rabbit_texture.loadFromFile("/home/edwin/Projects/jumpnbump/data/rabbit.png");

}

void game_manager_t::init_deprecated_data() {
    std::string datfile_name = "/home/edwin/Projects/jumpnbump/data/jumpbump.dat";

    player_anims = {
            {0, {{ 0, 0x7fff}}},
            {0, {{0, 4}, {1, 4}, {2, 4}, {3, 4}}},
            {0, {{4, 0x7fff}}},
            {2, {{5, 8}, {6, 10}, {7, 3}, {6, 3}}},
            {0, {{6, 0x7fff}}},
            {1, {{5, 8}, {4, 0x7fff}}},
            {0, {{8, 5}}}
    };



    rabbit_gobs.add("/home/edwin/Projects/jumpnbump/data/rabbit.json", this->rabbit_texture);


    object_gobs.add("/home/edwin/Projects/jumpnbump/data/objects.json", this->object_texture);


    if (!ban_map.read_from_file("/home/edwin/Projects/jumpnbump/data/levelmap.txt")) {
        printf("Error loading 'rabbit.gob', aborting...\n");
        return;
    }
}

void game_manager_t::draw() {
    this->window.clear();
    sf::Sprite background(this->background_texture);

    this->window.draw(background);


    for (int i = 0 ; i < players.size(); i++) {
        main_info.pobs.add(players[i].get_position(), players[i].anim_handler.image + i * 18, &rabbit_gobs);
    }

    main_info.pobs.draw();
    leftovers.draw();

    sf::Sprite foreground(this->foreground_texture);
    this->window.draw(foreground);

    this->window.display();

}

void game_manager_t::draw_pob(const pob_t& pob)
{
    auto& image = pob.pob_data->images[pob.image];
    image.setPosition(pob.position.x, pob.position.y);
    external_game_manager->window.draw(image);

}

void game_manager_t::reset_frames() {
    this->clock.restart();
    this->frame_counter= 0;
}

int game_manager_t::get_elapsed_frames() {
    auto elapsed = this->clock.getElapsedTime();

    const auto milliseconds_per_frame = 1000 / 60;

    auto frames = elapsed.asMilliseconds() / milliseconds_per_frame;;
    if ( this->frame_counter < frames ) {
        auto elapsed_frames = frames - this->frame_counter;
        this->frame_counter = frames;
        return elapsed_frames;
    } else {
        return 0;
    }

}

void game_manager_t::process_input() {
    sf::Event event;
    while (this->window.pollEvent(event))
    {
        // Close window: exit
        if (event.type == sf::Event::Closed)
            this->window.close();
        if ( event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased ) {
            if ( event.key.code == sf::Keyboard::Key::Escape ) {
                this->window.close();
            } else {
                if (event.type == sf::Event::KeyReleased)
                    addkey(event.key.code | 0x8000);
                else {
                    addkey(event.key.code | 0x7fff);
                }
            }
        }
    }
/*
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    SDL_Quit();
                    exit(1);
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if (e.key.repeat != 0) {
                        continue;
                    }
                    switch (e.key.keysym.scancode) {
                        default:
                            e.key.keysym.scancode = static_cast<SDL_Scancode>(e.key.keysym.scancode & 0x7fff);
                            if (e.type == SDL_KEYUP)
                                e.key.keysym.scancode = static_cast<SDL_Scancode>(e.key.keysym.scancode | 0x8000);
                            addkey(e.key.keysym.scancode);
                            break;
                            /*
                    }
                    break;
                default:
                    break;
            }
            i++;
        }

    }*/
}