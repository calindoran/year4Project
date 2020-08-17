// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: main.cpp

//sfml libs
#include <sfml/graphics.hpp>
#include <sfml/window.hpp>
#include <sfml/system.hpp>

#include <iostream>
#include <string>

#include "..\lidar_draws\window_constants.h"
#include "..\lidar_draws\lidarscene.h"
#include "..\sfml_wrap\winrend.h"
#include "..\lidar\lidardevicewrapper.h"
#include "..\lidar\lidar_params.h"

using namespace std;

int main(int argc, const char* argv[])
{
    std::cout << "A.R.M LIDAR System" << std::endl;
    std::cout << "Version: " << RPLIDAR_SDK_VERSION << std::endl;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(DESIGNED_WIDTH, DESIGNED_HEIGHT), "A.R.M Lidar System", sf::Style::Default, settings);

    //TODO: add something like boost command line parser, so can use like
    //-d com3 -b 300

    LidarParams params;
    //this is delay between lidar readings, so 1 reading will be at least this value or slower
    //it is not the same as draw frame rate, those 2 things are independant +/-
    params.update_delay_ms = 100;


    if (argc > 1)
        params.device = std::string(argv[1]); // or set to a fixed value: e.g. "com3"

    if (argc > 2)
    {
        try
        {
            params.bauds = std::max(0, std::stoi(argv[2])); // no negatives
        }
        catch (std::invalid_argument const& e)
        {
            std::cerr << "Bad input: std::invalid_argument thrown" << '\n';
        }
        catch (std::out_of_range const& e)
        {
            std::cerr << "Integer overflow: std::out_of_range thrown" << '\n';
        }
    }

    LidarScene scene(params);
    return WinRend::Main(window, scene, false, 60.f);
}
