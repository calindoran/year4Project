// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidardevicewrapper.h

#pragma once

#include <string.h>
#include "..\helpers\cm_ctors.h"
#include "lidar_device.h"
#include "lidar_data.h"
#include "lidar_params.h"

//this class wraps LidarDriver or fake to our task

class LidarDeviceWrapper
{
private:
    RPlidarDriverPtr drv{ nullptr };
    rplidar_response_device_info_t devinfo;

    LidarParams orig_params;
    bool init(std::string dev_path, const uint32_t baud_rate);

public:
    NO_COPYMOVE(LidarDeviceWrapper);
    LidarDeviceWrapper() = delete;
    LidarDeviceWrapper(const LidarParams& params);
    ~LidarDeviceWrapper();

    bool checkRPLIDARHealth() const;
    bool testHealthAndReinitIfNeed();
    void cleanup();

    void runScan(uint32_t options = 0) const;
    void stopScan() const;

    //grabs scan data of count samples, i.e. 1 full circle divided into count pieces
    LidarValuesVector readOnce(size_t count = 8192) const;

    std::string toString() const;
};
