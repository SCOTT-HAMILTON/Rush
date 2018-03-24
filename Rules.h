#ifndef RULES_H
#define RULES_H

#include <iostream>
#include <SFML/Graphics.hpp>

class Rules
{
public:
    Rules(sf::RenderWindow &fenetre);
    virtual ~Rules();
    void play();

private:
    sf::RenderWindow &fenetre;
    int id;
    sf::Image img;
    sf::Text text;
    sf::Font font;
};

#endif // RULES_H
