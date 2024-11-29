#include "Vector2Operators.h"
#include <raymath.h>
#include <unordered_set>
// Implement operator overloads



namespace std {
    template <>
    struct hash<Vector2> {
        size_t operator()(const Vector2& v) const {
            // A simple hash function using a combination of x and y
            size_t h1 = hash<float>{}(v.x);
            size_t h2 = hash<float>{}(v.y);
            return h1 ^ (h2 << 1);  // Combine the hashes
        }
    };
}
bool IsPointOnEdge(Vector2 point, Vector2 vertex1, Vector2 vertex2)
{
    // Check if the point lies on the line segment using vector cross product and bounds
    float crossProduct = (point.y - vertex1.y) * (vertex2.x - vertex1.x) - (point.x - vertex1.x) * (vertex2.y - vertex1.y);
    if (abs(crossProduct) > 1e-2)
    {
        return false; // Not on the line.
    }

    // Check if the point is within the bounds of the segment
    float minX = std::min(vertex1.x, vertex2.x), maxX = std::max(vertex1.x, vertex2.x);
    float minY = std::min(vertex1.y, vertex2.y), maxY = std::max(vertex1.y, vertex2.y);

    return point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY;
}
float dot(const Vector2& v1, const Vector2& v2) {
    return Vector2DotProduct(v1, v2);
}

float cross(const Vector2& v1, const Vector2& v2) {
    return v1.x * v2.y - v1.y * v2.x; // Cross product in 2D
}

Vector2 normalized(const Vector2& v){
    Vector2 result = { 0 };
    float length = sqrtf((v.x*v.x) + (v.y*v.y));

    if (length > 0)
    {
        float ilength = 1.0f/length;
        result.x = v.x*ilength;
        result.y = v.y*ilength;
    }

    return result;
}

float HighPrecisionSignedAngle(Vector2 a, Vector2 b) {

return atan2( a.x*b.y - a.y*b.x, a.x*b.x + a.y*b.y );

}

// TriangleSweep Function
std::optional<Vector2> TriangleSweep(
    const std::vector<Vector2>& curPoints,
    Vector2 anchor, 
    Vector2 oldPos, 
    Vector2 newPos,
    const std::vector<std::vector<Vector2>>& polygons,
    const std::unordered_set<Vector2>& forbidden,
    bool  edge
) {

    Vector2 anchorToOld = {oldPos.x - anchor.x, oldPos.y - anchor.y};
    Vector2 anchorToNew = {newPos.x - anchor.x, newPos.y - anchor.y};
    Vector2 oldToNew = {newPos.x - oldPos.x, newPos.y - oldPos.y};

    Vector2 point1 = anchor;
    Vector2 point2 = oldPos;
    Vector2 point3 = newPos;

    std::vector<Vector2> triangle = {point1, point2, point3};

    float minAngle = std::numeric_limits<float>::max();
    float maxDistance = std::numeric_limits<float>::lowest();
    std::optional<Vector2> ans = std::nullopt;

    // Iterate over all polygons
    for (const auto& polygon : polygons) {
        for (const auto& point : polygon) {
            // Skip points that are part of the triangle
            if ((point.x == point1.x && point.y == point1.y) || 
                (point.x == point2.x && point.y == point2.y) || 
                (point.x == point3.x && point.y == point3.y)) {
                continue;
            }

            if (forbidden.find(point) != forbidden.end()) {
                continue;
            }

            // Check if the point is inside the triangle
            if (IsPointInPolygon(point, triangle,edge)) {
                Vector2 anchorToPoint = {point.x - anchor.x, point.y - anchor.y};

                // Compute angle from old vector to the current point
                float angleFromOldToNew = Vector2Angle(anchorToOld, anchorToPoint);

                // Calculate distance from anchor to the point
                float distance = std::sqrt(std::pow(point.x - anchor.x, 2) + std::pow(point.y - anchor.y, 2));

                // Compare and update based on primary (angle) and secondary (distance) keys
                if (angleFromOldToNew < minAngle || 
                    (angleFromOldToNew == minAngle && distance > maxDistance)) {
                    minAngle = angleFromOldToNew;
                    maxDistance = distance;
                    ans = point;
                }
            }
        }
    }

    // Log the best candidate (for debugging, optional)
    if (ans.has_value()) {
        printf("Best candidate: (%f, %f)\n", ans->x, ans->y);
    }

    return ans;
}







bool IsPointInPolygon(Vector2 point, const std::vector<Vector2>& polygon, bool edge = true) {
    if (polygon.size() < 3) return false; // Ensure it's a valid polygon

    // Convert std::vector<Vector2> to a C-style array of Vector2 for Raylib's function
    Vector2* polygonArray = const_cast<Vector2*>(polygon.data());

    // Use Raylib's function for the point-in-polygon check
    bool inside = CheckCollisionPointPoly(point, polygonArray, polygon.size());
    constexpr float tolerance = 0.1f; // Adjust based on your use case

    if (!edge) {
        // If the point is inside but edge is false, check if it's exactly on an edge
        for (size_t i = 0; i < polygon.size(); i++) {
            Vector2 v1 = polygon[i];
            Vector2 v2 = polygon[(i + 1) % polygon.size()];
            if (CheckCollisionPointLine(point, v1, v2, tolerance)) {
                return false; // Point is on an edge, and edge == false
            }
        }
    }

    if (edge) {
        // If the point is not inside, check if it's on any edge
        for (size_t i = 0; i < polygon.size(); i++) {
            Vector2 v1 = polygon[i];
            Vector2 v2 = polygon[(i + 1) % polygon.size()];
            if (CheckCollisionPointLine(point, v1, v2, tolerance)) {
                return true; // Point is on an edge
            }
        }
    }

    return inside;
}




bool AreVectorsParallel(const Vector2& vec1, const Vector2& vec2) {
    return std::fabs(cross(vec1, vec2)) < 1e-10; // Use an epsilon for floating-point comparisons
}

// Port of the rayint function
std::optional<Vector2> rayint(Vector2 a, Vector2 b, Vector2 c, Vector2 d) {
    Vector2 r = {b.x - a.x, b.y - a.y}; // Direction vector of ray AB
    Vector2 s = {d.x - c.x, d.y - c.y}; // Direction vector of ray CD

    float determinant = r.x * s.y - r.y * s.x;

    if (std::fabs(determinant) < 1e-6) {
        // Rays are parallel or coincident
        return std::nullopt; // No unique intersection
    }

    Vector2 delta = {c.x - a.x, c.y - a.y};

    // Solve for t1
    float t1 = (delta.x * s.y - delta.y * s.x) / determinant;

    // Calculate the intersection point (t1 is sufficient for infinite rays)
    return Vector2{a.x + t1 * r.x, a.y + t1 * r.y};
}
