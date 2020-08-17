// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidar_data.h

#pragma once

#include <stdint.h>
#include <sstream>

#include "..\helpers\cm_ctors.h"
#include "..\helpers\type_checks.h"
#include "..\helpers\pooled_shared.h"

class LidarValues
{
public:
    //TODO: update here to match SDK / sense
    using flag_t  = uint32_t;
    using qual_t  = uint32_t;
    using float_t = float;


    flag_t  rFlag{0};
    float_t rAngle{0.f};
    float_t rDistance{0.f};
    qual_t  rQuality{0};

public:
    DEFAULT_COPYMOVE(LidarValues);

    LidarValues()  = default;
    ~LidarValues() = default;

    LidarValues(flag_t rFlag, float_t rAngle, float_t rDistance, qual_t rQuality):
        rFlag(rFlag), rAngle(rAngle), rDistance(rDistance), rQuality(rQuality)
    {
    }

    bool operator < (const LidarValues& c) const
    {
        return rAngle < c.rAngle;
    }

    //doing template so compiler will accept anything with proper fields present
    template <class T>
    static inline LidarValues fromDriverData(const T& data) noexcept
    {
        constexpr static auto div = static_cast<float_t>(static_cast<uint32_t>(1) << 14);
        LidarValues r;
        CASTSET2FIELD(r.rFlag, data.flag);
        CASTSET2FIELD(r.rAngle, data.angle_z_q14 * 90.f / div);
        CASTSET2FIELD(r.rDistance, data.dist_mm_q2 / 4.0f);
        CASTSET2FIELD(r.rQuality, data.quality);
        return r;
    }

    std::string toString() const
    {
        std::stringstream cout;
        cout << "Flag: " << rFlag << " | Angle: " << rAngle << " | Distance: " << rDistance << " | Quality: " << rQuality;
        return cout.str();
    }
};

//checking moving is allowed, so compiler will do fast code. on some changes it may get prohibited
TEST_MOVE_NOEX(LidarValues);


using LidarValuesQueue  = pools::PooledDeque<LidarValues>;
using LidarValuesVector = pools::PooledVector<LidarValues>;
