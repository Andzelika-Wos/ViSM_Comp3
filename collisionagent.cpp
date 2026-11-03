#include "Renderer.h"
#include "CollisionAgent.h"
#include <QApplication>
#include <QProcess>

CollisionAgent::CollisionAgent(Renderer& renderer) : mRenderer(renderer) {}

QVector3D CollisionAgent::vectorBetween(const VisualObject &a, const VisualObject &b)
{
    return a.Position - b.Position;  // Vector from A to B
}

void CollisionAgent::handleCollision(const VisualObject &a, const VisualObject &b)
{
    if (vectorBetween(a, b).length() < a.radius + b.radius)
    {
        if (b.getName() == "Enemy")
        {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}

void CollisionAgent::checkCollision()
{
    std::vector<VisualObject*>& Objects = mRenderer.getObjects(); //Returns mObjects

    if (Objects.empty())
    {
        qDebug() << "No objects on the map to check!";
    }
    for (size_t i = 0; i < Objects.size(); ++i)
    {
        if(i == 3)
        {
            continue;
        }
        else if (vectorBetween(*Objects.at(4), *Objects.at(i)).length() < Objects.at(4)->radius + Objects.at(i)->radius)
        {
            handleCollision(*Objects.at(4), *Objects.at(i));
        }
    }
}
