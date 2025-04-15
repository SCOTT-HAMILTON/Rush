#include "Button.h"
#include "Config.h"

Button::Button(const sf::Vector2f &pos, const std::string &strtext)
{
    sf::Vector2f buttonScale(2, 1);
    button = sf::CircleShape(30);
    button.setScale(buttonScale);
    button.setFillColor(sf::Color(255, 157, 13));
    button.setOutlineColor(sf::Color(232, 78, 12));
    button.setOutlineThickness(5);
    button.setPosition(pos.x-button.getGlobalBounds().width/2, pos.y-button.getGlobalBounds().height/2);

    font.loadFromFile(FILE_BEBASNEUE);
    text = sf::Text(strtext, font, 30);
    this->text.setPosition(pos.x-this->text.getGlobalBounds().width/2-8, pos.y-this->text.getGlobalBounds().height/2-12);
}

Button::~Button()
{
    //dtor
}

void Button::draw(sf::RenderWindow &fenetre){
     sf::Vector2f pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(fenetre));
     if (button.getGlobalBounds().contains(pos)){
        button.setFillColor(sf::Color(255, 157, 113));
        button.setOutlineColor(sf::Color(232, 78, 112));
     }else{
        button.setFillColor(sf::Color(255, 157, 13));
        button.setOutlineColor(sf::Color(232, 78, 12));
     }
    fenetre.draw(button);
    fenetre.draw(text);

}

bool Button::mouseTouch(const sf::Vector2f &posmouse){
    return button.getGlobalBounds().contains(posmouse);
}
