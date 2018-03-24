#ifndef HELP_H
#define HELP_H

#include <SFML/Graphics.hpp>

class Help
{
public:
    Help(sf::RenderWindow &fenetre);
    virtual ~Help();
    void play();

private:
    sf::RenderWindow &fenetre;
};

#endif // HELP_H
