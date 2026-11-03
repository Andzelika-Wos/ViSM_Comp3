#include "Ball.h"
#include "Terrain.h"
#include <QDebug>

Ball::Ball(const std::string& filename)
    : ObjMesh(filename)
{
    scale(1.f);
    setPosition(4.0f, 0.0f, 1.0f);
    velocity = QVector3D(0.0f, 0.0f, 0.0f);

}

void Ball::update(float dt, Terrain* terrain)
{
    if (!terrain) return;

    QVector3D pos = Position;


    QVector3D normal = terrain->GetNormal(pos).normalized();
    float groundY = terrain->GetHeightAt(pos.x(), pos.z());

    //qDebug() << "Ball pos:" << pos << " GroundY:" << groundY << " Normal:" << normal;


    QVector3D gravity(0.0f, -9.81f, 0.0f);

    QVector3D tangentAcc = gravity - QVector3D::dotProduct(gravity, normal) * normal;

    velocity += tangentAcc * dt;


    velocity *= friction;

    pos += velocity * dt;

    if (pos.y() < groundY + radius)
    {
        pos.setY(groundY + radius);
    }

    Position = pos;
    mMatrix.setToIdentity();
    mMatrix.translate(Position);
    mMatrix.scale(1.f);
}


