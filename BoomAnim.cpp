#include "BoomAnim.h"

BoomAnim::BoomAnim(int nb_particles, const sf::Vector2f &center, float break_air, float speed_min, float speed_max) :
    nb_particles(nb_particles), center(center), break_air(break_air)
{
    angle_dist = std::uniform_real_distribution<float>(0,M_PI*2);
    rng = std::mt19937(rd());
    speed_dist = std::uniform_real_distribution<float>(speed_min, speed_max);
    gravity_dist = std::uniform_real_distribution<float>(0, 0);
    color_dist = std::uniform_int_distribution<int>(0, 2);

    for (int i = 0; i < nb_particles; i++){
        Part part;
        int color_rand = color_dist(rng);
        sf::Color color(0, 0, 0);
        if (color_rand == 0)color.r = 255;
        else if (color_rand == 1)color.g = 255;
        else color.b = 255;
        part.vert = sf::Vertex(center, color);
        part.direction = angle_dist(rng);
        part.speed = speed_dist(rng);
        part.offset.x = std::cos(part.direction);
        part.offset.y = std::sin(part.direction);
        part.gravity = gravity_dist(rng);
        particles.push_back(part);
    }
    finished = false;
}

void BoomAnim::update(float dt){
    int somme_speed = 0;
    if (!finished){
        for (int i = 0; i < particles.size(); i++){
            sf::Vector2f pos = particles[i].vert.position;
            pos += particles[i].offset*particles[i].speed*dt;
            pos.y += particles[i].gravity*dt;
            particles[i].vert.position = pos;
            particles[i].speed += (0-particles[i].speed)*break_air;
            if (particles[i].speed<=0.1)particles[i].speed = 0;
            somme_speed+=particles[i].speed;
        }
        if (somme_speed<2){
            finished = true;
        }
    }
}

bool BoomAnim::isFinished(){
    return finished;
}

void BoomAnim::draw(sf::RenderTarget &target){
    for (int i = 0; i < particles.size(); i++){
        target.draw(&particles[i].vert, 1, sf::Points);
    }
}

void BoomAnim::restart(){
    finished = false;
    for (int i = 0; i < nb_particles; i++){
        if (i>=particles.size())particles.push_back(Part());
        int color_rand = color_dist(rng);
        sf::Color color(0, 0, 0);
        if (color_rand == 0)color.r = 255;
        else if (color_rand == 1)color.g = 255;
        else color.b = 255;
        particles[i].vert.color = color;
        particles[i].vert.position = center;
        particles[i].direction = angle_dist(rng);
        particles[i].speed = speed_dist(rng);
        particles[i].offset.x = std::cos(particles[i].direction);
        particles[i].offset.y = std::sin(particles[i].direction);
        particles[i].gravity = gravity_dist(rng);
    }
}

BoomAnim::~BoomAnim()
{
    //dtor
}
