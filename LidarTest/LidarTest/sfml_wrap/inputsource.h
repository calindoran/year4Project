#ifndef INPUTSOURCE_H
#define INPUTSOURCE_H

#include <utility>
#include <map>
#include "SFML/Window/Event.hpp"
#include "..\helpers\safequeue.h"
#include "2dnavigator.h"

using EventsQueue = SafeQueue<sf::Event>;

class InputSource
{
public:
	InputSource();
	void consumeEvents(EventsQueue& events);
	bool isKeyPressed(sf::Keyboard::Key key) const;
	INavTest2DPtr getNavigator() const;
protected:
	virtual void processEvents(const EventsQueue::queue_t& src);

	void updateKbPressed(const EventsQueue::queue_t& src);
private:
	std::map<sf::Keyboard::Key, bool> kbPressedMap{};
	INavTest2DPtr currentConfiguredNavigator{ nullptr };
};

#endif // INPUTSOURCE_H
