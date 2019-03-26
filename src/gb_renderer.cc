#include <algorithm>

#include "gb_renderer.h"

gb_renderer::gb_renderer(unsigned int width, unsigned int height)
    : m_window(sf::VideoMode(std::max(width, static_cast<unsigned int>(GB_WIDTH)), std::max(height, static_cast<unsigned int>(GB_HEIGHT))), "gb_emulator", sf::Style::Default),
      m_framebuffer(), m_texture(), m_sprite(), m_input()
{
    sf::Vector2f window_size (m_window.getSize());
    m_framebuffer.create(GB_WIDTH, GB_HEIGHT);
    m_sprite.setScale(window_size.x / static_cast<float>(GB_WIDTH), window_size.y / static_cast<float>(GB_HEIGHT));
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);
    m_window.setKeyRepeatEnabled(false);
    m_window.display();
}

gb_renderer::~gb_renderer() {
}

gb_framebuffer& gb_renderer::get_framebuffer() {
    return m_framebuffer;
}

gb_input& gb_renderer::get_input() {
    return m_input;
}

bool gb_renderer::is_open() {
    return m_window.isOpen();
}

void gb_renderer::close() {
    m_window.close();
}

void gb_renderer::update(bool draw_framebuffer) {
    sf::Event event;

    while (m_window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                close();
                break;
            case sf::Event::KeyPressed:
                m_input.set_pressed(event.key.code);
                break;
            case sf::Event::KeyReleased:
                m_input.set_released(event.key.code);
                break;
            default: break;
        }
    }

    m_window.clear(sf::Color::White);

    if (draw_framebuffer) {
        m_texture.loadFromImage(m_framebuffer);
        m_sprite.setTexture(m_texture, true);
        m_window.draw(m_sprite);
    }

    m_window.display();
}
