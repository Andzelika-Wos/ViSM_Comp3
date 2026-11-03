#include "Terrain.h"
#include <QVector2D>
#include <QVector3D>

Terrain::Terrain()
{

    Vertex v0{-1.0f,  0.0f, 0.0f,   1,0,0,  0,0};
    Vertex v1{-1.0f,  0.0f, 1.0f,   1,0,0,  0,1};
    Vertex v2{ 0.0f, -1.0f, 0.0f,   0,1,0,  0.5f,0};
    Vertex v3{ 0.0f, -1.0f, 1.0f,   0,1,0,  0.5f,1};
    Vertex v4{ 1.0f,  0.0f, 0.0f,   0,0,1,  1,0};
    Vertex v5{ 1.0f,  0.0f, 1.0f,   0,0,1,  1,1};

    mVertices = { v0, v1, v2, v3, v4, v5 };

    mIndices = {
        0, 2, 1,
        1, 2, 3,
        2, 4, 3,
        3, 4, 5
    };

    for (auto& v : mVertices) {
        v.x = 5.0f * v.x;
        v.y = 5.0f * v.y + 2.0f;
        v.z = 5.0f * v.z - 2.0f;
    }


    mMatrix.setToIdentity();

    qDebug() << "=== Terrain (WORLD) Vertices ===";
    for (auto& v : mVertices)
        qDebug() << "x:" << v.x << "y:" << v.y << "z:" << v.z;
    qDebug() << "================================";
}


float Terrain::GetHeightAt(float x, float z) const
{
    QVector2D point(x + 0.001f, z);

    for (size_t i = 0; i < mIndices.size(); i += 3)
    {
        const Vertex& v0 = mVertices[mIndices[i+0]];
        const Vertex& v1 = mVertices[mIndices[i+1]];
        const Vertex& v2 = mVertices[mIndices[i+2]];

        QVector2D a(v0.x, v0.z);
        QVector2D b(v1.x, v1.z);
        QVector2D c(v2.x, v2.z);


        QVector2D v0v1 = b - a;
        QVector2D v0v2 = c - a;
        QVector2D v0p  = point - a;

        float d00 = QVector2D::dotProduct(v0v1, v0v1);
        float d01 = QVector2D::dotProduct(v0v1, v0v2);
        float d11 = QVector2D::dotProduct(v0v2, v0v2);
        float d20 = QVector2D::dotProduct(v0p,  v0v1);
        float d21 = QVector2D::dotProduct(v0p,  v0v2);

        float denom = d00 * d11 - d01 * d01;
        if (denom == 0.0f) continue;

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        if (u >= 0.0f && v >= 0.0f && w >= 0.0f)
            return v0.y * u + v1.y * v + v2.y * w;
    }

    return 0.0f;
}

QVector3D Terrain::GetNormal(const QVector3D position) const
{
    QVector2D point(position.x() + 0.001f, position.z());


    for (size_t i = 0; i < mIndices.size(); i += 3)
    {
        const Vertex& v0 = mVertices[mIndices[i]];
        const Vertex& v1 = mVertices[mIndices[i + 1]];
        const Vertex& v2 = mVertices[mIndices[i + 2]];

        QVector2D a(v0.x, v0.z);
        QVector2D b(v1.x, v1.z);
        QVector2D c(v2.x, v2.z);

        QVector2D v0v1 = b - a;
        QVector2D v0v2 = c - a;
        QVector2D v0p  = point - a;

        float d00 = QVector2D::dotProduct(v0v1, v0v1);
        float d01 = QVector2D::dotProduct(v0v1, v0v2);
        float d11 = QVector2D::dotProduct(v0v2, v0v2);
        float d20 = QVector2D::dotProduct(v0p, v0v1);
        float d21 = QVector2D::dotProduct(v0p, v0v2);

        float denom = d00 * d11 - d01 * d01;
        if (denom == 0.0f) continue;

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;


        if (u >= 0.0f && v >= 0.0f && w >= 0.0f)
        {
            QVector3D A(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
            QVector3D B(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
            QVector3D n = QVector3D::crossProduct(A, B).normalized();
            if (n.y() < 0) n = -n;
            return n;
        }
    }


    return QVector3D(0, 1, 0);
}
