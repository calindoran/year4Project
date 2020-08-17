#pragma once

#if BOOST_OK
    #include <boost/pool/singleton_pool.hpp>
    #include <boost/pool/pool_alloc.hpp>
#endif

#include <memory>
#include <atomic>
#include <vector>
#include <deque>
#include <sstream>

#include "type_checks.h"

/*
 * Allocates shared pointers using boost's pools
 * */

namespace pools
{
#if BOOST_OK
    using PooledString       = std::basic_string<std::string::value_type, std::string::traits_type, boost::pool_allocator<std::string::value_type>>;

    template <class T>
    using PooledVector = std::vector<T, boost::pool_allocator<T>>;

    template <class T>
    using PooledDeque = std::deque<T, boost::fast_pool_allocator<T>>;
#else
    using PooledString       = std::string;

    template <class T>
    using PooledVector = std::vector<T>;

    template <class T>
    using PooledDeque = std::deque<T>;

#endif

    using PooledStringStream = std::basic_stringstream<PooledString::value_type, PooledString::traits_type, PooledString::allocator_type>;


    template <class T>
    auto pooledVector(size_t inisize = 0)
    {
        PooledVector<T> r;
        r.resize(inisize);
        return r;
    }

    template <class T, class V>
    auto pooledVector(size_t inisize, const V& v)
    {
        PooledVector<T> r;
        r.resize(inisize, v);
        return r;
    }

    //just to make friends easier
    template<class GenT, typename... Args>
    struct AllocShared
    {
#if BOOST_OK
        static std::shared_ptr<GenT> alloc(Args&&... args)
        {

            using pool_t = boost::singleton_pool<GenT, sizeof(GenT)>;
            void *mem = pool_t::malloc();
            return std::shared_ptr<GenT>(new (mem) GenT(std::forward<Args>(args)...), [](GenT * p)
            {
                if (p)
                {
                    p->~GenT();
                    pool_t::free(p);
                }
            });
        }
#else
        static std::shared_ptr<GenT> alloc(Args&&... args)
        {
            return std::shared_ptr<GenT>(new GenT(std::forward<Args>(args)...));
        }
#endif
    };


    template<class GenT, typename... Args>
    inline std::shared_ptr<GenT> allocShared(Args&&... args)
    {
        return AllocShared<GenT, Args...>::alloc(std::forward<Args>(args)...);
    }
}

#define DECLARE_FRIEND_POOL    template<class GenT, typename... Args> friend struct pools::AllocShared
