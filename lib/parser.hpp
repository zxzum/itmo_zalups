#pragma once
#include "types.hpp"
#include <map>
#include <string>

struct ScoreData {
    double bpm = 120;
    std::map<std::string, InstrumentCfg> instruments;
    std::map<std::string, Pattern> patterns;
};

bool parseScore(const std::string& path, ScoreData& score);
double computePatternLen(const std::string& pname, std::map<std:: string, Pattern>& pats);