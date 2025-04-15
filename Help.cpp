#include "Help.h"
#include "Config.h"

Help::Help(sf::RenderWindow &fenetre) :
    fenetre(fenetre)
{
}

Help::~Help(){
}

void Help::play(){
    bool continuer = true;
    sf::Event event;

    std::string strtext = {
        "To play with a partner, put your partners IP in \n\
        the options.txt file like this : \n\
        192.168.1.22 \n\
                    \n\
        Only local address works !!! \n\
        "
    };

    sf::Text text;
    sf::Font font;
    font.loadFromFile(FILE_BEBASNEUE);

    sf::Clock c;

    text = sf::Text(strtext, font, 30);
    text.setPosition(sf::Vector2f(400-text.getGlobalBounds().width/2, 300-text.getGlobalBounds().height/2));

    while (fenetre.isOpen() && continuer){
        while (fenetre.pollEvent(event)){
            if (event.type == sf::Event::Closed)fenetre.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || c.getElapsedTime().asMilliseconds()>15000)continuer = false;

        fenetre.clear();
        fenetre.draw(text);
        fenetre.display();
    }
}
