#pragma once
#include <unordered_map>

template<typename T, typename H>
class Registry
{
public:
    inline void add(T key, H value)
    {
        reg.insert(std::pair<T, H>(key, value));
    }

    H* get(T key)
    {
        auto found = reg.find(key);

        if (found == reg.end())
            return nullptr;

        return &found->second;
    }

    bool has(T key)
    {
        auto found = reg.find(key);

        return found != reg.end();
    }
private:
    std::unordered_map<T, H> reg;
};
