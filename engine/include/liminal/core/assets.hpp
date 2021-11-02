#ifndef LIMINAL_CORE_ASSETS_HPP
#define LIMINAL_CORE_ASSETS_HPP

#include <string>
#include <unordered_map>

namespace liminal
{
    template <typename T>
    class assets
    {
    public:
        assets();
        ~assets();

    private:
        std::unordered_map<std::string, T *> cache;
    };
}

#endif
