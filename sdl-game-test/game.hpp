/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2021 EPAM SYSTEMS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GAME_HPP
#define GAME_HPP

#include <SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> 
#include <SDL2/SDL_timer.h>
#include <fstream>
#include <vector>
#include <string>
#include "object.hpp"
#include "entity.hpp"

class Game
{

public:
    Game();
    ~Game();

    void update();
    void mainLoop();
    void keyInput();
    void render();
    void drawObject(const Object& obj);
    void loadMap(const std::string& s);
    void drawMap();
    void drawMsg(const std::string& msg, int x, int y, int r, int g, int b);
    bool mapCollision(const Object& obj1, int xPositionDelta);

private:
    TTF_Font* font{nullptr};
    std::vector<Object> backgroundMap;
    bool running{true};
    SDL_Window* window{nullptr};
    SDL_Renderer* rend{nullptr};
    Entity player;
    int playerSpeed{5};

    int timeSinceLastFrame{0};
    int lastFrame{0};

    bool left{false};
    bool right{false};
    int idle, runLeft, runRight;
};


#endif // GAME_HPP
