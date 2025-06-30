#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "./store.hpp"


namespace pez
{

struct ResourcesStore
{
    void registerFont(std::string const& filename, std::string const& name)
    {
        sf::Font* const font = m_fonts.create(name);
        if (!font->openFromFile(filename)) {
            std::cout << "Failed to open font file '" << filename << "'\n";
        }
    }

    void registerTexture(std::string const& filename, std::string const& name)
    {
        sf::Texture* const texture = m_textures.create(name);
        if (!texture->loadFromFile(filename)) {
            std::cout << "Failed to open texture file '" << filename << "'\n";
            return;
        }
        if (!texture->generateMipmap()) {
            std::cout << "Failed to generate mipmap\n";
            return;
        }
        texture->setSmooth(true);
    }

    [[nodiscard]]
    sf::Font const* getFont(std::string const& name) const
    {
        return m_fonts.get(name);
    }

    [[nodiscard]]
    sf::Texture const* getTexture(std::string const& name) const
    {
        return m_textures.get(name);
    }

private:
    Store<sf::Font> m_fonts;
    Store<sf::Texture> m_textures;
};

}