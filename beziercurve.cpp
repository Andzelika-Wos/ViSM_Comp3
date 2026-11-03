#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
    drawType = 1; // line drawing

    // Set control points (you can also add setters later)
    mP0 = QVector3D(-2.5f, 0.f, -2.f);
    mP1 = QVector3D(1.f, 0.f, -0.5f);
    mP2 = QVector3D(-1.5f, 0.f, 1.f);
    mP3 = QVector3D(1.5f, 0.f, 2.5f);

    const int segments = 20;
    for (int i = 0; i <= segments; ++i)
    {
        float t = i / float(segments);
        QVector3D pos = deCasteljau(mP0, mP1, mP2, mP3, t);

        // Add vertex with red color
        mVertices.push_back(Vertex{
            pos.x(), pos.y(), pos.z(),
            1.0f, 0.0f, 0.0f,  // RGB color
            0.f, 0.f           // UV coords
        });
    }

    // Indices for lines connecting consecutive points
    for (int i = 0; i < segments; ++i)
    {
        mIndices.push_back(i);
        mIndices.push_back(i + 1);
    }
}

QVector3D BezierCurve::deCasteljau(const QVector3D& p0, const QVector3D& p1,
                                   const QVector3D& p2, const QVector3D& p3, float t)
{
    QVector3D a = (1 - t) * p0 + t * p1;
    QVector3D b = (1 - t) * p1 + t * p2;
    QVector3D c = (1 - t) * p2 + t * p3;

    QVector3D Q1 = (1 - t) * a + t * b;
    QVector3D Q2 = (1 - t) * b + t * c;

    return (1 - t) * Q1 + t * Q2;
}
