#include "HeightMap.h"
#include "Vertex.h"
#include "stb_image.h"

HeightMap::HeightMap()
{ }

void HeightMap::makeTerrain(std::string heightMapImage)
{
    //Load the heightmap image
    //Using stb_image to load the image
    stbi_uc* pixelData = stbi_load(heightMapImage.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
    if (pixelData == nullptr)
    {
        qDebug() << "Failed to load heightmap image!";
        return;
    }
    //Make the terrain from the pixel data
    makeTerrain(pixelData, mWidth, mHeight);
    stbi_image_free(pixelData);
}

void HeightMap::makeTerrain(unsigned char* textureData, int widthIn, int heightIn)
{

    float normal[3]{0.f, 1.f, 0.f};

    float horisontalSpacing{.2f};
    float heightSpacing{.02f};

    float heightPlacement{-5.f};

    unsigned short width = widthIn;       //Width == x-axis
    unsigned short depth = heightIn;      //Depth == z-axis

    //Temp variables for creating the mesh
    //Adding offset so the middle of the terrain will be in World origo
    float vertexXStart{ 0.f - width * horisontalSpacing / 2 };            // if world origo should be at center use: {0.f - width * horisontalSpacing / 2};
    float vertexZStart{ 0.f + depth * horisontalSpacing / 2 };            // if world origo should be at center use: {0.f + depth * horisontalSpacing / 2};

    //Loop to make the mesh from the values read from the heightmap (textureData)
    //Double for-loop to make the depth and the width of the terrain in one go
    for(int d{0}; d < depth; ++d)       //depth loop
    {
        for(int w{0}; w < width; ++w)   //width loop
        {
            //Heightmap image is actually stored as an one dimentional array - so calculating the correct index for column and row
            //and scale it according to variables
            // Calculate the correct index for the R value of each pixel
            int index = (w + d * width) * 4; // Each pixel has 4 bytes (RGBA)
            float heightFromBitmap = static_cast<float>(textureData[index]) * heightSpacing + heightPlacement;
            //                                          x - value                      y-value               z-value
            mVertices.emplace_back(Vertex{vertexXStart + (w * horisontalSpacing), heightFromBitmap, vertexZStart - (d * horisontalSpacing),
                //  dummy normal=0,1,0                  Texture coordinates
                normal[0],normal[1],normal[2],           w / (width - 1.f), d / (depth - 1.f)});
        }
    }

    // The mesh(grid) is drawn in quads with diagonals from lower left to upper right
    //          _ _
    //         |/|/|
    //          - -
    //         |/|/|
    //          - -
    //Making the indices for this mesh:
    for(int d{0}; d < depth-1; ++d)        //depth - 1 because we draw the last quad from depth - 1 and in negative z direction
    {
        for(int w{0}; w < width-1; ++w)    //width - 1 because we draw the last quad from width - 1 and in positive x direction
        {
            //Indices for one quad:
            mIndices.emplace_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.emplace_back(w + d * width + width);       // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.emplace_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * width + 1);           // 0 + 0 * mWidth + 1           = 1
            mIndices.emplace_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }

    //Calculating the normals for the mesh
    //Function not made yet:
    //calculateHeighMapNormals();
}

float HeightMap::barycentric(const QVector2D& p, const QVector3D& a, const QVector3D& b, const QVector3D& c)
{
    QVector2D a2D(a.x(), a.z());
    QVector2D b2D(b.x(), b.z());
    QVector2D c2D(c.x(), c.z());

    QVector2D v0 = b2D - a2D;
    QVector2D v1 = c2D - a2D;
    QVector2D v2 = p   - a2D;

    float d00 = QVector2D::dotProduct(v0, v0);
    float d01 = QVector2D::dotProduct(v0, v1);
    float d11 = QVector2D::dotProduct(v1, v1);
    float d20 = QVector2D::dotProduct(v2, v0);
    float d21 = QVector2D::dotProduct(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    if (denom == 0.0f)
        return a.y();

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return u * a.y() + v * b.y() + w * c.y();
}

float HeightMap::getHeight(float worldX, float worldZ)
{
    float horisontalSpacing = 0.2f;

    // Center the grid using the same offset as makeTerrain()
    float offsetX = -mWidth * horisontalSpacing / 2.0f;
    float offsetZ = +mHeight * horisontalSpacing / 2.0f;

    // Convert world coords to grid coords
    float localX = worldX - offsetX;
    float localZ = -(worldZ - offsetZ); // flipped due to terrain growing downward in Z

    int gridX = static_cast<int>(localX / horisontalSpacing);
    int gridZ = static_cast<int>(localZ / horisontalSpacing);

    if (gridX < 0 || gridZ < 0 || gridX >= mWidth - 1 || gridZ >= mHeight - 1)
        return 0.0f;

    float xCoord = fmod(localX, horisontalSpacing) / horisontalSpacing;
    float zCoord = fmod(localZ, horisontalSpacing) / horisontalSpacing;

    QVector3D a, b, c;
    int topLeftIndex = gridX + gridZ * mWidth;

    if (xCoord + zCoord <= 1.0f)
    {
        // Upper-left triangle
        a = QVector3D(mVertices[topLeftIndex].x, mVertices[topLeftIndex].y, mVertices[topLeftIndex].z);
        b = QVector3D(mVertices[topLeftIndex + 1].x, mVertices[topLeftIndex + 1].y, mVertices[topLeftIndex + 1].z);
        c = QVector3D(mVertices[topLeftIndex + mWidth].x, mVertices[topLeftIndex + mWidth].y, mVertices[topLeftIndex + mWidth].z);
    }
    else
    {
        // Lower-right triangle
        a = QVector3D(mVertices[topLeftIndex + 1 + mWidth].x, mVertices[topLeftIndex + 1 + mWidth].y, mVertices[topLeftIndex + 1 + mWidth].z);
        b = QVector3D(mVertices[topLeftIndex + mWidth].x, mVertices[topLeftIndex + mWidth].y, mVertices[topLeftIndex + mWidth].z);
        c = QVector3D(mVertices[topLeftIndex + 1].x, mVertices[topLeftIndex + 1].y, mVertices[topLeftIndex + 1].z);
    }

    return barycentric(QVector2D(worldX, worldZ), a, b, c);
}

