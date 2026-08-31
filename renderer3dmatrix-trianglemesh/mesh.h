#pragma once
#include <vector>
#include "math.h"

struct Mesh {
    std::vector<Vector3> positions;
    std::vector<Vector2> uvs;         // parallel to positions
    std::vector<Vector3> normals;     // parallel to positions
    std::vector<Triangle> triangles;
};
