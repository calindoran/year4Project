#pragma once

#include <memory>

class INavTest2D
{
public:
    virtual bool isLeft()  const = 0;
    virtual bool isRight() const = 0;
    virtual bool isTop() const = 0;
    virtual bool isBottom() const = 0;
    virtual bool isModify1() const = 0;
};

using INavTest2DPtr = std::shared_ptr<INavTest2D>;
