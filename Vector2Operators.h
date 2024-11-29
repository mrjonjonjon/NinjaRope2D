#ifndef VECTOR2_OPERATORS_H
#define VECTOR2_OPERATORS_H

#include "raylib.h"
#include <raymath.h>
#include <vector>
#include <unordered_set>
float dot(const Vector2& v1, const Vector2& v2);
float cross(const Vector2& v1, const Vector2& v2);
Vector2 normalized(const Vector2& v);
bool IsPointOnEdge(Vector2 point, Vector2 vertex1, Vector2 vertex2);
float HighPrecisionSignedAngle(Vector2 v1, Vector2 v2);

std::optional<Vector2> TriangleSweep(
    const std::vector<Vector2>& curPoints,
    Vector2 anchor, 
    Vector2 oldPos, 
    Vector2 newPos,
    const std::vector<std::vector<Vector2>>& polygons,
    const std::unordered_set<Vector2>& forbidden,
    bool edge
);

bool IsPointInPolygon(Vector2 point, const std::vector<Vector2>& polygon,bool edge);

bool AreVectorsParallel(const Vector2& vec1, const Vector2& vec2);


std::optional<Vector2> rayint(Vector2 a, Vector2 b, Vector2 c, Vector2 d);
#endif // VECTOR2_OPERATORS_H
