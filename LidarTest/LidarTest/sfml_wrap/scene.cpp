#include "scene.h"
#include "SFML/Graphics/Texture.hpp"
#include <iostream>
#include "..\helpers\pooled_shared.h"

Scene::Scene()
{
    clock.restart();
}

void Scene::processEvents(EventsQueue &e)
{
    events.consumeEvents(e);
}

float Scene::getElapsed()
{
    auto r = clock.restart().asMicroseconds() * 0.001f;
    return r;
}

void Scene::viewWasChanged(const sf::View &view) const
{
}

const DrawableEntityList &Scene::getEntities() const
{
    return entities;
}

void Scene::win() const
{

}

void Scene::processUpdatedEntitesBeforeDraw(const DrawableEntityList&)
{
}

bool Scene::updateViewBeforeRender(sf::View&)
{
    return false;
}

void Scene::render(sf::RenderTarget &where, bool paused)
{
    const auto set_new_view = [this, &where](const auto & v)
    {
        where.setView(v);
        viewWasChanged(v);
    };

    if (needUpdateView)
    {
        needUpdateView = false;
        //doing "copy", I think initialy is empty here
        const sf::View v(where.getDefaultView());
        set_new_view(v);
    }

    const float time = getElapsed();
    if (!paused)
    {
        for (auto it = entities.begin(); it != entities.end(); ++it)
        {
            const auto& e = *it;
            e->update(*this, time, events);
        }

        processUpdatedEntitesBeforeDraw(entities);

        //fixme: possible copy of view maybe slow ...
        auto v = where.getView();
        if (updateViewBeforeRender(v))
            set_new_view(v);
    }

    const auto& render_state = sf::RenderStates::Default;

    for (const auto & e : entities)
        where.draw(*e, render_state);
}
