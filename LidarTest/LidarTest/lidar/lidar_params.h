// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidar_params.h

#pragma once

#include <stdint.h>
#include <string>

struct LidarParams
{
	std::string device;
	uint32_t bauds{ 0 };

	//this is delay between 2 lidar readings as it goes in parallel
	uint64_t update_delay_ms{ 1000 / 60 };
};
