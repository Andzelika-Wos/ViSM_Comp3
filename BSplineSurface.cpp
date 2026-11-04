#include "BSplineSurface.h"
#include <cmath>
#include <QDebug>

BSplineSurface::BSplineSurface() : VisualObject()
{
    mName = "biquadratic_surface";
    n_u = 4;
    n_v = 3;
    d_u = 2;
    d_v = 2;

    // Initiating knot vectors (clamped B-spline knot vectors)
    // For n_u = 4, d_u = 2 -> size = n_u + d_u + 1 = 7
    mu = {0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 2.0f, 2.0f};

    // For n_v = 3, d_v = 2 -> size = n_v + d_v + 1 = 6
    mv = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

    // Initiating control points (row-wise in u-direction, 4 x 3 grid)
    // Row 0 (v = 0)
    for (int i = 0; i < n_u; i++) {
        mc.push_back(QVector3D(static_cast<float>(i), 0.0f, 0.0f));
    }
    // Row 1 (v = 1)
    mc.push_back(QVector3D(0.0f, 1.0f, 0.0f));
    mc.push_back(QVector3D(1.0f, 1.0f, 2.0f));
    mc.push_back(QVector3D(2.0f, 1.0f, 2.0f));
    mc.push_back(QVector3D(3.0f, 1.0f, 0.0f));

    // Row 2 (v = 2)
    for (int i = 0; i < n_u; i++) {
        mc.push_back(QVector3D(static_cast<float>(i), 2.0f, 0.0f));
    }

    // Generating surface vertices and indices
    makeBiquadraticSurface();
}

// Finding the knot interval index where knotVector[i] <= t < knotVector[i+1]
int BSplineSurface::findKnotInterval(const std::vector<float>& knotVector, int degree, int numCP, float t)
{
    // Handle upper bound edge case
    if (t >= knotVector[numCP])
        return numCP - 1;

    for (int i = degree; i < numCP; ++i) {
        if (t >= knotVector[i] && t < knotVector[i + 1])
            return i;
    }
    return degree;
}

// Cox-de Boor recursion formula for B-spline basis functions (handles repeated knots)
float BSplineSurface::basisFunction(int i, int p, float t, const std::vector<float>& knot)
{
    if (p == 0) {
        if (i + 1 < knot.size() && knot[i] <= t && t < knot[i + 1])
            return 1.0f;
        if (i + 1 < knot.size() && t == knot.back() && t >= knot[i] && t <= knot[i + 1])
            return 1.0f;
        return 0.0f;
    }

    float term1 = 0.0f;
    float denom1 = knot[i + p] - knot[i];
    if (std::abs(denom1) > 1e-5f) {
        term1 = (t - knot[i]) / denom1 * basisFunction(i, p - 1, t, knot);
    }

    float term2 = 0.0f;
    float denom2 = knot[i + p + 1] - knot[i + 1];
    if (std::abs(denom2) > 1e-5f) {
        term2 = (knot[i + p + 1] - t) / denom2 * basisFunction(i + 1, p - 1, t, knot);
    }

    return term1 + term2;
}

// De Boor's triangular scheme for quadratic B-spline basis functions (degree d = 2)
// Returns 3 basis function values for u and 3 for v as QVector3D (storing the 3 weights)
std::pair<QVector3D, QVector3D> BSplineSurface::B2(float tu, float tv, int my_u, int my_v)
{
    auto computeBasis = [](float t, int my, const std::vector<float>& knot)
    {
        float w11 = 0.0f, w12 = 0.0f;

        float denom1 = knot[my + 1] - knot[my - 1];
        if (std::abs(denom1) > 1e-5f)
            w11 = (t - knot[my - 1]) / denom1;

        float denom2 = knot[my + 2] - knot[my];
        if (std::abs(denom2) > 1e-5f)
            w12 = (knot[my + 2] - t) / denom2;

        float denom3 = knot[my + 1] - knot[my];
        float b0 = 0.0f;
        if (std::abs(denom3) > 1e-5f)
            b0 = (knot[my + 1] - t) * w11 / denom3;

        float denom4 = knot[my + 1] - knot[my];
        float b2 = 0.0f;
        if (std::abs(denom4) > 1e-5f)
            b2 = (t - knot[my]) * w12 / denom4;

        // Boundary limits for clamped repeated knots
        float denom5 = knot[my] - knot[my - 1];
        float term1 = 0.0f;
        if (std::abs(denom5) > 1e-5f)
            term1 = (t - knot[my - 1]) * (1.0f - w11) / denom5;
        else
            term1 = 1.0f - w11; // Limit value when knot is repeated at lower boundary

        float denom6 = knot[my + 2] - knot[my + 1];
        float term2 = 0.0f;
        if (std::abs(denom6) > 1e-5f)
            term2 = (knot[my + 2] - t) * (1.0f - w12) / denom6;
        else
            term2 = 1.0f - w12; // Limit value when knot is repeated at upper boundary

        float b1 = term1 + term2;

        return QVector3D(b0, b1, b2);
    };

    QVector3D Bu = computeBasis(tu, my_u, mu);
    QVector3D Bv = computeBasis(tv, my_v, mv);

    return {Bu, Bv};
}

// Evaluates a point on the tensor-product surface using the outer product of basis weights and control points
QVector3D BSplineSurface::evaluateBiquadratic(int my_u, int my_v, QVector3D bu, QVector3D bv)
{
    QVector3D r(0.0f, 0.0f, 0.0f);

    // Extract 3x3 local control points corresponding to the active knot span
    // Control point grid index: cp_index(i, j) where i corresponds to u, j corresponds to v
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 3; i++) {
            int cp_u = my_u - 2 + i;
            int cp_v = my_v - 2 + j;

            // Clamps indices to ensure they never go out of bounds of the control point grid
            if (cp_u < 0) cp_u = 0;
            if (cp_u >= n_u) cp_u = n_u - 1;
            if (cp_v < 0) cp_v = 0;
            if (cp_v >= n_v) cp_v = n_v - 1;

            // Index in 1D control point vector (row-major: index = v * n_u + u)
            int index = cp_v * n_u + cp_u;

            if (index >= 0 && index < static_cast<int>(mc.size())) {
                float weight = bu[i] * bv[j];
                r += mc[index] * weight;
            }
        }
    }
    return r;
}

void BSplineSurface::makeBiquadraticSurface()
{
    float h = 0.05f; // Spacing

    // Insets the parameter domain slightly away from the exact clamped boundaries (0.0 and max) - prevents boundary collapse
    float u_min = mu[d_u] + 1e-3f;         // Starts just above 0.0f
    float u_max = mu[n_u] - 1e-3f;         // Ends just below 2.0f
    float v_min = mv[d_v] + 1e-3f;         // Starts just above 0.0f
    float v_max = mv[n_v + 1] - 1e-3f;     // Ends just below 1.0f

    // Computing nu and nv based on the parameter ranges [0, u_max] and [0, v_max]
    int nu = static_cast<int>((u_max - u_min) / h) + 1; // Should be ~21
    int nv = static_cast<int>((v_max - v_min) / h) + 1; // Should be ~11

    // Generates grid of points
    for (int i = 0; i < nv; i++) {
        for (int j = 0; j < nu; j++) {
            float u = u_min + j * h;
            float v = v_min + i * h;

            if (u > u_max) u = u_max;
            if (v > v_max) v = v_max;

            int my_u = findKnotInterval(mu, d_u, n_u, u);
            int my_v = findKnotInterval(mv, d_v, n_v, v);

            std::pair<QVector3D, QVector3D> koeff_par = B2(u, v, my_u, my_v);
            QVector3D p0 = evaluateBiquadratic(my_u, my_v, koeff_par.first, koeff_par.second);

            // Map UV coordinates relative to full span [0, 2] and [0, 1]
            float normalized_u = (u - mu[d_u]) / (mu[n_u] - mu[d_u]);
            float normalized_v = (v - mv[d_v]) / (mv[n_v + 1] - mv[d_v]);

            qDebug() << "Generated points: " << p0;

            // Adds vertex (position, color/normal, UV)
            mVertices.push_back(Vertex(p0, QVector3D(0.f, 1.f, 0.f), QVector2D(normalized_u, normalized_v)));

        }
    }

    // Computes indices for regular triangulation (quad topology: 4 corners)
    // 4 ===== 3
    // | \     |
    // |   \   |
    // 1 ===== 2
    for (int j = 0; j < nv - 1; j++) {
        for (int i = 0; i < nu - 1; i++) {
            uint32_t vertexIndex1 = j * nu + i;
            uint32_t vertexIndex2 = j * nu + (i + 1);
            uint32_t vertexIndex3 = (j + 1) * nu + (i + 1);
            uint32_t vertexIndex4 = (j + 1) * nu + i;

            // Triangle 1 (1, 2, 3)
            mIndices.push_back(vertexIndex1);
            mIndices.push_back(vertexIndex2);
            mIndices.push_back(vertexIndex3);

            // Triangle 2 (1, 3, 4)
            mIndices.push_back(vertexIndex1);
            mIndices.push_back(vertexIndex3);
            mIndices.push_back(vertexIndex4);
        }
    }
}
