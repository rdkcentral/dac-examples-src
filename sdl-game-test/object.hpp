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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <SDL.h> 
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 

class Object
{
public:
    struct Coordinates
    {
        int x;
        int y;
        int w;
        int h;
    };

    Object() = default;
    virtual ~Object() = default;

    void setSrc(const Coordinates& c);
    void setDest(const Coordinates& c);
    SDL_Rect getSrc() const;
    SDL_Rect getDest() const;
    SDL_Texture* getTex() const;
    void setImage(const std::string& imageName, SDL_Renderer* rend);

private:
    SDL_Rect src;
    SDL_Rect dest;
    SDL_Texture* tex;

};


#endif // OBJECT_HPP
