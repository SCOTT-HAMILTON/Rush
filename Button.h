#ifndef BUTTON_H
#define BUTTON_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

class Button
{
public:
    Button(const sf::Vector2f &pos, const std::string &strtext);
    virtual ~Button();
    void draw(sf::RenderWindow &fenetre);
    bool mouseTouch(const sf::Vector2f &posmouse);

private:
    sf::CircleShape button;
    sf::Text text;
    sf::Font font;
    bool touched;
};

#endif // BUTTON_H
