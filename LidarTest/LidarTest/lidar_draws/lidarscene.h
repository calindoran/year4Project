// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidarscene.h

#pragma once

#include "..\sfml_wrap\scene.h"
#include "..\lidar\lidar_params.h"

class LidarScene : public Scene
{
public:
    LidarScene(const LidarParams& params);
    ~LidarScene() override = default;

    void viewWasChanged(const sf::View& view) const override;
};
