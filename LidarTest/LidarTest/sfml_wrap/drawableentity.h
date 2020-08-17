#ifndef DRAWABLEENTITY_H
#define DRAWABLEENTITY_H

#include <memory>
#include <vector>
#include <cstdint>
#include "inputsource.h"
#include "SFML/Graphics/Drawable.hpp"

class Scene;
class DrawableEntity : public sf::Drawable
{
protected:
	DrawableEntity() = default;
public:
	~DrawableEntity() override = default;
	virtual void update(Scene& scene, float time, const InputSource& input) = 0;
	virtual void setScreenSize(float width, float height);
	virtual void setScale(float mx, float my);
};

using DrawablePtr = std::shared_ptr<sf::Drawable>;
using DrawableEntityPtr = std::shared_ptr<DrawableEntity>;
using DrawableEntityList = std::vector<DrawableEntityPtr>;

#endif // DRAWABLEENTITY_H
