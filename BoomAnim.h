#ifndef BOOMANIM_H
#define BOOMANIM_H

#include <SFML/Graphics.hpp>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265359
#endif // M_PI

struct Part{
    sf::Vertex vert;
    float direction;
    sf::Vector2f offset;
    float speed;
    float gravity;
};

class BoomAnim
{
public:
    BoomAnim(int nb_particles, const sf::Vector2f &center, float break_air, float speed_min, float speed_max);
    virtual ~BoomAnim();
    void update(float dt);
    void restart();
    void draw(sf::RenderTarget &target);
    bool isFinished();

private:
    std::vector<Part> particles;
    int nb_particles;
    sf::Vector2f center;
    float break_air;
    bool finished;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_real_distribution<float> angle_dist;
    std::uniform_real_distribution<float> speed_dist;
    std::uniform_real_distribution<float> gravity_dist;
    std::uniform_int_distribution<int> color_dist;
};

#endif // BOOMANIM_H
