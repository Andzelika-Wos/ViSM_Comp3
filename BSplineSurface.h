#ifndef BSPLINESURFACE_H
#define BSPLINESURFACE_H

#include "visualobject.h"
#include <vector>
#include <utility>

class BSplineSurface : public VisualObject
{
private:
    int n_u = 4; // number of control points for u
    int n_v = 3; // number of control points for v
    int d_u = 2; // degree in u
    int d_v = 2; // degree in v
    float h_u = 0.1f, h_v = 0.1f; // spacing

    std::vector<float> mu; // knot vector u
    std::vector<float> mv; // knot vector v
    std::vector<QVector3D> mc; // control points (1D storage or 2D mapped)

    // Helper functions
    int findKnotInterval(const std::vector<float>& knotVector, int degree, int numCP, float t);

    float basisFunction(int i, int p, float t, const std::vector<float>& knot);

    std::pair<QVector3D, QVector3D> B2(float tu, float tv, int my_u, int my_v);
    QVector3D evaluateBiquadratic(int my_u, int my_v, QVector3D bu, QVector3D bv);
    void makeBiquadraticSurface();

public:
    BSplineSurface();
};

#endif // BSPLINESURFACE_H
