//
// Created by Alyssa Wang on 2025/11/12.
//

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct LeaderboardEntry {
    std::string time;
    std::string name;
    int totalSeconds;
    bool newRecord = false;
};

class Leaderboard {
public:
    Leaderboard();
    void LoadLeaderboard();
    void UpdateLeaderboard(const std::string& newName, long long newTimeSeconds);
    void SaveLeaderboard();
    std::string GetFormattedContent();

private:
    std::vector<LeaderboardEntry> entries;
    static const int MAX_ENTRIES = 5;

    int TimeToSeconds(const std::string& timeStr);
    std::string SecondsToTime(int totalSeconds);
};

#endif