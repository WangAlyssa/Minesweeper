//
// Created by Alyssa Wang on 2025/11/12.
//

#include <algorithm>
#include "Leaderboard.h"
#include <iostream>

int Leaderboard::TimeToSeconds(const std::string& timeStr) {
    if (timeStr.length() < 5) return 9999;
    try {
        int minutes = std::stoi(timeStr.substr(0, 2));
        int seconds = std::stoi(timeStr.substr(3, 2));
        return minutes * 60 + seconds;
    } catch (...) {
        return 9999;
    }
}

std::string Leaderboard::SecondsToTime(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    std::string mStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
    std::string sStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);
    return mStr + ":" + sStr;
}

Leaderboard::Leaderboard() {
    LoadLeaderboard();
}

void Leaderboard::LoadLeaderboard() {
    entries.clear();
    std::ifstream file("files/leaderboard.txt");
    std::string line;
    if (!file.is_open()) return;

    while (std::getline(file, line) && entries.size() < MAX_ENTRIES) {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            LeaderboardEntry entry;
            entry.time = line.substr(0, commaPos);
            entry.name = line.substr(commaPos + 1);
            entry.totalSeconds = TimeToSeconds(entry.time);
            entry.newRecord = false;
            entries.push_back(entry);
        }
    }
    file.close();
}

void Leaderboard::UpdateLeaderboard(const std::string& newName, long long newTimeSeconds) {
    LeaderboardEntry newEntry;
    newEntry.name = newName;
    newEntry.totalSeconds = (int)newTimeSeconds;
    newEntry.time = SecondsToTime((int)newTimeSeconds);
    newEntry.newRecord = true;

    auto it = entries.begin();
    while (it != entries.end() && it->totalSeconds <= newEntry.totalSeconds) {
        it++;
    }

    if (entries.size() < MAX_ENTRIES || it != entries.end()) {
        entries.insert(it, newEntry);
        if (entries.size() > MAX_ENTRIES) {
            entries.pop_back();
        }
        SaveLeaderboard();
    }
}

void Leaderboard::SaveLeaderboard() {
    std::ofstream file("files/leaderboard.txt");
    for (const auto& entry : entries) {
        file << entry.time << "," << entry.name << "\n";
    }
    file.close();
}

std::string Leaderboard::GetFormattedContent() {
    std::string content = "LEADERBOARD\n\n";

    for (size_t i = 0; i < entries.size(); ++i) {
        content += std::to_string(i + 1) + ".\t";
        content += entries[i].time + "\t";
        content += entries[i].name;
        if (entries[i].newRecord) {
            content += "*";
        }
        content += "\n\n";
    }
    return content;
}