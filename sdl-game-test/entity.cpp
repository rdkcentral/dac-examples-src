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

#include "entity.hpp"

int Entity::createAnimation(int row, int w, int h, int amount, int speed)
{
    Cycle tmp;
    tmp.row = row - 1;
    tmp.w = w;
    tmp.h = h;
    tmp.amount = amount;
    tmp.speed = speed;
    tmp.tick = 0;

    animations.push_back(tmp);
    return animations.size() - 1;
}

void Entity::setCurAnimation(int ca)
{
    currentAnimationIterator = 0;
    currAnim = ca;
}


void Entity::updateAnimation()
{
    setSrc(Object::Coordinates{animations[currAnim].w * animations[currAnim].tick,
                               animations[currAnim].h * animations[currAnim].row,
                               animations[currAnim].w,
                               animations[currAnim].h});

    if(currentAnimationIterator > animations[currAnim].speed)
    {
        animations[currAnim].tick++;
        currentAnimationIterator = 0;
    }

    currentAnimationIterator++;

    if(animations[currAnim].tick >= animations[currAnim].amount)
    {
        animations[currAnim].tick = 0;
    }
}

int Entity::getCurAnimation() const
{
    return currAnim;
}

