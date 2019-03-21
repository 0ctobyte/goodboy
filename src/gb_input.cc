#include "gb_input.h"

gb_input::gb_input()
    : m_button_state()
{
    m_button_state.fill(false);
}

void gb_input::set_pressed(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Down:   m_button_state.at(GB_BUTTON_DOWN)   = true; break;
        case sf::Keyboard::Up:     m_button_state.at(GB_BUTTON_UP)     = true; break;
        case sf::Keyboard::Left:   m_button_state.at(GB_BUTTON_LEFT)   = true; break;
        case sf::Keyboard::Right:  m_button_state.at(GB_BUTTON_RIGHT)  = true; break;
        case sf::Keyboard::Enter:  m_button_state.at(GB_BUTTON_START)  = true; break;
        case sf::Keyboard::RShift: m_button_state.at(GB_BUTTON_SELECT) = true; break;
        case sf::Keyboard::Z:      m_button_state.at(GB_BUTTON_B)      = true; break;
        case sf::Keyboard::X:      m_button_state.at(GB_BUTTON_A)      = true; break;
        default: break;
    }
}

void gb_input::set_released(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Down:   m_button_state.at(GB_BUTTON_DOWN)   = false; break;
        case sf::Keyboard::Up:     m_button_state.at(GB_BUTTON_UP)     = false; break;
        case sf::Keyboard::Left:   m_button_state.at(GB_BUTTON_LEFT)   = false; break;
        case sf::Keyboard::Right:  m_button_state.at(GB_BUTTON_RIGHT)  = false; break;
        case sf::Keyboard::Enter:  m_button_state.at(GB_BUTTON_START)  = false; break;
        case sf::Keyboard::RShift: m_button_state.at(GB_BUTTON_SELECT) = false; break;
        case sf::Keyboard::Z:      m_button_state.at(GB_BUTTON_B)      = false; break;
        case sf::Keyboard::X:      m_button_state.at(GB_BUTTON_A)      = false; break;
        default: break;
    }
}

bool gb_input::is_pressed(gb_button_t button) {
    return m_button_state.at(button);
}
