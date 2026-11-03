#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "VisualObject.h"
#include <QVector3D>

class BezierCurve : public VisualObject
{
public:
    BezierCurve();

private:
    QVector3D mP0, mP1, mP2, mP3;

    QVector3D deCasteljau(const QVector3D& p0, const QVector3D& p1,
                          const QVector3D& p2, const QVector3D& p3, float t);
};

#endif // BEZIERCURVE_H
