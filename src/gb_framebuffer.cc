#include "gb_framebuffer.h"

#define COLOUR_MAP_INIT \
{{\
    sf::Color(255, 255, 255),\
    sf::Color(192, 192, 192),\
    sf::Color(96, 96, 96),\
    sf::Color(0, 0, 0)\
}}

gb_framebuffer::gb_framebuffer()
    : sf::Image(), m_colour_map(COLOUR_MAP_INIT)
{
}

void gb_framebuffer::set_pixel(unsigned int x, unsigned int y, gb_colour_t colour) {
    sf::Image::setPixel(x, y, m_colour_map.at(colour));
}
