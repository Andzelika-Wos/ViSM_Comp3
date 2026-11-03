#ifndef BALL_H
#define BALL_H

#include "ObjMesh.h"
#include "Terrain.h"
#include <QVector3D>

class Ball : public ObjMesh
{
public:
    Ball(const std::string& filename);

    void update(float dt, Terrain* terrain = nullptr);

    QVector3D velocity;
    QVector3D acceleration;

    float radius   = 0.2f;
    float gravity  = -9.81f;
    float friction = 0.998f;
};

#endif // BALL_H
