#include <algorithm>

#include "gb_renderer.h"

gb_renderer::gb_renderer(unsigned int width, unsigned int height)
    : m_window(sf::VideoMode(std::max(width, static_cast<unsigned int>(GB_WIDTH)), std::max(height, static_cast<unsigned int>(GB_HEIGHT))), "gb_emulator", sf::Style::Default),
      m_framebuffer(), m_texture(), m_sprite(), m_open(true)
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

bool gb_renderer::is_open() {
    return m_open;
}

void gb_renderer::update(bool draw_framebuffer) {
    sf::Event event;

    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_open = false;
            m_window.close();
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
