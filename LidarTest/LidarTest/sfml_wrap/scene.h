#pragma once

#include "SFML/Window/Event.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "drawableentity.h"
#include <map>
#include <string>
#include "inputsource.h"
#include "..\helpers\cm_ctors.h"

class Scene
{
public:
	NO_COPYMOVE(Scene);
	Scene();
	virtual ~Scene() = default;

public:
	//API for entities
	const DrawableEntityList& getEntities() const;

	//signals winning condition
	virtual void win() const;
protected:
	friend class WinRend;
	DrawableEntityList entities;
	void processEvents(EventsQueue& e);
	void render(sf::RenderTarget& where, bool paused);
	inline float getElapsed();
	//"signal", called once new view set
	virtual void viewWasChanged(const sf::View& view) const;

	virtual void processUpdatedEntitesBeforeDraw(const DrawableEntityList& entities);

	//view parameter has current view on call, if function returns true - view will be updated
	//and will call viewWasChanged
	virtual bool updateViewBeforeRender(sf::View& view);

private:
	bool needUpdateView{ true };
	sf::Clock clock{};
	InputSource events;
};
