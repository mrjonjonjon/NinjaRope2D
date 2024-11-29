#ifndef LOGGER_H
#define LOGGER_H

#include "raylib.h"
#include <deque>
#include <string>
#include <vector>

// Logger class
class Logger {
public:
    // Log entry structure
    struct Log {
        std::string message;
        bool isSticky;
    };

private:
    std::deque<Log> ephemeralLogs;  // Queue for ephemeral logs
    std::deque<Log> stickyLogs;     // Queue for sticky logs
    std::vector<Color> stickyColors; // List of colors for sticky logs
    size_t maxLogs;                 // Maximum number of logs in each queue
    Vector2 ephemeralPos;           // Position to start drawing ephemeral logs
    Vector2 stickyPos;              // Position to start drawing sticky logs
    int fontSize;                   // Font size for text
    int lineSpacing;                // Line spacing between log messages
    Color color;                    // Default color of log text

    // Helper function to generate a random color (can be customized)
    Color GetRandomColor() const;

public:
    // Constructor
    Logger(size_t maxLogs, Vector2 ephemeralPos, Vector2 stickyPos, int fontSize, int lineSpacing, Color color);

    // Add a log (default is ephemeral)
    void AddLog(const std::string& message, bool isSticky = false);

    // Draw all logs on the screen
    void Draw() const;

    // Clear only ephemeral logs
    void ClearEphemeralLogs();

    // Clear all logs, including sticky ones
    void ClearAllLogs();
};

#endif // LOGGER_H
