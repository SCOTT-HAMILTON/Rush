#include "Rules.h"

Rules::Rules(sf::RenderWindow &fenetre) :
    fenetre(fenetre)
{

}

Rules::~Rules()
{
    //dtor
}


void Rules::play(){

    id = 0;
    if (!img.loadFromFile("demos_rule_degat.png")){
        std::cout << "image rule doesn't exist!" << std::endl;
    }
    font.loadFromFile("BebasNeue-Regular.otf");
    text = sf::Text("Don't touch the boat's missiles !!!", font, 35);
    text.setPosition(200, 100);
    text.setFillColor(sf::Color(250, 250, 250));

    bool continuer = true;
    sf::Event event;

    sf::Sprite sp;
    sf::Texture texture;
    sf::IntRect rect(id*800, 0, 800, 600);
    texture.loadFromImage(img, rect);
    sp.setTexture(texture);

    sf::Clock c;
    int fps = 25;
    int width = 798;

    int id_max = static_cast<int>(img.getSize().x/width);
    std::string status("degat");
    sf::Clock pause;

    bool anim_transit = false;
    int delay = 2000;

    while (fenetre.isOpen() && continuer){
        while (fenetre.pollEvent(event)){
            if (event.type == sf::Event::Closed)fenetre.close();
            if (event.key.code == sf::Keyboard::Escape){
                continuer = false;
                break;
            }
        }

        if (c.getElapsedTime().asMilliseconds()>1000/fps){
            if (img.getSize().x == 0 || img.getSize().y == 0)continuer = false;
            id++;
            if (id >= id_max){
                id--;
                if (status == "land" && !anim_transit){
                    status = "end";
                    pause.restart();
                    anim_transit = true;
                }
                if (status == "missile" && !anim_transit){
                    status = "land";
                    pause.restart();
                    anim_transit = true;
                }
                if (status == "degat" && !anim_transit){
                    status = "missile";
                    pause.restart();
                    anim_transit = true;
                }

                if (pause.getElapsedTime().asMilliseconds()>delay){
                    anim_transit = false;
                    if (status == "end"){
                        continuer = false;
                        break;
                    }
                    id = 0;
                    img = sf::Image();
                    if (status == "missile"){
                        if (!img.loadFromFile("demos_rule_shoot.png")){
                            std::cout << "image rule doesn't exist!" << std::endl;
                            continuer = false;
                            break;
                        }
                        text.setString("You can shoot with space !");
                        delay = 3000;
                    }else if (status == "land"){
                        if (!img.loadFromFile("demos_rule_land.png")){
                            std::cout << "image rule doesn't exist!" << std::endl;
                            continuer = false;
                            break;
                        }
                        text.setString("You can land now !!!");
                        delay = 1700;
                        text.setPosition(50, 200);
                    }

                    id_max = static_cast<int>(img.getSize().x/width);
                }
            }

            if (status == "missile" && id == 100){
                text.setString("Drop bombs with X!! Bombs are you're best weapons!!!");
                text.setPosition(50, 250);
            }

            c.restart();
            texture.loadFromImage(img, sf::IntRect(id * width, 0, width, 600));
            sp.setTexture(texture);


        }

        fenetre.clear();
        fenetre.draw(sp);
        fenetre.draw(text);
        fenetre.display();
    }
}
