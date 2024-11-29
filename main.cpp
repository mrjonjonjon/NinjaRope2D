#include <raylib.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <unordered_set>
#include <functional>
#include <cmath>
#include <optional>
#include <stdio.h>
#include <iostream>
#include <raymath.h>
#include "Vector2Operators.h"
#include <vector>
#include <cmath>
#include <limits>
#include <unordered_set>
#include <sstream>
#include "logger.h"

using namespace std;
    const int screenWidth = 800;
    const int screenHeight = 600;
    Logger logger(30, {10, 10}, {screenWidth - 210, 10}, 10, 5, BLACK);



void logvec(Vector2 sweptPoint,string text){
    logger.AddLog(text+to_string(sweptPoint.x)+" , "+to_string(sweptPoint.y),true);

}
// Define a hash function for Vector2
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
// Define the operator globally to print Vector2
std::ostream& operator<<(std::ostream& os, const Vector2& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}


#include <iomanip> // For better console formatting

void DebugDrawPoint(Vector2 point, Color color, const char* label) {
    DrawCircleV(point, 5, color);
    if (label) {
         std::ostringstream logStream;
        logStream << point << " "<<label;
        DrawText(logStream.str().c_str(), point.x + 6, point.y - 6, 10, color);
    }
}

void DebugDrawVector(Vector2 start, Vector2 end, Color color, const char* label) {
    DrawLineV(start, end, color);
    if (label) {
        Vector2 midPoint = {(start.x + end.x) / 2, (start.y + end.y) / 2};
        DrawText(label, midPoint.x + 6, midPoint.y - 6, 10, color);
    }
}

void DebugPrintVector(const char* name, Vector2 vec) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << name << ": (" << vec.x << ", " << vec.y << ")" << std::endl;
}




// Wrap function
void Wrap(std::vector<Vector2>& curPoints, std::vector<Vector2>& newPoints, Vector2 anchor,
          Vector2& oldPos, Vector2 newPos, const std::vector<std::vector<Vector2>>& polygons,
          std::unordered_set<Vector2>& forbidden) {
    int safetyCounter = 0;
        if(newPos==oldPos){return;}

    while (safetyCounter < 100) {
        safetyCounter++;

        auto a = TriangleSweep(curPoints, anchor, oldPos, newPos, polygons, forbidden,false);
        Vector2 sweptPoint;
        if(a.has_value()){
            sweptPoint=a.value();
             //printf("Swept: (%f, %f)\n", sweptPoint.x, sweptPoint.y);
            logvec(anchor," wrap:anchor");
            logvec(oldPos," wrap:oldPos");
            logvec(newPos," wrap:newPos");
             logger.AddLog("Swept: "+to_string(sweptPoint.x)+" , "+to_string(sweptPoint.y),true);
        }else{
            return;
        }


        newPoints.push_back(sweptPoint);
        auto x = rayint(anchor, sweptPoint, oldPos, newPos);
        if (x.has_value()){
            oldPos=(x.value());
        }
        anchor = sweptPoint;
    }
}

// Unwrap function
std::unordered_set<Vector2> Unwrap(std::vector<Vector2>& curPoints, std::vector<Vector2>& newPoints,
                                   Vector2& anchor, Vector2& oldPos, Vector2 newPos,
                                   const std::vector<std::vector<Vector2>>& polygons) {
    std::unordered_set<Vector2> forbidden;

    if (newPoints.size() <= 1) {
        return forbidden;
    }
    if(newPos==oldPos){return forbidden;}

    Vector2 stopPoint;
    Vector2 finalOldPos = oldPos;

    auto a  = TriangleSweep(curPoints, anchor, oldPos, newPos, polygons, forbidden,true);
    if (!a.has_value()) {
        stopPoint = newPos;
    } else {
        stopPoint = a.value();
    }
    logger.AddLog("StopPoint: "+to_string(stopPoint.x) + " , "+ to_string(stopPoint.y),true);
    while (newPoints.size() > 1) {
        Vector2 anchorToStop = (stopPoint - anchor);
        Vector2 anchorToOld = (oldPos - anchor);
        Vector2 anchorToNew = (newPos- anchor);


       

        Vector2 preAnchorLine = (newPoints[newPoints.size() - 1]- newPoints[newPoints.size() - 2]);

        float angleToOld= HighPrecisionSignedAngle(preAnchorLine, anchorToOld);
        float angleToStop =  HighPrecisionSignedAngle(preAnchorLine, anchorToStop);
        float product = angleToOld*angleToStop;
       // logger.AddLog("AngleToOld: "+ to_string(angleToOld),true);
        //logger.AddLog("AngleToStop: "+ to_string(angleToStop),true);
        //logger.AddLog("Product: "+ to_string(product),true);

                       

        if (product >= 0) break;
   
             logger.AddLog("Unswept: "+to_string(newPoints.back().x)+" , "+to_string(newPoints.back().y),true);

             Vector2 prevAnchor = newPoints.back();
            newPoints.pop_back();
        

        if (newPoints.size() <= 1) break;
        
        auto x = rayint(newPoints[newPoints.size() - 1], prevAnchor, oldPos, newPos);
        if(x.has_value()){
                finalOldPos = x.value();
        }
       
        anchor = newPoints.back();
    }
    DebugDrawPoint(finalOldPos,BLACK,"NEW OLD POS");
    oldPos = finalOldPos + normalized(newPos-oldPos)*0.01f;
    return forbidden;
}

// Process function
std::vector<Vector2> Process(std::vector<Vector2>& curPoints, Vector2 anchor, Vector2 oldPos,
                             Vector2 newPos, const std::vector<std::vector<Vector2>>& polygons) {
    if (oldPos.x == newPos.x && oldPos.y == newPos.y) {
        return curPoints;
    }

    std::vector<Vector2> newPoints = curPoints;
    Vector2 playerPos = newPoints.back();
    newPoints.pop_back();

    std::unordered_set<Vector2> forbidden = Unwrap(curPoints, newPoints, anchor, oldPos, newPos, polygons);
    Wrap(curPoints, newPoints, anchor, oldPos, newPos, polygons, forbidden);

    newPoints.push_back(playerPos);
    return newPoints;
}
float dist(Vector2 v1,Vector2 v2){
    return  std::sqrt((v1.x-v2.x) * (v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y));
}




#include <random>

std::vector<std::vector<Vector2>> GenerateRandomPolygons(int count, float minSize, float maxSize, float screenWidth, float screenHeight, float minDistance) {
    std::vector<std::vector<Vector2>> polygons;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> positionDistX(minSize, screenWidth - maxSize);
    std::uniform_real_distribution<float> positionDistY(minSize, screenHeight - maxSize);
    std::uniform_real_distribution<float> sizeDist(minSize, maxSize);
    std::uniform_int_distribution<int> vertexCountDist(3, 6); // 3 to 6 vertices per polygon

    for (int i = 0; i < count; ++i) {
        bool validPolygon = false;
        std::vector<Vector2> polygon;

        while (!validPolygon) {
            // Generate random center and size
            float centerX = positionDistX(rng);
            float centerY = positionDistY(rng);
            float radius = sizeDist(rng);
            int vertexCount = vertexCountDist(rng);

            // Create polygon vertices (regular polygon with small randomness for variation)
            polygon.clear();
            for (int j = 0; j < vertexCount; ++j) {
                float angle = (2.0f * PI * j) / vertexCount;
                float offsetX = radius * cosf(angle) * (0.8f + 0.4f * (rng() / float(rng.max()))); // Add slight randomization
                float offsetY = radius * sinf(angle) * (0.8f + 0.4f * (rng() / float(rng.max())));
                polygon.push_back({centerX + offsetX, centerY + offsetY});
            }

            // Check if this polygon overlaps with existing polygons
            validPolygon = true;
            for (const auto& existingPolygon : polygons) {
                for (const auto& vertex : polygon) {
                    for (const auto& existingVertex : existingPolygon) {
                        if (Vector2Distance(vertex, existingVertex) < minDistance) {
                            validPolygon = false;
                            break;
                        }
                    }
                    if (!validPolygon) break;
                }
                if (!validPolygon) break;
            }
        }

        // Add valid polygon to the list
        polygons.push_back(polygon);
    }

    return polygons;
}

int main() {
    InitWindow(screenWidth, screenHeight, "2D Rope Debugging Visualization");

    Vector2 anchor = {(float)screenWidth / 2, (float)screenHeight / 2};
    Vector2 oldPos = anchor;
    std::vector<Vector2> curPoints = {anchor, anchor + Vector2{0,10}};
    
    // Generate a lot of random polygons
    std::vector<std::vector<Vector2>> polygons = GenerateRandomPolygons(20, 30.0f, 80.0f, screenWidth, screenHeight, 20.0f);

    float zoom = 1.0f;  // Default zoom (1.0 is normal scale)
    bool updateCurPoints = true;  // Flag to control whether to update the rope points

    SetTargetFPS(60);
    Vector2 cursorPos;
    Vector2 newPos;

    while (!WindowShouldClose()) {
        // Clear logger for fresh logs
        logger.ClearEphemeralLogs();
        
        // Handle zooming with mouse wheel (trackpad gesture equivalent)
        float zoomChange = GetMouseWheelMove();  // Positive value for zoom in, negative for zoom out
        if (zoomChange != 0.0f) {
            zoom += zoomChange * 0.1f;  // Adjust zoom speed
            zoom = std::max(0.1f, zoom); // Prevent zooming out too much
        }

        if (IsKeyPressed(KEY_P)) {
            updateCurPoints = !updateCurPoints;  // Toggle the update status
        }

        if (updateCurPoints) {
            oldPos = curPoints.back();
            anchor = curPoints[curPoints.size() - 2];    
            newPos = GetMousePosition();
            Vector2 zoomedCursorPos = Vector2Scale(newPos, 1.0f / zoom);  // Scale mouse position by zoom
            curPoints[curPoints.size()-1] = zoomedCursorPos;

            if (curPoints.size() >= 3) {
                Vector2 preAnchor = curPoints[curPoints.size() - 3];
                Vector2 preAnchorLine = (anchor - preAnchor);
                Vector2 toOldPos = (oldPos - anchor);
                Vector2 toNewPos = (newPos - anchor);

                if (!AreVectorsParallel(preAnchorLine, toNewPos)) {
                    curPoints = Process(curPoints, anchor, oldPos, newPos, polygons);
                }
            } else {
                curPoints = Process(curPoints, anchor, oldPos, newPos, polygons);
            }
        }

        if (IsKeyPressed(KEY_C)) {
            logger.ClearAllLogs();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw polygons with zoom effect
        for (auto& polygon : polygons) {
            for (size_t i = 0; i < polygon.size(); i++) {
                Vector2 p1 = Vector2Scale(polygon[i], zoom);
                Vector2 p2 = Vector2Scale(polygon[(i + 1) % polygon.size()], zoom);
                DrawLineV(p1, p2, BLACK);
            }
        }

        // Draw rope
        for (size_t i = 1; i < curPoints.size(); i++) {
            Vector2 prevPoint = Vector2Scale(curPoints[i - 1], zoom);
            Vector2 currPoint = Vector2Scale(curPoints[i], zoom);
            DrawLineV(prevPoint, currPoint, BLUE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
