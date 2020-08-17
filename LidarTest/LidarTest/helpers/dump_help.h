#pragma once

#include <iostream>
#include <string>
#include "strutils.h"

/*
 * Simple dump helper class
 * */

namespace dump_helper
{
    template <class Cont>
    void dumpContainer(const Cont& src)
    {
        for (const auto& v : src)
            std::cout << utility::toStdStr(v) << std::endl;
    }
}
