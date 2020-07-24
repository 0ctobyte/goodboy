/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_FRAMEBUFFER_H_
#define GB_FRAMEBUFFER_H_

#include <array>

#include <SFML/Graphics.hpp>

// Gameboy has four colours
// 00 - White
// 01 - Light Gray
// 10 - Dark Gray
// 11 - Black
enum gb_colour_t {
    GB_COLOUR0 = 0,
    GB_COLOUR1 = 1,
    GB_COLOUR2 = 2,
    GB_COLOUR3 = 3
};

class gb_framebuffer : public sf::Image {
public:
    gb_framebuffer();

    // Set a pixel with the specified colour. The colour is translated to something
    // that can be used on modern GPUs
    void set_pixel(unsigned int x, unsigned int y, gb_colour_t colour);

    // Get a pixel colour at the specified framebuffer location
    gb_colour_t get_pixel(unsigned int x, unsigned int y);

private:
    using gb_colour_map_t = std::array<sf::Color, 4>;

    gb_colour_map_t m_colour_map;
};

#endif // GB_FRAMEBUFFER_H_
