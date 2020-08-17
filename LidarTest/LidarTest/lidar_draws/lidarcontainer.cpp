// Name: Calin Doran
// Student Number: C00220175
// Project: A.R.M Lidar System
// File: lidarcontainer.cpp

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "lidarcontainer.h"
#include "..\lidar\lidardevicewrapper.h"
#include "..\helpers\block_delay.h"
#include "..\helpers\guard_on.h"
#include "..\helpers\containers_helpers.h"

constexpr static float minDist = 200;
constexpr static float maxDist = 600;
constexpr static float halfFOV = 30.f; //half of field-of-view in degrees
constexpr static uint32_t initial_nodes_amount = 1024;//that was 8192 reduced for better managment
constexpr static size_t do_health_check_each_N_reads = 100;

LidarContainer::LidarContainer(const LidarParams& params) :
    scanDensity(initial_nodes_amount)
{
    using DelayMeasuredIn = std::chrono::milliseconds;

    //this is lidar reader thread
    lidarThread = utility::startNewRunner([this, params](const auto need2stop)
        {
            const DelayMeasuredIn DELAY = std::chrono::duration_cast<DelayMeasuredIn>(std::chrono::milliseconds(params.update_delay_ms));
            try
            {
                //creating device inside thread as many OS dislike cross-thread handles
                LidarDeviceWrapper lidar(params);
                lidar.runScan();
                size_t counter = 0;
                while (!(*need2stop)) //checking if program terminates (need2stop is shared pointer to boolean value)
                {
                    //this will ensure 1 iteration takes at least DELAY
                    DelayBlockMs<DelayMeasuredIn> delay(DELAY);
                    (void)delay;

                    //each couple steps lets check health and restart / reconnect if needed
                    if (((++counter) % do_health_check_each_N_reads) == 0)
                    {
                        lidar.testHealthAndReinitIfNeed();
                        lidar.runScan();
                    }

                    auto readings = lidar.readOnce(scanDensity.load());
                    if (readings.size())
                    {
                        const auto static cannot_see = [](const LidarValues& v)->bool
                        {
                            //took "can see" from example and inverted
                            return !(v.rDistance > minDist && v.rDistance < maxDist && (v.rAngle < halfFOV || v.rAngle > 360 - halfFOV));
                        };

                        //removing all readings we cannot use
                        types_ns::remove_if(readings, cannot_see);

                        //making left side angles negatives, so it is proper sorted
                        std::transform(std::begin(readings), std::end(readings), std::begin(readings), [](LidarValues& a)
                            {
                                static_assert(halfFOV < 45.f, "FOV must be less then 90 degree.");
                                if (a.rAngle >= 270.f)
                                    a.rAngle -= 360.f;
                                return a;
                            });

                        //sort by angle as LidarValues has comparator by angle
                        std::sort(std::begin(readings), std::end(readings));

                        //then sort by distance in reverse order
                        std::sort(std::begin(readings), std::end(readings), [](const LidarValues& a, const LidarValues& b)
                            {
                                return a.rDistance > b.rDistance;
                            });

                        //pushing values to "global", so drawer thread may use it
                        LOCK_GUARD_ON(valuesLock);
                        lastToDraw.swap(readings);
                    }
                    else
                    {
                        counter = do_health_check_each_N_reads - 1; //no readings, try to restart ASAP

                        //FIXME: if lidar fails to read drop whats shown, however can comment out 2 lines below
                        //then drawer will show last update permanent
                        LOCK_GUARD_ON(valuesLock);
                        lastToDraw.clear();
                    }

                    //here pause may happen issued by delay destructor
                }
            }
            catch (...)
            {
                std::cerr << "Exception in lidar reader thread." << std::endl;
                std::exit(255);
            }
        });
}

LidarContainer::~LidarContainer()
{
    lidarThread.reset(); //terminating thread
}

void LidarContainer::update(Scene& scene, float time, const InputSource& input)
{
    (void)scene;
    (void)time;
    (void)input;
    //TODO: here can do something which changes draw logic, for example zoom by keyboard

    //example if initial_nodes_amount is 1024
    const bool plus = input.getNavigator()->isTop();
    const bool minus = input.getNavigator()->isBottom();
    if (plus != minus)
    {
        auto val = scanDensity.load();
        val += (plus) ? 3 : -5;
        scanDensity = std::max(10u, std::min(8192u * 2, val));
    }
}

void LidarContainer::draw(sf::RenderTarget& where, sf::RenderStates states) const
{
    //this function generates visuals accodring to data and draws it. Visual objects are not stored.
    //expect lidar thread pushed only visible items to here, so it wont spend time on calculations


    //doing local copy of latest lidar readings. so lidar reader can keep update for
    //also need a copy (not move) because may draw faster then lidar makes new data, so we need to draw something
    //a must of {}, keeping thread lock short
    LidarValuesVector curr;
    {

        LOCK_GUARD_ON(valuesLock);
        curr = lastToDraw;
    }

    const sf::Vector2f drawSz(size.x * scale.x, size.y * scale.y);

    const float x_per_degree = drawSz.x / (2 * halfFOV);
    const float y_mid = drawSz.y / 2.f;
    const float distance_0_size = std::fmin(drawSz.x, drawSz.y) * 0.9f;

    //drawing, making visual rectangle for each piece of data


    const static sf::Color colors[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue };
    size_t clr_index = 0;
    for (const auto& v : curr)
    {
        sf::RectangleShape rectangle;
        rectangle.setSize({ 1.0f, 1.0f });
        rectangle.setOrigin(0.5f, 0.5f);

        rectangle.setFillColor(colors[clr_index++]);
        clr_index = clr_index % types_ns::countof(colors);

        const float sz = distance_0_size / ((v.rDistance - minDist) - 1.f);
        rectangle.scale(sz, sz);
        rectangle.setPosition(x_per_degree * (v.rAngle + halfFOV), y_mid);

        where.draw(rectangle, states);
    }
}

void LidarContainer::setScreenSize(float width, float height)
{
    size = sf::Vector2f(width, height);
}

void LidarContainer::setScale(float mx, float my)
{
    scale = sf::Vector2f(mx, my);
}
