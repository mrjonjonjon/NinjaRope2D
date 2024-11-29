#include "logger.h"
#include <cstdlib>  // For rand()

// Helper function to generate a random color (can be customized)
// Helper function to generate a random dark color
Color Logger::GetRandomColor() const {
    // Generate random RGB values for dark colors (limited to 0-128)
    return Color{
        (unsigned char)(rand() % 128),  // Random red component (dark)
        (unsigned char)(rand() % 128),  // Random green component (dark)
        (unsigned char)(rand() % 128),  // Random blue component (dark)
        255                             // Full opacity
    };
}


// Constructor
Logger::Logger(size_t maxLogs, Vector2 ephemeralPos, Vector2 stickyPos, int fontSize, int lineSpacing, Color color)
    : maxLogs(maxLogs), ephemeralPos(ephemeralPos), stickyPos(stickyPos), fontSize(fontSize), lineSpacing(lineSpacing), color(color) {}

// Add a log
void Logger::AddLog(const std::string& message, bool isSticky) {
    if (isSticky) {
        stickyLogs.push_back({message, true});
        stickyColors.push_back(GetRandomColor());  // Assign a random color for the sticky log

        if (stickyLogs.size() > maxLogs) {
            stickyLogs.erase(stickyLogs.begin());  // Erase the oldest sticky log
            stickyColors.erase(stickyColors.begin());  // Remove the corresponding color
        }
    } else {
        ephemeralLogs.push_back({message, false});

        if (ephemeralLogs.size() > maxLogs) {
            ephemeralLogs.erase(ephemeralLogs.begin());  // Erase the oldest ephemeral log
        }
    }
}

// Draw all logs on the screen
void Logger::Draw() const {
    float yOffsetEphemeral = ephemeralPos.y;
    float yOffsetSticky = stickyPos.y;

    // Draw sticky logs at the top-right with different colors
    for (size_t i = 0; i < stickyLogs.size(); ++i) {
        DrawText(stickyLogs[i].message.c_str(), GetScreenWidth() - MeasureText(stickyLogs[i].message.c_str(), fontSize) - 10, yOffsetSticky, fontSize, stickyColors[i]);
        yOffsetSticky += fontSize + lineSpacing;
    }

    // Draw ephemeral logs at the top-left
    for (const auto& log : ephemeralLogs) {
        DrawText(log.message.c_str(), ephemeralPos.x, yOffsetEphemeral, fontSize, color);
        yOffsetEphemeral += fontSize + lineSpacing;
    }
}

// Clear only ephemeral logs
void Logger::ClearEphemeralLogs() {
    ephemeralLogs.clear();
}

// Clear all logs, including sticky ones
void Logger::ClearAllLogs() {
    ephemeralLogs.clear();
    stickyLogs.clear();
    stickyColors.clear();
}
