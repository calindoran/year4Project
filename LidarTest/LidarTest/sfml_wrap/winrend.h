#pragma once
#include "SFML/System.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <mutex>
#include "mthread_config.h"
#include "scene.h"

/*
    This class implements basic game loop to be called from the programme
 */


class WinRend
{
private:
    WinRend() = delete;
    static RendererLockType renderMutex;
    static std::atomic<uint32_t> clearColor;
public:
    static void setClearColor(const sf::Color& color);

    //makeNewViewIfResized == false  - view will remain same, all content will be scaled to new window proportions
    //makeNewViewIfResized == true  - everything will retain size, view will be recreated to match new window size
    int  static Main(sf::RenderWindow& window, Scene& game, const bool makeNewViewIfResized = false, const float desiredFPS = 60.f);
};
