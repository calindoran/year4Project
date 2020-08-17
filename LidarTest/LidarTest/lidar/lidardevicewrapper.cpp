// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidardevicewrapper.cpp

#include "lidardevicewrapper.h"
#include <iostream>
#include <exception>
#include <sstream>
#include <iomanip>
#include <algorithm>

//TODO: change this to any object (stream) which has overloaded << operator
#define ERROR_OUT std::cerr

LidarDeviceWrapper::LidarDeviceWrapper(const LidarParams& params) :
    orig_params(params),
    devinfo{}
{
    init(params.device, params.bauds);
}

LidarDeviceWrapper::~LidarDeviceWrapper()
{
    cleanup();
}

std::string LidarDeviceWrapper::toString() const
{
    std::stringstream cout;
    //Print out the device serial number, firmware and hardware version number
    cout << "RPLIDAR S/N: ";
    for (const auto v : devinfo.serialnum)
        cout << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(v);
    cout << std::endl;
    cout << "Firmware Ver: " << devinfo.firmware_version << std::endl;
    cout << "Hardware Rev: " << static_cast<int>(devinfo.hardware_version);

    return cout.str();
}

bool LidarDeviceWrapper::checkRPLIDARHealth() const
{
    bool res = false;
    if (drv)
    {
        rplidar_response_device_health_t healthinfo;
        const auto op_result = drv->getHealth(healthinfo);
        if (IS_OK(op_result))
            res = (healthinfo.status != (RPLIDAR_STATUS_ERROR));
        else
            ERROR_OUT << "Error, cannot retrieve the lidar health code: " << op_result << std::endl;
    }
    else
        ERROR_OUT << "Trying to check health for not connected device." << std::endl;
    return res;
}

bool LidarDeviceWrapper::init(std::string dev_path, const uint32_t baud_rate)
{
    cleanup();

    if (dev_path.empty())
    {
#ifdef _WIN32
        // use default com port
        // opt_com_path = "\\\\.\\com57";
        dev_path = "COM3";
#elif __APPLE__
        dev_path = "/dev/tty.SLAB_USBtoUART";
#else
        dev_path = "/dev/ttyUSB0";
#endif
    }
    std::vector<uint32_t> bauds{ 115200, 256000 };
    if (baud_rate)
    {
        bauds.clear();
        bauds.push_back(baud_rate);
    }

    for (const auto br : bauds)
    {
        //FIXME: not sure why allocation is for each baud
        //took from example, move prior the loop maybe
        drv = allocLidarDriver();
        if (!drv)
            throw std::runtime_error("Failed to allocate driver!");
        if (IS_OK(drv->connect(dev_path.c_str(), br)) && IS_OK(drv->getDeviceInfo(devinfo)))
            break;
    }

    const bool ok = drv && drv->isConnected() && checkRPLIDARHealth();
    if (!ok)
        ERROR_OUT << "Error, cannot bind to the specified serial port: " << dev_path << std::endl;
    else
        std::cout << toString() << std::endl;
    return ok;
}

void LidarDeviceWrapper::cleanup()
{
    stopScan();
    drv.reset();
    memset(&devinfo, 0, sizeof(devinfo));
}

void LidarDeviceWrapper::stopScan() const
{
    if (drv)
    {
        //stop...
        drv->stop();
        //stop motor...
        drv->stopMotor();
    }
}

void LidarDeviceWrapper::runScan(uint32_t options) const
{
    if (drv)
    {
        //start motor...
        drv->startMotor();
        //start scan...
        drv->startScan(false, true, options);
    }
}

LidarValuesVector LidarDeviceWrapper::readOnce(size_t count) const
{
    LidarValuesVector res;
    if (drv)
    {
        pools::PooledVector<rplidar_response_measurement_node_hq_t> nodes;
        nodes.resize(count);
        if (IS_OK(drv->grabScanDataHq(nodes.data(), count)))
        {
            nodes.resize(count); //make vector of same size as returned
            drv->ascendScanData(nodes.data(), count);
            res.reserve(count); //making memory allocation in front, so loop is fast
            std::transform(std::begin(nodes), std::end(nodes), std::back_inserter(res), [](const auto& v)
                {
                    return LidarValues::fromDriverData(v);
                });
        }
    }
    return res;
}

bool LidarDeviceWrapper::testHealthAndReinitIfNeed()
{
    return checkRPLIDARHealth() || init(orig_params.device, orig_params.bauds);;
}
