#include "gb_framebuffer.h"

#define GB_FRAMEBUFFER_WHITE      sf::Color(255, 255, 255)
#define GB_FRAMEBUFFER_LIGHT_GREY sf::Color(192, 192, 192)
#define GB_FRAMEBUFFER_DARK_GREY  sf::Color(96, 96, 96)
#define GB_FRAMEBUFFER_BLACK      sf::Color(0, 0, 0)

#define COLOUR_MAP_INIT \
{{\
    GB_FRAMEBUFFER_WHITE,\
    GB_FRAMEBUFFER_LIGHT_GREY,\
    GB_FRAMEBUFFER_DARK_GREY,\
    GB_FRAMEBUFFER_BLACK\
}}

gb_framebuffer::gb_framebuffer()
    : sf::Image(), m_colour_map(COLOUR_MAP_INIT)
{
}

gb_colour_t gb_framebuffer::get_pixel(unsigned int x, unsigned int y) {
    sf::Color pixel_colour = sf::Image::getPixel(x, y);

    // Reverse map the sf::Colour to a gb_colour_t
    gb_colour_t gb_colour = GB_COLOUR0;
    for (unsigned int i = 0; i < m_colour_map.size(); i++) {
       if (pixel_colour == m_colour_map.at(i)) gb_colour = static_cast<gb_colour_t>(i);
    }

    return gb_colour;
}

void gb_framebuffer::set_pixel(unsigned int x, unsigned int y, gb_colour_t colour) {
    sf::Image::setPixel(x, y, m_colour_map.at(colour));
}
