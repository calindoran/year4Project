// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidarcontainer.h

#pragma once

#include <mutex>
#include <atomic>
#include "..\helpers\spinlock.h"
#include "..\sfml_wrap\drawableentity.h"
#include "..\helpers\runners.h"
#include "..\lidar\lidar_params.h"
#include "..\lidar\lidar_data.h"
#include <SFML/System/Vector2.hpp>

//this class completely deals with lidar - draws anything inside own borders
//creates driver and access it

class LidarContainer : public DrawableEntity
{
private:
    utility::runner_t lidarThread{nullptr};

    mutable spinlock valuesLock;
    LidarValuesVector lastToDraw;
    sf::Vector2f size{0.f, 0.f};
    sf::Vector2f scale{1.f, 1.f};

    //this is not requested example of communication with program
    std::atomic<uint32_t> scanDensity;
public:
    LidarContainer(const LidarParams &params);
    ~LidarContainer() override;

    void update(Scene& scene, float time, const InputSource& input) override;
    void draw(sf::RenderTarget &where, sf::RenderStates states) const override;

    virtual void setScreenSize(float width, float height);
    virtual void setScale(float mx, float my);
};
