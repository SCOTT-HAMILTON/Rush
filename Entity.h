#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif // M_PI

using idtype = unsigned long long int;

class Entity
{
public:
    Entity(std::shared_ptr<sf::Shape> shape, const sf::Vector2f &pos, const sf::Vector2f &speed = {5, 5}, float direction = 0, float mylife = 500);
    virtual ~Entity();
    void draw(sf::RenderTarget &target);
    void update(float dt);
    const sf::Vector2f &getPos() const;
    void right(float dt);
    void left(float dt);
    void up(float dt);
    void down(float dt);
    void move(float x, float y);
    const std::string &getType() const;
    void setType(const std::string &type);
    void setDead();
    bool isDead() const;
    sf::FloatRect getRect();
    void setSpeed(const sf::Vector2f &speed);
    sf::Vector2f getSpeed() const;
    void setPos(const sf::Vector2f &pos);
    const sf::Vector2f &getPos();
    void setWantedPos(const sf::Vector2f &wantedpos);
    const sf::Vector2f &getWantedPos();
    void setPrecPos(const sf::Vector2f &precpos);
    const sf::Vector2f &getPrecPos();
    void stopMoves();
    float getLife() const;
    void setLife(float val);
    const std::string &getTypeCollided() const;
    void setTypeCollided(const std::string &typecollided);
    float getDirection();
    void setDirection(float val);
    sf::Clock cooler;
    void decreaseLife(float val);

    void dontApplyGravity();

    void restart();

private:
    sf::Vector2f speed;
    const sf::Vector2f max_speed;
    float min_speed;
    const float break_air;
    float gravity;
    float direction;
    bool applyGravity;
    bool dead;
    idtype myid;
    float life_start;
    static idtype cur_id;
    sf::Vector2f prec_pos;
    sf::Vector2f pos;
    sf::Vector2f first_pos;
    sf::Vector2f wanted_pos;
    std::shared_ptr<sf::Shape> shape;
    std::string type;
    std::string type_collided;
    float life;
};

#endif // ENTITY_H
