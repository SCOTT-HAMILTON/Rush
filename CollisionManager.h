#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "Entity.h"

class CollisionManager
{
public:
    CollisionManager();
    virtual ~CollisionManager();
    void updateCollisions();
    void addEntity(std::shared_ptr<Entity> entity);
    void updateEntities(float dt);

private:
    std::vector<std::weak_ptr<Entity>> entities;
};

#endif // COLLISIONMANAGER_H
