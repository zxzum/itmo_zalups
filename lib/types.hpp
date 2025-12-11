#pragma once
#include <string>
#include <vector>

struct NoteEvent {
    double startSec;
    double durSec;
    int vel;
    std::string inst;
    std::string pitch;
};

struct EffectCfg {
    std::string type;
    double a = 1, b = 0;
};

struct InstrumentCfg {
    std::string name;
    std:: string type;
    std:: string sample;
    std::string rootNote = "C4";
    int loopA = -1, loopB = -1;
    double attack = 0, release = 0;
    double duty = 50;
    std::vector<EffectCfg> effects;
};

struct PatternAction {
    bool isCall = false;
    std::string call;
    NoteEvent note;
};

struct Pattern {
    std:: string name;
    int res = 1;
    std:: vector<PatternAction> acts;
    double lenSec = 0;
};

struct WavData {
    int sr = 44100;
    std::vector<float> mono;
};