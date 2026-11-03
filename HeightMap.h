 #ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "ObjMesh.h"
#include "VisualObject.h"
#include <string>

class HeightMap : public VisualObject
{
public:
    HeightMap();

    void makeTerrain(std::string heightMapImage);

    void makeTerrain(unsigned char* textureData, int width, int height);

    float getHeight(float worldX, float worldZ);

    float barycentric(const QVector2D& p, const QVector3D& a, const QVector3D& b, const QVector3D& c);

private:
    int mWidth{ 0 };
    int mHeight{ 0 };
    int mChannels{ 0 };
};

#endif // HEIGHTMAP_H
