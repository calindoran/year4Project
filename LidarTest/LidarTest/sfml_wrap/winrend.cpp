#include "SFML/Window/Event.hpp"

#include <thread>
#include <chrono>
#include "winrend.h"
#include <iostream>
#include "..\helpers\block_delay.h"
#include "..\helpers\guard_on.h"

using DelayMeasuredIn = std::chrono::milliseconds;


//static defs
RendererLockType WinRend::renderMutex;
std::atomic<uint32_t> WinRend::clearColor(sf::Color::White.toInteger());

void WinRend::setClearColor(const sf::Color &color)
{
    clearColor = color.toInteger();
}

int WinRend::Main(sf::RenderWindow& window, Scene& game, const bool makeNewViewIfResized, const float desiredFPS)
{
    using namespace std::chrono_literals;
    window.setActive(false); //detaching window from thread
    EventsQueue events;
    std::atomic<bool> lostFocus(false);

    const DelayMeasuredIn DELAY = std::chrono::duration_cast<DelayMeasuredIn>(std::chrono::milliseconds(static_cast<int32_t>(1000.f / desiredFPS)));

    std::thread renderThread([&window, &events, &game, &lostFocus, &DELAY]
    {
        window.setActive(true); //attaching window to this thread
        while (window.isOpen())
        {
            DelayBlockMs<DelayMeasuredIn> delay(DELAY);//defines FPS, however it is MS delay ...
            (void)delay;

            const bool p = lostFocus;
            if (!p)
                game.processEvents(events);

            {
                LOCK_GUARD_ON(renderMutex);
                window.clear(sf::Color(clearColor));
                game.render(window, p);
                window.display();
            }
        }
    });

    // run the program as long as the window is open
    while (window.isOpen())
    {
        DelayBlockMs<DelayMeasuredIn> delay(DELAY);//defines FPS, however it is MS delay ...
        (void)delay;

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // catch the resize events
            if (event.type == sf::Event::Resized)
            {
                if (makeNewViewIfResized)
                {
                    LOCK_GUARD_ON(renderMutex);
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                    const sf::View v(visibleArea);
                    window.setView(v);
                    game.viewWasChanged(v);
                }
            }
            else
                if (event.type == sf::Event::Closed)// "close requested" event: we close the window
                {
                    LOCK_GUARD_ON(renderMutex);
                    window.close();
                }
                else
                    if (event.type == sf::Event::LostFocus)
                    {
                        lostFocus = true;
                        events.clear();
                    }
                    else
                        if (event.type == sf::Event::GainedFocus)
                            lostFocus = false;
                        else
                            events.push(event);
        }
    }

    renderThread.join();
    return 0;
}
