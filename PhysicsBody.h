#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "VisualObject.h"
#include <QVector3D>

class PhysicsBody : public VisualObject
{
public:

    float mass = 1.0f;
    QVector3D position;
    QVector3D velocity;
    QVector3D acceleration;
    QVector3D force;


    PhysicsBody(float m = 1.0f, QVector3D startPos = {0,0,0})
        : mass(m), position(startPos){}

    void ApplyForce(const QVector3D& f)
    {
        force += f;
    }

    void update(float dt)
    {
        acceleration = force/mass;
        velocity += acceleration * dt;
        position += velocity * dt;
        force = QVector3D(0,0,0);
    }

};

#endif // PHYSICSBODY_H
