#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <atomic>
#include <iostream>

/*
 * Runners examples
 * */

namespace utility
{
    using runner_t    = std::shared_ptr<std::thread>;
    using runnerint_t = std::shared_ptr<std::atomic<bool>>;

    using runner_f_t  = std::function<void(const runnerint_t should_int)>;

    //simple way to execute lambda in thread, in case when shared_ptr is cleared it will send
    //stop notify and join(), so I can ensure 1 pointer has only 1 running thread always for the same task
    inline runner_t startNewRunner(runner_f_t func)
    {
        auto stop = runnerint_t(new std::atomic<bool>(false));
        return runner_t(new std::thread(func, stop), [stop](std::thread* p) //no auto need c++14 or higher
        {
            stop->store(true);
            if (p)
            {
                if (p->joinable())
                    p->join();
                delete p;
            }
        });
    }

    inline size_t currentThreadId()
    {
        return std::hash<std::thread::id> {}(std::this_thread::get_id());
    }
}

//tests atomic bool for expected, if it is - sets it to !expected and returns true
inline bool testandflip(std::atomic<bool>& var, const bool expected)
{
    bool exp{expected};
    return var.compare_exchange_strong(exp, !expected);
}

//atomic var = var || value
inline void or_equal(std::atomic<bool>& var, const bool value)
{
    /*
     * Compares the contents of the contained value with expected:
        - if true, it replaces the contained value with val (like store).
        - if false, it replaces expected with the contained value .
     * */
    bool expected = false;
    var.compare_exchange_strong(expected, value);
}

//atomic var = var && value
inline void and_equal(std::atomic<bool>& var, const bool value)
{
    /*
     * Compares the contents of the contained value with expected:
        - if true, it replaces the contained value with val (like store).
        - if false, it replaces expected with the contained value .
     * */
    bool expected = true;
    var.compare_exchange_strong(expected, value);
}
