/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_INPUT_H_
#define GB_INPUT_H_

#include <array>

#include <SFML/Window/Keyboard.hpp>

enum gb_button_t {
    GB_BUTTON_DOWN   = 0,
    GB_BUTTON_UP     = 1,
    GB_BUTTON_LEFT   = 2,
    GB_BUTTON_RIGHT  = 3,
    GB_BUTTON_START  = 4,
    GB_BUTTON_SELECT = 5,
    GB_BUTTON_B      = 6,
    GB_BUTTON_A      = 7
};

class gb_input {
public:
    gb_input();

    void set_pressed(sf::Keyboard::Key code);
    void set_released(sf::Keyboard::Key code);
    bool is_pressed(gb_button_t button);

private:
    using gb_button_state_t = std::array<bool, 8>;

    gb_button_state_t m_button_state;
};

#endif // GB_INPUT_H_
