#ifndef ENEMY_H
#define ENEMY_H
#include <QVector3D>
#include "ObjMesh.h"

class Enemy : public ObjMesh
{
public:


    Enemy(const std::string& filename);

    void update(float deltaTime, const QVector3D& playerPosition);

    QVector3D deCasteljau(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, float t);

    void setControlPoints(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3);

private:

    QVector3D mP0, mP1, mP2, mP3;
    float mT{0.f};
    float mSpeed{0.5f};
    bool mForward{true};
    float mChaseRange;
};

#endif // ENEMY_H
