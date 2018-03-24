#include "Entity.h"
idtype Entity::cur_id = 0;
Entity::Entity(std::shared_ptr<sf::Shape> shape, const sf::Vector2f &pos, const sf::Vector2f &speed, float direction, float mylife) :
    shape(shape), break_air(0.05f), max_speed(speed), applyGravity(true),
    min_speed(0.00001), gravity(0.05), dead(false), life(mylife)
{
    this->pos = pos;
    this->direction = direction;
    first_pos = pos;
    wanted_pos = pos;
    prec_pos = pos;
    myid = cur_id;
    cur_id++;
    life_start = mylife;
}

void Entity::restart(){
    //if (type == "perso")std::cout << "perso gravity : " << gravity << " air br : " << break_air << " maxseed : " << max_speed.x << ", " << max_speed.y<< std::endl;
    pos = first_pos;
    wanted_pos = first_pos;
    prec_pos = first_pos;
    speed = {0, 0};
    dead = false;
    life = life_start;
}

Entity::~Entity()
{
}

void Entity::dontApplyGravity(){
    applyGravity = false;
}

void Entity::draw(sf::RenderTarget &target){
    target.draw((*(shape)));
}

void Entity::update(float dt){
    //if (speed.y>min_speed)std::cout << "speed : " << speed.y << std::endl;
    if (life == 0){
        dead = true;
    }
    wanted_pos += speed;
    prec_pos = pos;
    pos = wanted_pos;
    float speed_x_b = speed.x;
    float speed_y_b = speed.y;
    speed.x += (0-speed.x)*break_air;
    //speed.y += (0-speed.y)*break_air;
    if (speed.y<0)speed.y += (0-speed.y)*break_air;
    if (applyGravity)speed.y+=gravity*dt;
    if ((speed_x_b<0&&speed.x>0)||(speed_x_b>0&&speed.x<0) || std::abs(speed.x) < min_speed)speed.x = 0;
    if ((speed_y_b<0&&speed.y>0)||(speed_y_b>0&&speed.y<0) || std::abs(speed.y) < min_speed)speed.y = 0;
    shape->setPosition(pos);

    shape->setRotation(direction*180/M_PI);
}

void Entity::right(float dt){
    speed.x = max_speed.x*dt;
}
void Entity::left(float dt){
    speed.x = -max_speed.x*dt;
}
void Entity::up(float dt){
    speed.y = -max_speed.y*dt;
}
void Entity::down(float dt){
    speed.y = max_speed.y*dt;
}

void Entity::move(float x, float y){
    if (x)speed.x = x;
    if (y)speed.y = y;
}

const sf::Vector2f &Entity::getPos() const{
    return pos;
}

const std::string &Entity::getType() const{
    return type;
}
void Entity::setType(const std::string &type){
    this->type = type;
}

void Entity::setDead(){
    dead = true;
}
bool Entity::isDead() const{
    return dead;
}

sf::FloatRect Entity::getRect(){
    return shape->getGlobalBounds();
}

void Entity::setSpeed(const sf::Vector2f &speed){
    this->speed = speed;
}

sf::Vector2f Entity::getSpeed() const{
    return speed;
}

void Entity::setPos(const sf::Vector2f &pos){
    this->pos = pos;
}
const sf::Vector2f &Entity::getPos(){
    return pos;
}
void Entity::setWantedPos(const sf::Vector2f &wantedpos){
    wanted_pos = wantedpos;
}
const sf::Vector2f &Entity::getWantedPos(){
    return wanted_pos;
}

void Entity::stopMoves(){
    speed = {0, 0};
}

void Entity::setPrecPos(const sf::Vector2f &precpos){
    prec_pos = precpos;
}

const sf::Vector2f &Entity::getPrecPos(){
    return prec_pos;
}

float Entity::getLife() const{
    return life;
}
void Entity::setLife(float val){
    life = val;
}

const std::string &Entity::getTypeCollided() const{
    return type_collided;
}

void Entity::setTypeCollided(const std::string &typecollided){
    type_collided = typecollided;
}

float Entity::getDirection(){
    return direction;
}

void Entity::setDirection(float val){
    direction = val;
}

void Entity::decreaseLife(float val){
    life -= val;
}
