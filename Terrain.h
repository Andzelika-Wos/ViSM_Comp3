#ifndef TERRAIN_H
#define TERRAIN_H

#include "Vertex.h"
#include "VisualObject.h"
#include <QVector3D>
#include <QVector2D>
#include <vector>

class Terrain : public VisualObject
{
public:
    Terrain();

    float GetHeightAt(float x, float z) const;

    QVector3D GetNormal(const QVector3D position) const;

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};


#endif // TERRAIN_H
