#include "enemy.h"

Enemy::Enemy(const std::string& filename) : ObjMesh(filename)
{
    isEnemy = true;
    mP0 = QVector3D(-2.5f, 0.f, -2.f);
    mP1 = QVector3D(1.f, 0.f, -0.5f);
    mP2 = QVector3D(-1.5f, 0.f, 1.f);
    mP3 = QVector3D(1.5f, 0.f, 2.5f);

    mChaseRange = 6.0f;
}

QVector3D Enemy::deCasteljau(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, float t)
{
    QVector3D a = (1-t)* p0 + t * p1;
    QVector3D b = (1-t)* p1 + t * p2;
    QVector3D c = (1-t)* p2 + t * p3;

    QVector3D Q1 = (1-t)* a + t * b;
    QVector3D Q2 = (1-t)* b + t * c;

    QVector3D R = (1-t) * Q1 + t * Q2;


    return R;
}

//Bernstein basis funksjon
// void RouteManager::calculatePoints()
// {
//     QVector3D P0 = pointA;
//     QVector3D P1 = pointB;
//     QVector3D P2 = pointC;

//     patrolRoute.clear();
//     const int numSegments = 100; // Number of points along the curve

//     for (int i = 0; i <= numSegments; ++i) {
//         float t = static_cast<float>(i) / numSegments;

//         float b0 = (1 - t) * (1 - t);     // Bernstein basis for P0
//         float b1 = 2 * (1 - t) * t;       // Bernstein basis for P1
//         float b2 = t * t;                 // Bernstein basis for P2

//         QVector3D point = b0 * P0 + b1 * P1 + b2 * P2;

//         patrolRoute.push_back(point);
//     }
// }

void Enemy::update(float deltaTime, const QVector3D& playerPosition)
{
    float distance = (playerPosition - Position).length();

    if (distance <= mChaseRange) {

        setTextureType(1);

        QVector3D direction = (playerPosition - Position).normalized();
        Position += direction * mSpeed * deltaTime * 20;


        setXPosition(Position.x(), Position.z());
    }
    else {
        setTextureType(2);

        if (mForward)
            mT += deltaTime * mSpeed;
        else
            mT -= deltaTime * mSpeed;

        if (mT >= 1.0f) {
            mT = 1.0f;
            mForward = false;
        }
        else if (mT <= 0.0f) {
            mT = 0.0f;
            mForward = true;
        }

        QVector3D pos = deCasteljau(mP0, mP1, mP2, mP3, mT);
        setXPosition(pos.x(), pos.z());
    }
}

