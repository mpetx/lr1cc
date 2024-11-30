#ifndef LR1CC_INCLUDE_UTIL_HH
#define LR1CC_INCLUDE_UTIL_HH

#include <functional>
#include <set>
#include <string>
#include <string_view>
#include <utility>

namespace lr1cc
{

    template <typename A,
              typename B,
              typename HashA = std::hash<A>,
              typename HashB = std::hash<B>>
    struct PairHash
    {

        std::size_t operator()(const std::pair<A, B> &pair) const
        {
            auto ha = HashA{}(pair.first);
            auto hb = HashB{}(pair.second);
            return (ha << 1) + ha + hb;
        }
        
    };

    template <typename A,
              typename HashA = std::hash<A>>
    struct SetHash
    {

        std::size_t operator()(const std::set<A> &set) const
        {
            std::size_t h = 0;

            for (const A &a : set)
            {
                h += HashA{}(a);
            }

            return h;
        }
        
    };

    struct HeterogeneousStringHash
    {
        using is_transparent = void;

        std::size_t operator()(std::string_view sv) const
        {
            return std::hash<std::string_view>{}(sv);
        }

        std::size_t operator()(const std::string &s) const
        {
            return std::hash<std::string_view>{}(s);
        }
        
    };
    
}

#endif
