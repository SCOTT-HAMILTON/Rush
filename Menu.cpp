#include "Menu.h"

Menu::Menu(sf::RenderWindow &fenetre) :
    fenetre(fenetre), soloPlayButton({400, 300}, "Solo"),
    rulesbutton({400, 380}, "Rules"), rule_demos(fenetre), multiPlayButton({400, 220}, "Multi"),
    helpButton({400, 460}, "Help"), help(fenetre)
{

}

Menu::~Menu()
{
    //dtor
}

GameMode Menu::play(){
    bool continuer = true;
    sf::Event event;
    GameMode mode = SOLO;
    while (fenetre.isOpen() && continuer){
        while (fenetre.pollEvent(event)){
            if (event.type == sf::Event::Closed)fenetre.close();
            if (event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left){
                    sf::Vector2f mousepos(event.mouseButton.x, event.mouseButton.y);
                    if (soloPlayButton.mouseTouch(mousepos)){
                        continuer = false;
                        mode = SOLO;
                    }else if (multiPlayButton.mouseTouch(mousepos)){
                        continuer = false;
                        mode = MULTI;
                    }
                    else if (rulesbutton.mouseTouch(mousepos)){
                        rule_demos.play();
                    }else if (helpButton.mouseTouch(mousepos)){
                        help.play();
                    }
                }
            }
        }

        fenetre.clear();
        soloPlayButton.draw(fenetre);
        multiPlayButton.draw(fenetre);
        rulesbutton.draw(fenetre);
        helpButton.draw(fenetre);
        fenetre.display();
    }

    return mode;
}
