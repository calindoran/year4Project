// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidar_device.h

#pragma once
#include <memory>
#include <memory.h>
#include <algorithm>

#ifndef LIDAR_SDK_ABSENT
#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

using namespace rp::standalone::rplidar;
using RPlidarDriverPtr = std::shared_ptr<rp::standalone::rplidar::RPlidarDriver>;

inline auto allocLidarDriver()
{
    return RPlidarDriverPtr(RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT), [](RPlidarDriver* p)
        {
            if (p)
                RPlidarDriver::DisposeDriver(p);
        });
}

#endif
