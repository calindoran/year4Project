// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidarscene.cpp

#include "lidarscene.h"
#include "lidarcontainer.h"
#include "..\helpers\pooled_shared.h"

LidarScene::LidarScene(const LidarParams &params)
{
    auto pc = pools::allocShared<LidarContainer>(params);
    entities.push_back(pc);
}

void LidarScene::viewWasChanged(const sf::View &view) const
{
    const sf::Vector2f curSize(view.getSize()); //copy, to make sure it remains while we're in function
    for (auto & e : entities)
        e->setScreenSize(curSize.x, curSize.y);
}
