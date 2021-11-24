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

#include "game.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::string;

static const int TILE_WIDTH = 16;
static const int TILE_HEIGHT = 16;
static const int TILE_GRID_SIZE = 8;
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const int PLAYER_WIDTH = 24;
static const int PLAYER_HEIGHT = 26;

Game::Game()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    { 
        cout << "error initializing SDL: " << SDL_GetError() << endl;
        throw std::runtime_error("SDL_Init failed");
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

    window = SDL_CreateWindow("GAME", // creates a window 
                                    0, 
                                    0, 
                                    WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == nullptr)
    {
        cout << "Error during SDL window creation" << endl;
        throw std::runtime_error("SDL_CreateWindow failed");
    }


    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 

    if (rend == nullptr)
    {
        cout << "Error during SDL renderer creation" << endl;
        throw std::runtime_error("SDL_CreateRenderer failed");
    }

    TTF_Init();
    font = TTF_OpenFont("/usr/share/fonts/truetype/AbyssinicaSIL-R.ttf", 32);

    if (font == nullptr)
    {
        throw std::runtime_error("TTF_OpenFont failed");
    }

    player.setImage("/usr/share/resources/player.png", rend);
    player.setDest(Object::Coordinates{100,375,PLAYER_WIDTH*3,PLAYER_HEIGHT*3});

    idle = player.createAnimation(1, PLAYER_WIDTH, PLAYER_HEIGHT, 4, 20);
    runRight = player.createAnimation(2, PLAYER_WIDTH, PLAYER_HEIGHT, 5, 10);
    runLeft = player.createAnimation(3, PLAYER_WIDTH, PLAYER_HEIGHT, 5, 10);

    player.setCurAnimation(idle);

    loadMap("/usr/share/resources/1.level");

    mainLoop();

}

Game::~Game()
{
    // destroy renderer 
    SDL_DestroyRenderer(rend); 

    // destroy window 
    SDL_DestroyWindow(window); 

    // quit
    TTF_Quit();
    SDL_Quit();

}

void Game::mainLoop()
{
    while(running)
    {
        render();
        keyInput();
        update();
    }
}

void Game::render()
{

    if (SDL_SetRenderDrawColor(rend, 126, 192, 238, 255) != 0)
    {
        cout << "error SDL_SetRenderDrawColor: " << SDL_GetError() << endl;
    }

    if (SDL_RenderClear(rend) != 0)
    {
        cout << "error SDL_RenderClear: " << SDL_GetError() << endl;
    }

    drawMap();

    drawObject(player);

    drawMsg("DAC example application", 170, 100, 255, 255, 255);

    SDL_RenderPresent(rend);

    timeSinceLastFrame = SDL_GetTicks() - lastFrame;
    if (timeSinceLastFrame < (1000/60))
    {
        SDL_Delay((1000/60) - timeSinceLastFrame); 
    }
    lastFrame = SDL_GetTicks();
}

void Game::drawObject(const Object& obj)
{
    SDL_Rect src = obj.getSrc();
    SDL_Rect dest = obj.getDest();

    SDL_RenderCopy(rend, obj.getTex(), &src, &dest);
}

void Game::keyInput()
{
    SDL_Event event; 

    while (SDL_PollEvent(&event)) {

        switch (event.type) {

        case SDL_QUIT:
            // handling of close button
            running = false;
            break;

        case SDL_KEYDOWN:
            // keyboard API for key pressed
            switch (event.key.keysym.sym)
            {
            case SDLK_a:
            case SDLK_LEFT:
                left = true;
                right = false;
                break; 
            case SDLK_d:
            case SDLK_RIGHT:
                left = false;
                right = true;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
                case SDLK_a: 
                case SDLK_LEFT: 
                    left = false;
                    player.setCurAnimation(idle);
                    break; 
                case SDLK_d: 
                case SDLK_RIGHT: 
                    right = false;
                    player.setCurAnimation(idle);
                    break;
            }
            break;
        }
    }
}

void Game::update()
{
    if(left)
    {
        if(player.getCurAnimation() != runLeft)
        {
            player.setCurAnimation(runLeft);
        }
        if(!mapCollision(player, -playerSpeed))
        {
            player.setDest(Object::Coordinates{player.getDest().x - playerSpeed, player.getDest().y, player.getDest().w, player.getDest().h});
        }
    }
    if(right)
    {
        if(player.getCurAnimation() != runRight)
        {
            player.setCurAnimation(runRight);
        }
        if(!mapCollision(player, playerSpeed))
        {
            player.setDest(Object::Coordinates{player.getDest().x + playerSpeed, player.getDest().y, player.getDest().w, player.getDest().h});
        }
    }

    player.updateAnimation();
}

void Game::drawMap()
{
    for(const auto& el : backgroundMap)
    {
        drawObject(el);
    }
}

void Game::loadMap(const string& s)
{
    int currMapObject;
    int mapX;
    int mapY;
    int mapWidth;
    int mapHeight;

    std::ifstream inputFile(s.c_str());

    if(!inputFile)
    {
        cout << "Error during file read." << endl;
        return;
    }

    inputFile >> mapWidth;
    inputFile >> mapHeight;
    inputFile >> mapX;
    inputFile >> mapY;

    Object tmpObj;
    tmpObj.setImage("/usr/share/resources/mapTile.png", rend);


    for(int h = 0; h < mapHeight; ++h)
    {
        for(int w = 0; w < mapWidth; ++w)
        {
            inputFile >> currMapObject;
            if(currMapObject != 0)
            {
                int gridX = (currMapObject-1)%TILE_GRID_SIZE;
                int gridY = static_cast<int>((currMapObject-1)/TILE_GRID_SIZE);

                tmpObj.setSrc(Object::Coordinates{gridX*TILE_WIDTH, gridY*TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT});
                tmpObj.setDest(Object::Coordinates{(w * TILE_WIDTH*3) + mapX, (h * TILE_HEIGHT*3) + mapY, TILE_WIDTH*3, TILE_HEIGHT*3});
                backgroundMap.push_back(tmpObj);
            }

        }
    }

}

bool Game::mapCollision(const Object& obj1, int xPositionDelta)
{
    if(obj1.getDest().x + xPositionDelta < 0 ||
       obj1.getDest().x + obj1.getDest().w + xPositionDelta > WINDOW_WIDTH)
    {
        return true;
    }

    for(auto& mapObj : backgroundMap)
    {
        if((obj1.getDest().x + xPositionDelta < (mapObj.getDest().x + mapObj.getDest().w)) && ((obj1.getDest().x + xPositionDelta + obj1.getDest().w) > mapObj.getDest().x) 
        && (obj1.getDest().y < (mapObj.getDest().y + mapObj.getDest().h)) && ((obj1.getDest().y + obj1.getDest().h) > mapObj.getDest().y))
        {
            return true;
        }
    }
    return false;

}

void Game::drawMsg(const string& msg, int x, int y, int r, int g, int b)
{
    SDL_Surface* surf;
    SDL_Texture* tex;
    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;

    surf = TTF_RenderText_Solid(font, msg.c_str(), color);
    tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surf->w;
    rect.h = surf->h;

    SDL_FreeSurface(surf);
    SDL_RenderCopy(rend, tex, nullptr, &rect);
    SDL_DestroyTexture(tex);
}
