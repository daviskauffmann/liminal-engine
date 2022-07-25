#ifndef LIMINAL_CORE_ASSETS_HPP
#define LIMINAL_CORE_ASSETS_HPP

#include <liminal/audio/sound.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/graphics/texture.hpp>
#include <unordered_map>

namespace liminal
{
    class assets
    {
    public:
        static assets *instance;

        assets();
        ~assets();

        template <typename Resource, typename... Args>
        Resource *load(const std::string &filename, Args &&...args)
        {
            if (cache.find(filename) == cache.end())
            {
                cache[filename] = new Resource(filename, std::forward<Args>(args)...);
            }

            return (Resource *)cache[filename];
        }

    private:
        // TODO: void * cannot be properly deleted
        // perhaps make this entire class a template and have multiple instances for each type of asset?
        std::unordered_map<std::string, void *> cache;
    };
}

#endif
