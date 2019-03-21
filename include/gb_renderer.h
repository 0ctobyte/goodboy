#ifndef GB_RENDERER_H_
#define GB_RENDERER_H_

#include <memory>

#include <SFML/Graphics.hpp>

#include "gb_framebuffer.h"
#include "gb_input.h"

#define GB_WIDTH  (160)
#define GB_HEIGHT (144)

class gb_renderer {
public:
    // Create a window at the specified width*height
    gb_renderer(unsigned int width, unsigned int height);
    ~gb_renderer();

    // Get a reference to the framebuffer
    gb_framebuffer& get_framebuffer();

    // Get a reference to the input class
    gb_input& get_input();

    // Check if the window is still open or if it has been closed
    bool is_open();

    // Update should be called every V-blank
    // draw_framebuffer indicates whether to draw the current contents of the framebuffer or not
    // (depending on if the LCD is on or off)
    void update(bool draw_framebuffer);

private:
    sf::RenderWindow m_window;
    gb_framebuffer   m_framebuffer;
    sf::Texture      m_texture;
    sf::Sprite       m_sprite;
    gb_input         m_input;
    bool             m_open;
};

#endif // GB_RENDERER_H_
