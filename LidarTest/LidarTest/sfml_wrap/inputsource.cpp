#include "inputsource.h"
#include "..\helpers\pooled_shared.h"

class KeyboardNavigator : public INavTest2D
{
    const InputSource *is{nullptr};
public:
    KeyboardNavigator(const InputSource *is): is(is) {}
    bool isLeft()  const override
    {
        return is->isKeyPressed(sf::Keyboard::Key::A);
    }
    bool isRight() const override
    {
        return is->isKeyPressed(sf::Keyboard::Key::D);
    }
    bool isTop() const override
    {
        return is->isKeyPressed(sf::Keyboard::Key::W);
    }
    bool isBottom() const override
    {
        return is->isKeyPressed(sf::Keyboard::Key::S);
    }
    bool isModify1() const override
    {
        return is->isKeyPressed(sf::Keyboard::Key::LShift);
    }
};


InputSource::InputSource()
{
    //fixme: will be keyboard only now
    currentConfiguredNavigator = pools::allocShared<KeyboardNavigator>(this);
}

void InputSource::consumeEvents(EventsQueue &events)
{
    EventsQueue::queue_t tmp;
    events.swap(tmp);
    processEvents(tmp);

}

bool InputSource::isKeyPressed(sf::Keyboard::Key key) const
{
    if (kbPressedMap.count(key))
        return kbPressedMap.at(key);
    return false;
}

INavTest2DPtr InputSource::getNavigator() const
{
    return currentConfiguredNavigator;
}

void InputSource::processEvents(const EventsQueue::queue_t &src)
{
    updateKbPressed(src);
}

void InputSource::updateKbPressed(const EventsQueue::queue_t &src)
{
    for (const auto& e : src)
    {
        if (e.type == sf::Event::KeyPressed)
            kbPressedMap[e.key.code] = true;

        if (e.type == sf::Event::KeyReleased)
            kbPressedMap[e.key.code] = false;
    }
}
