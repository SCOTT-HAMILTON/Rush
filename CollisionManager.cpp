#include "CollisionManager.h"

bool startsWidth(const std::string &cmpstr, const std::string &prefixe){
    return (cmpstr.rfind(prefixe, 0) == 0);
}

CollisionManager::CollisionManager()
{
    //ctor
}

CollisionManager::~CollisionManager()
{
    //dtor
}

void CollisionManager::updateEntities(float dt){
    for (int i = entities.size()-1; i>=0; i--){
        std::shared_ptr<Entity> ptr = entities[i].lock();
        if (ptr == nullptr){
            entities.erase(entities.begin()+i);
            continue;
        }

        if ((startsWidth(ptr->getType(), "missile") || startsWidth(ptr->getType(), "bomb")) && ptr->getPos().y>=550-ptr->getRect().height){
            ptr->setDead();
            entities.erase(entities.begin()+i);
            continue;
        }

        ptr->update(dt);
        ptr->setTypeCollided("");
        if (ptr->getPos().y+ptr->getRect().height>550){
            ptr->setPos(sf::Vector2f(ptr->getPos().x, 550-ptr->getRect().height));
            ptr->setWantedPos(ptr->getPos());
            ptr->setSpeed(sf::Vector2f(ptr->getSpeed().x, 0));
        }
    }
}

void CollisionManager::updateCollisions(){

    for (int i = 0; i < entities.size(); i++){
        std::shared_ptr<Entity> ptr = entities[i].lock();
        if (ptr == nullptr){
            entities.erase(entities.begin()+i);
            continue;
        }
        for (int j = i+1; j < entities.size(); j++){
            std::shared_ptr<Entity> ptr = entities[j].lock();
            if (ptr == nullptr){
                entities.erase(entities.begin()+j);
                continue;
            }
            std::shared_ptr<Entity> ptr_i = entities[i].lock();
            std::shared_ptr<Entity> ptr_j = entities[j].lock();
            if ((ptr_i->getType() == "perso" && ptr_j->getType() == "missile-perso") ||
                (ptr_j->getType() == "perso" && ptr_i->getType() == "missile-perso")){
                continue;
            }
            sf::FloatRect rect1 = ptr_i->getRect();
            sf::FloatRect rect2 = ptr_j->getRect();

            if (rect1.intersects(rect2)){
                if (ptr_i->getType() == "boat" && (ptr_j->getType() == "missile-boat" || ptr_j->getType() == "boat"))continue;
                if (ptr_j->getType() == "boat" && (ptr_i->getType() == "missile-boat" || ptr_i->getType() == "boat"))continue;
                if (ptr_i->getType() == "perso" && ptr_j->getType() == "perso")continue;

                if ((startsWidth(ptr_i->getType(), "missile") || startsWidth(ptr_i->getType(), "bomb")) &&
                    (startsWidth(ptr_j->getType(), "missile") || startsWidth(ptr_j->getType(), "bomb"))
                ){
                    continue;
                }

                bool i_is_airport = ptr_i->getType() == "airport";
                bool j_is_airport = ptr_j->getType() == "airport";

                if (!i_is_airport && !j_is_airport && ptr_i->getType() != "missile-bot" && ptr_j->getType() != "missile-bot"){
                    if (ptr_i->getType() != "perso" || ptr_j->getType() != "perso"){
                        sf::Vector2f speed;
                        speed = ptr_i->getSpeed();
                        speed += ptr_j->getSpeed();
                        //speed.y = 0;
                        ptr_i->setSpeed(speed);
                        ptr_j->setSpeed(speed);
                    }
                }

                if (!i_is_airport && j_is_airport && ptr_i->getType() != "perso")ptr_i->setDead();
                if (!j_is_airport && i_is_airport && ptr_j->getType() != "perso")ptr_j->setDead();

                if (ptr_i->getType() != "airport"){
                    ptr_i->setPos(ptr_i->getPrecPos());
                    ptr_i->setWantedPos(ptr_i->getPos());
                    ptr_i->setTypeCollided(ptr_j->getType());
                }
                if (ptr_j->getType() != "airport"){
                    ptr_j->setPos(ptr_j->getPrecPos());
                    ptr_j->setWantedPos(ptr_j->getPos());
                    ptr_j->setTypeCollided(ptr_i->getType());
                }

                if (ptr_i->getType() == "missile-bot" && ptr_j->getType() == "perso" ){
                    ptr_i->setDead();
                }

                if (ptr_j->getType() == "missile-bot" && ptr_i->getType() == "perso" ){
                    ptr_j->setDead();
                }
            }
        }
    }
}

void CollisionManager::addEntity(std::shared_ptr<Entity> entity){
    entities.push_back(entity);

}
