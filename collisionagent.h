#ifndef COLLISIONAGENT_H
#define COLLISIONAGENT_H

class Renderer;

#include "visualobject.h"

class CollisionAgent
{
public:
    CollisionAgent(Renderer& renderer);

    QVector3D vectorBetween(const VisualObject& a, const VisualObject& b);
    void handleCollision(const VisualObject& a, const VisualObject& b);
    void checkCollision();

private:
    Renderer& mRenderer;


};

#endif // COLLISIONAGENT_H
