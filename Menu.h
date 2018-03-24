#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Rules.h"
#include "Help.h"

enum GameMode{MULTI, SOLO};

class Menu
{
public:
    Menu(sf::RenderWindow &fenetre);
    virtual ~Menu();
    GameMode play();

private:
    sf::RenderWindow &fenetre;
    Button rulesbutton;
    Button soloPlayButton;
    Button multiPlayButton;
    Button helpButton;
    Rules rule_demos;
    Help help;
};

#endif // MENU_H
